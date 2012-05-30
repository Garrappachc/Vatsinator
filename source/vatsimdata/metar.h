/*
    metar.h
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


#ifndef METAR_H
#define METAR_H

#include <QString>
#include <QDate>
#include <QTime>

class Metar {
	
public:
	
	Metar();
	Metar(const QString&, const QString& = "");
	
	inline void
	setMetar(const QString& _m) { __metar = _m; }
	
	inline const QString &
	getIcao() const { return __icao; }
	
	inline const QString &
	getMetar() const { return __metar; }
	
private:
	
	QString	__icao;
	QString	__metar;
	QDate	__lastFetchedDate;
	QTime	__lastFetchedTime;
	
};

#endif // METAR_H
