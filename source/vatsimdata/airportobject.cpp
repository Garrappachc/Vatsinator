/*
    airportobject.cpp
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


#include "db/firsdatabase.h"

#include "ui/mapwidget/mapwidget.h"

#include "airportobject.h"
#include "defines.h"

AirportObject::AirportObject(const QString& _icao) :
		__data(AirportsDatabase::GetSingleton().find(_icao)) {
#ifndef NO_DEBUG
	if (!__data)
		qDebug() << "Airport " << _icao << " not found.";
#endif
	if (__data) {
		Fir* f = FirsDatabase::GetSingleton().findFirByIcao(__data->fir);
		if (f)
			f->addAirport(this);
	}
	
	__generateTip();
}

AirportObject::~AirportObject() {
	MapWidget::deleteImage(labelTip);
}

void
AirportObject::addStaff(const Controller* _c) {
	__staff.push_back(_c);
}

void
AirportObject::addInbound(const Pilot* _p) {
	__inbounds.push_back(_p);
}

void
AirportObject::addOutbound(const Pilot* _p) {
	__outbounds.push_back(_p);
}

unsigned
AirportObject::countDepartures() const {
	unsigned i = 0;
	for (const Pilot* p: __outbounds)
		if (p->flightStatus == DEPARTING)
			++i;
	
	return i;
}

unsigned
AirportObject::countArrivals() const {
	unsigned i = 0;
	for (const Pilot* p: __inbounds)
		if (p->flightStatus == ARRIVED)
			++i;
	
	return i;
}

bool
AirportObject::hasApproach() const {
	for (const Controller* c: __staff)
		if (c->facility == APP)
			return true;
	
	return false;
}

unsigned
AirportObject::getFacilities() const {
	unsigned facilities = 0;
	for (const Controller* c: __staff) {
		facilities |= c->facility;
	}
	
	return facilities;
}

void
AirportObject::__generateTip() {
	if (!__data) {
		labelTip = 0;
		return;
	}
	
	QImage temp(MapWidget::GetSingleton().getAirportToolTipBackground());
	QPainter painter(&temp);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.setFont(MapWidget::GetSingleton().getAirportFont());
	painter.setPen(QColor(AIRPORTS_LABELS_FONT_COLOR));
	QRect rectangle(0, 0, 48, 12); // size of the tooltip.png
	painter.drawText(rectangle, Qt::AlignCenter, static_cast< QString >(__data->icao));
	labelTip = MapWidget::loadImage(temp);
}
