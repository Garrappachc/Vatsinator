/*
    airporttableview.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AIRPORTTABLEVIEW_H
#define AIRPORTTABLEVIEW_H

#include <QTableView>

class AirportTableModel;

class AirportTableView : public QTableView {
  
  /*
   * This class is used to represent nicely the AirportTableModel in
   * the QTableView format.
   */
  
  Q_OBJECT
  
public:
  explicit AirportTableView(QWidget* = 0);
  
  void setModel(AirportTableModel*);
  
    
protected slots:
  void rowsInserted(const QModelIndex&, int, int);
  
private slots:
  /**
   * Re-sets buttons within given range.
   * If no range is given, whole view is updated.
   */
  void __updateButtons(int = -1, int = -1);
  
};

#endif // AIRPORTTABLEVIEW_H
