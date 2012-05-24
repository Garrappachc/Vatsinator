/*
    uir.cpp
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

#include "uir.h"
#include "defines.h"

void
Uir::addFir(Fir* _f) {
	__range.push_back(_f);
}

void
Uir::addStaff(const Controller* _c) {
	for (Fir* f: __range)
		f->addUirStaff(_c);
	__staff.push_back(_c);
}
