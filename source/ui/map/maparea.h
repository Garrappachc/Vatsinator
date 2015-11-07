/*
 * maparea.h
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

#ifndef MAPAREA_H
#define MAPAREA_H

#include <QObject>
#include <QRectF>
#include <QPainter>
#include "ui/map/worldtransform.h"

/**
 * The MapArea class is an abstract base for all map objects that
 * are represented as areas - for example FIRs or approach areas around
 * airports.
 */
class MapArea : public QObject {
    Q_OBJECT

public:
    explicit MapArea(QObject* parent = nullptr);
    
    /**
     * Defines the outer bounds of the area. The MapScene uses this
     * value to determine whether the given area is visible on the
     * screen.
     */
    virtual QRectF boundingRect() const = 0;
    
    /**
     * Determines whether the area is visible or not.
     * 
     * This function is called only if the screen rectangle contains
     * any point of \c boundingRect().
     */
    virtual bool isVisible() const = 0;
    
    /**
     * Draws the area on the painter.
     * 
     * \param painter The painter to have the area drawn onto.
     * \param transform The current world transformation.
     */
    virtual void draw(QPainter* painter, const WorldTransform& transform) const = 0;

};

#endif // MAPAREA_H