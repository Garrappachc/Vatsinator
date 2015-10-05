/*
    userinterface.cpp
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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

#include "events/notificationevent.h"
#include "vatsinatorapplication.h"

#ifdef Q_OS_ANDROID
# include "ui/quickuserinterface.h"
#else
# include "ui/widgetsuserinterface.h"
#endif


#include "userinterface.h"

UserInterface::UserInterface(QObject* parent) :
    QObject(parent) {}

UserInterface*
UserInterface::instantiate(QObject* parent)
{
#ifdef Q_OS_ANDROID
    return new QuickUserInterface(parent);
#else
    return new WidgetsUserInterface(parent);
#endif
}

qreal
UserInterface::dp()
{
    return QGuiApplication::primaryScreen()->physicalDotsPerInch() / 160;
}

void
UserInterface::customEvent(QEvent* event)
{
    if (event->type() == Event::Notification)
        notificationEvent(static_cast<NotificationEvent*>(event));
    else if (event->type() == Event::Vatsim)
        vatsimEvent(static_cast<VatsimEvent*>(event));
}

void
UserInterface::notificationEvent(NotificationEvent* event)
{
    switch (event->gravity()) {
        case NotificationEvent::Fatal:
            fatal(event->message());
            vApp()->exit(1);
            break;
            
        case NotificationEvent::Warning:
            warning(event->message());
            break;
            
        case NotificationEvent::Information:
            /* TODO */
            break;
    }
}
