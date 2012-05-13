/*
    ShowPilotButton.cpp
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

#include "ShowPilotButton.h"
#include "defines.h"

ShowPilotButton::ShowPilotButton(const Pilot* _p, QWidget* _parent) :
		QPushButton("Show", _parent),
		__current(_p) {
	connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
}

void
ShowPilotButton::__handleClicked() {
	emit clicked(__current);
}
