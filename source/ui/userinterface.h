/*
    userinterface.h
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


#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>

#include "ui/widgets/mapwidget.h"
#include "singleton.h"

class AboutWindow;
class Airport;
class AtcListWindow;
class Client;
class DatabaseWindow;
class Fir;
class FlightListWindow;
class Pilot;
class MetarsWindow;
class NotificationEvent;
class SettingsWindow;
class VatsinatorWindow;

class UserInterface : public QObject, public Singleton<UserInterface> {
  Q_OBJECT

public:
  UserInterface(QObject* = nullptr);
  virtual ~UserInterface();
  
  /**
   * Initializes UI components.
   */
  void init();
  
  /**
   * @return Instance of the "About Vatsinator" window.
   */
  AboutWindow* aboutWindow();
  
  /**
   * @return Instance of the "Atc list" window.
   */
  AtcListWindow* atcListWindow();
  
  /**
   * @return Instance of the "VatsinatorDatabase" window.
   */
  DatabaseWindow* databaseWindow();
  
  /**
   * @return Instance of the "Flight list" window.
   */
  FlightListWindow* flightListWindow();
  
  /**
   * @return Instance of the "METARs" window.
   */
  MetarsWindow* metarsWindow();
  
  /**
   * @return Instance of the settings window.
   */
  SettingsWindow* settingsWindow();
  
  /**
   * @return Instance of the main Vatsinator window.
   */
  VatsinatorWindow* mainWindow();
  
  /**
   * Custom events handling.
   */
  bool event(QEvent*) override;
  
public slots:
  /**
   * Reports fatal error to user.
   * 
   * @param msg Message to be shown.
   */
  void fatal(const QString&);
  
  /**
   * Reports warning to user.
   * 
   * @param msg Warning message.
   */
  void warning(const QString&);
  
  void showAppRestartDialog();
  void showVatsimMessage(const QString&);
  
  void showDetailsWindow(const Airport*);
  void showDetailsWindow(const Client*);
  void showDetailsWindow(const Fir*);
  
protected:
  virtual bool notificationEvent(NotificationEvent*);
  
private slots:
  void __statusError();
  void __dataError();
  void __showNewVersionDialog();
  
private:

  AboutWindow*          __aboutWindow;
  MetarsWindow*         __metarsWindow;
  DatabaseWindow*       __databaseWindow;
  AtcListWindow*        __atcListWindow;
  FlightListWindow*     __flightsListWindow;
  SettingsWindow*       __settingsWindow;
  VatsinatorWindow*     __vatsinatorWindow;

};

#endif // USERINTERFACE_H
