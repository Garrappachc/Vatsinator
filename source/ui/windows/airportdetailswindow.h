/*
    airportdetailswindow.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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


#ifndef AIRPORTDETAILSWINDOW_H
#define AIRPORTDETAILSWINDOW_H

#include "ui/windows/basewindow.h"

#include "ui/ui_airportdetailswindow.h"

class Airport;
class Controller;
class Pilot;
class NotamListModel;
class WeatherForecastModel;

/**
 * The AirportDetailsWindow class represents a window that shows up when user
 * clicks on airport icon
 */
class AirportDetailsWindow : public BaseWindow,
    private Ui::AirportDetailsWindow {

  Q_OBJECT

public:
  AirportDetailsWindow(const Airport* airport, QWidget* parent = nullptr);
  virtual ~AirportDetailsWindow();
  
protected:
  void showEvent(QShowEvent* event) override;

private:
  void __fillLabels();
  
private slots:
  void __updateForecast();
  void __notamUpdate(NotamListModel* model);
  void __goToNotam(QModelIndex index);

private:
  const Airport* __airport;

};

#endif // AIRPORTDETAILSWINDOW_H
