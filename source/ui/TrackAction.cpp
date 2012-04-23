/*
    TrackAction.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "TrackAction.h"
#include "defines.h"

TrackAction::TrackAction(const Pilot* _pilot, QObject* _parent) :
		QAction("Track this flight", _parent),
		__current(_pilot) {
	
	connect(this, SIGNAL(triggered()), this, SLOT(handleTriggered()));
}

void
TrackAction::handleTriggered() {
	emit clicked(__current);
}