/*
 * euroutenotamprovider.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef EUROUTENOTAMPROVIDER_H
#define EUROUTENOTAMPROVIDER_H

#include <QDateTime>
#include <QMap>

#include "network/abstractnotamprovider.h"

class EurouteNotamProvider : public AbstractNotamProvider {
  
  /**
   * This is EUroute NOTAM provider class.
   * http://notams.euroutepro.com/doc/
   * 
   * To avoid excessive downloads, the XML file is cached and
   * fetched only once per 24h.
   */
  
  Q_OBJECT

public:
  EurouteNotamProvider();
  virtual ~EurouteNotamProvider();
  
  void fetchNotam(const QString&) override;
  QString providerInfo() const override;
  
private:
  void __fetchXml();
  void __readXmlHeader();
  void __fillNotamListModel(NotamListModel*);
  
private slots:
  void __xmlReady(QString);
  void __checkXmlUpToDate();
  
private:
  
  QDateTime __lastUpdate;
  
  /* ICAO <-> model map */
  QMap<QString, NotamListModel*> __notamModels;
  
  QString __icaoRequest;
  
};

#endif // EUROUTENOTAMPROVIDER_H