/*
    airportdetailsbutton.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include <QtGui>

#include "ui/windows/airportdetailswindow.h"

#include "vatsimdata/airport.h"

#include "airportdetailsbutton.h"
#include "defines.h"

AirportDetailsButton::AirportDetailsButton(QWidget* _parent) :
    QPushButton("", _parent),
    __current(NULL) {
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
  // we assume that autoconnect is on
  connect(this,                                     SIGNAL(clicked(const Airport*)),
          AirportDetailsWindow::getSingletonPtr(),  SLOT(show(const Airport*)));
}

AirportDetailsButton::AirportDetailsButton(const Airport* _ap,
                                     bool _autoconnect,
                                     QWidget* _parent) :
    QPushButton("", _parent),
    __current(_ap) {
  setIcon(QIcon(":/uiIcons/button-details.png"));
  
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));

  if (_autoconnect)
    connect(this,                                     SIGNAL(clicked(const Airport*)),
            AirportDetailsWindow::getSingletonPtr(),  SLOT(show(const Airport*)));
}

void
AirportDetailsButton::setAirportPointer(const Airport* _ap) {
  __current = _ap;
  setEnabled(_ap != NULL);
  setFlat(_ap == NULL);
}

void
AirportDetailsButton::__handleClicked() {
  if (__current)
    emit clicked(__current);
}
