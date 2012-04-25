/*
    FirDetailsWindow.h
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


#ifndef FIRDETAILSWINDOW_H
#define FIRDETAILSWINDOW_H

#include "ui/ui_FirDetailsWindow.h"
#include "Singleton.h"

class Controller;
class Fir;
class Pilot;

class FirDetailsWindow : public QWidget, public Singleton< FirDetailsWindow >, private Ui::FirDetailsWindow {
	
	Q_OBJECT
	
public:
	FirDetailsWindow(QWidget* = 0);
	
signals:
	void showPilotRequest(const Pilot*);
	void showATCDetailsRequest(const Controller*);
	
public slots:
	void showWindow(const Fir*);
	void handleShowClicked(const Pilot*);
	void handleDetailsClicked(const Controller*);
	
private:
	void __updateContents(const Fir*);
	void __setWindowPosition();
	
	QString	__currentICAO;
	
private slots:
	void __updateContents();
	
	
};

#endif // FIRDETAILSWINDOW_H
