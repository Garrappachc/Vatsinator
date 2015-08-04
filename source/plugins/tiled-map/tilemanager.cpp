/*
 * tilemanager.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtCore>

#include "network/filedownloader.h"
#include "storage/filemanager.h"
#include "ui/map/maprenderer.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "tile.h"
#include "tilereadyevent.h"

#include "tilemanager.h"

Q_CONSTEXPR quint32 TileWidth = 256; /* in pixels */
Q_CONSTEXPR int Downloaders = 4; /* number of tile downloaders */

TileManager::TileManager(MapRenderer* renderer, QObject* parent) :
    QObject(parent),
    __renderer(renderer),
    __tileZoom(1)
{
    Q_ASSERT(renderer);
    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__calculateTileZoom);
    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__calculateTileZoom);
    
    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__updateTileList);
    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__updateTileList);
    connect(renderer, &MapRenderer::centerChanged, this, &TileManager::__updateTileList);
}

void
TileManager::initialize()
{
    for (int i = 0; i < Downloaders; ++i) {
        FileDownloader* d = new FileDownloader(this);
        connect(d, &FileDownloader::finished, this, &TileManager::__tileDownloaded);
        __downloaders << d;
    }
}

void
TileManager::fetchTile(const TileUrl& url)
{
    Q_ASSERT(url.zoom() > 0);
    Q_ASSERT(this->thread() == QThread::currentThread()); /* This method is not thread-safe */
    
    auto it = std::min_element(__downloaders.begin(), __downloaders.end(), [this](FileDownloader* a, FileDownloader* b)  {
        return a->tasks() < b->tasks();
    });
    
    if ((*it)->tasks() > 0) {
        __tileQueue << url;
    } else {
        (*it)->setProperty("tileUrl", QVariant::fromValue<TileUrl>(url));
        (*it)->fetch(url.toUrl());
    }
}

QList<Tile*>
TileManager::tilesForCurrentZoom()
{   
    auto topLeft = tileCoordForLonLat(__renderer->screen().topLeft());
    auto bottomRight = tileCoordForLonLat(__renderer->screen().bottomRight());
    
    if (bottomRight.first < topLeft.first)
        bottomRight = tileCoordForLonLat(LonLat(180.0, __renderer->screen().bottomRight().y()));
    
    __mutex.lock();
    
    TileMap& tiles = __tiles[__tileZoom];
    QList<Tile*> l;
    
    for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
        for (auto j = topLeft.second; j <= bottomRight.second; ++j) {
            auto it = tiles.find(TileCoord(i, j));
            
            if (it != tiles.end()) {
                l << it->second;
            }
        }
    }
    
    __mutex.unlock();
    return l;
}

QPair<quint64, quint64>
TileManager::tileCoordForLonLat(const LonLat& lonLat)
{
    return tileCoordForLonLat(lonLat, __tileZoom);
}

Tile*
TileManager::tile(quint64 z, quint64 x, quint64 y)
{
    Q_ASSERT(z > 0); 
    TileMap& tiles = __tiles[z];
    auto it = tiles.find(TileCoord(x, y));
    if (it == tiles.end())
        return nullptr;
    else
        return it->second;
}

QPair<quint64, quint64>
TileManager::tileCoordForLonLat(const LonLat& lonLat, unsigned zoom)
{
    LonLat ll = lonLat.bound();
    qreal n = qPow(2.0, static_cast<qreal>(zoom));
    quint64 x = qFloor((ll.longitude() + 180.0) / 360.0 * n);
    qreal lat_rad = qDegreesToRadians(ll.latitude());
    quint64 y = qFloor((1.0 - qLn(qTan(lat_rad) + (1.0 / qCos(lat_rad))) / M_PI) / 2.0 * n);
    
    return qMakePair(x, y);
}

void
TileManager::__calculateTileZoom()
{
    LonLat p1 = __renderer->mapToLonLat(QPoint(0, 0));
    LonLat p2 = __renderer->mapToLonLat(QPoint(TileWidth, 0));
    
    qreal dist = VatsimDataHandler::nmDistance(p2, p1);
    qreal nmPerPix = dist / TileWidth;
    
    int zoom = 0;
    qreal resolution;
    do {
        zoom += 1;
        resolution = 84.5264761719 * qCos(qDegreesToRadians(p1.latitude())) / (qPow(2.0, zoom));
    } while (resolution > nmPerPix);
    
    __tileZoom = zoom;
}

void
TileManager::__updateTileList()
{
    auto topLeft = tileCoordForLonLat(__renderer->screen().topLeft());
    auto bottomRight = tileCoordForLonLat(__renderer->screen().bottomRight());
    
    if (bottomRight.first < topLeft.first)
        bottomRight = tileCoordForLonLat(LonLat(180.0, __renderer->screen().bottomRight().y()));
    
    TileMap& tiles = __tiles[__tileZoom];
    
    for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
        for (auto j = topLeft.second; j <= bottomRight.second; ++j) {
            TileCoord coord(i, j);
            
            __mutex.lock();
            auto it = tiles.find(coord);
            
            if (it == tiles.end()) {
                Tile* tile = new Tile(i, j, __tileZoom, this);
                connect(tile, &Tile::ready, __renderer, &MapRenderer::updated);
                tiles.insert(std::make_pair(coord, tile));
            }
            __mutex.unlock();
        }
    }
}

void
TileManager::__tileDownloaded(const QString& fileName, const QUrl& url)
{
    FileDownloader* downloader = qobject_cast<FileDownloader*>(sender());
    Q_ASSERT(downloader);
    Q_ASSERT(downloader->property("tileUrl").isValid());
    Q_ASSERT(downloader->property("tileUrl").canConvert<TileUrl>());
    
    TileUrl tileUrl = downloader->property("tileUrl").value<TileUrl>();
    if (!tileUrl.isValid())
        return;
    
    FileManager::moveToCache(fileName, tileUrl.path());
    
    TileMap& tiles = __tiles[tileUrl.zoom()];
    auto it = std::find_if(tiles.begin(), tiles.end(), [&tileUrl](const std::pair<const TileCoord&, const Tile*>& it) {
        return tileUrl == it.second->url();
    });
    
    Q_ASSERT(it != tiles.end());
    
    TileReadyEvent e;
    qApp->sendEvent(it->second, &e);
    
    Q_ASSERT(downloader->tasks() <= 1); /* One downloader - one task at a time */
    if (__tileQueue.length() > 0) {
        int i = qrand() % __tileQueue.count();
        TileUrl url = __tileQueue.takeAt(i);
        downloader->setProperty("tileUrl", QVariant::fromValue<TileUrl>(url));
        downloader->fetch(url.toUrl());
    }
    
    Q_UNUSED(url);
}
