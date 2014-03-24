/*
 * notamlistmodel.cpp
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

#include <QtGui>

#include "notamlistmodel.h"
#include "defines.h"

NotamListModel::NotamListModel(QString _icao, QObject* _parent) :
  QAbstractTableModel(_parent),
  __icao(std::move(_icao)) {}

void
NotamListModel::addNotam(Notam _notam) {
  __notams << std::move(_notam);
}

QVariant
NotamListModel::data(const QModelIndex& _index, int _role) const {
  if (_index.column() != 0)
    return QVariant();
  
  switch (_role) {
    case Qt::DisplayRole:
      return QString("%1 %2 %3").arg(
          __notams.at(_index.row()).icao(),
          __notams.at(_index.row()).ident(),
          __notams.at(_index.row()).notam()
        );
      
    case Qt::ToolTipRole: {
      QString to;
      if (__notams.at(_index.row()).to().isValid()) {
        to = __notams.at(_index.row()).to().toString("yyyy-MM-dd hh:mm:ss");
        if (__notams.at(_index.row()).cflag() == Notam::Est)
          to += " <strong>EST</strong>";
      } else {
        to = "<strong>PERM</strong>";
      }
      
      return QString("<p style='white-space:nowrap'>Effective from %1 until %2</p>").arg(
        __notams.at(_index.row()).from().toString("yyyy-MM-dd hh:mm:ss"),
        to
      );
    }
    
    case Qt::BackgroundRole:
      switch (__notams.at(_index.row()).type()) {
        
        case Notam::Cancellation:
          return QBrush(QColor(255, 177, 177));
          
        default:
          return QVariant();
      }
      
    default:
      return QVariant();
  }
}

int
NotamListModel::rowCount(const QModelIndex&) const {
  return __notams.size();
}

int
NotamListModel::columnCount(const QModelIndex&) const {
  return 1;
}

void
NotamListModel::sort(int _column, Qt::SortOrder _order) {
  if (_column != 0)
    return;
  
  qSort(__notams.begin(), __notams.end());
}
