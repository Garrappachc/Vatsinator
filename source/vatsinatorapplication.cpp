/*
    vatsinatorapplication.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "events/decisionevent.h"
#include "models/metarlistmodel.h"
#include "network/metarupdater.h"
#include "network/plaintextdownloader.h"
#include "network/resourcemanager.h"
#include "network/statspurveyor.h"
#include "storage/cachefile.h"
#include "storage/languagemanager.h"
#include "storage/settingsmanager.h"
#include "ui/vatsinatorstyle.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "storage/filemanager.h"
#include "config.h"

#ifdef Q_OS_ANDROID
# include "ui/quickuserinterface.h"
#else
# include "ui/widgetsuserinterface.h"
#endif

#include "vatsinatorapplication.h"

VatsinatorApplication::VatsinatorApplication(int& argc, char** argv) :
#ifndef Q_OS_ANDROID
    QApplication(argc, argv),
#else
    QGuiApplication(argc, argv),
#endif
    __userInterface(UserInterface::instantiate(this)),
    __fileManager(new FileManager),
    __settingsManager(new SettingsManager(this)),
    __airlineDatabase(new AirlineDatabase(this)),
    __airportDatabaase(new AirportDatabase(this)),
    __firDatabase(new FirDatabase(this)),
    __metarUpdater(new MetarUpdater(new MetarListModel(this), this)),
    __vatsimData(new VatsimDataHandler(this)),
    __languageManager(new LanguageManager),
    __resourceManager(new ResourceManager()),
    __statsPurveyor(new StatsPurveyor())
{

    /* Set up translations */
    QString locale = SettingsManager::earlyGetLocale();
    __loadTranslation(locale);
    
#ifndef Q_OS_ANDROID
    setStyle(new VatsinatorStyle());
#endif
    
    connect(this, &VatsinatorApplication::initializing, &VatsinatorApplication::__initialize);
    emit initializing();
}

VatsinatorApplication::~VatsinatorApplication()
{
    std::for_each(__threads.begin(), __threads.end(), [](auto t) {
        t->quit();
    });
    
    std::for_each(__threads.begin(), __threads.end(), [](auto t) {
        t->wait();
    });
}

LanguageManager*
VatsinatorApplication::languageManager()
{
    Q_ASSERT(__languageManager.data());
    return __languageManager.data();
}

void
VatsinatorApplication::restart()
{
    /* http://stackoverflow.com/questions/5129788/how-to-restart-my-own-qt-application */
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void
VatsinatorApplication::customEvent(QEvent* event)
{
    if (event->type() == Event::Decision)
        userDecisionEvent(static_cast<DecisionEvent*>(event));
}

void
VatsinatorApplication::userDecisionEvent(DecisionEvent* event)
{
    if (event->context() == QStringLiteral("statistics")) {
        statsPurveyor()->setUserDecision(
            event->decision() == DecisionEvent::Accepted ? StatsPurveyor::Accepted : StatsPurveyor::Declined
        );
    }
}

void
VatsinatorApplication::__initialize()
{
    qDebug("VatsinatorApplication: initializing");
    
    /* Create windows */
    __userInterface->initialize();
    
    /* Initialize everything else */
    __airlineDatabase->initialize();
    __airportDatabaase->initialize();
    __firDatabase->initialize();
    
    /* Read data files only after databases are ready */
    __vatsimData->initialize();
    
    /* Thread for ResourceManager */
    QThread* rmThread = new QThread(this);
    rmThread->setObjectName("ResourceManager thread");
    __resourceManager->moveToThread(rmThread);
    rmThread->start();
    connect(this, &QCoreApplication::aboutToQuit, __resourceManager, &QObject::deleteLater);
    __threads << rmThread;
    
    /* Thread for StatsPurveyor */
    QThread* spThread = new QThread(this);
    spThread->setObjectName("StatsPurveyor thread");
    __statsPurveyor->moveToThread(spThread);
    spThread->start();
    connect(this, &QCoreApplication::aboutToQuit, __statsPurveyor, &QObject::deleteLater);
    __threads << spThread;
    
    /* Initialize statistics */
    QSettings s;
    
    if (!s.contains("Decided/stats"))   // no decision made yet
        __userInterface->showStatsDialog();
}

void
VatsinatorApplication::__loadTranslation(const QString& locale)
{
    std::for_each(__translators.begin(), __translators.end(), [this](auto tr) {
        removeTranslator(tr);
    });
    
    qDeleteAll(__translators);
    __translators.clear();
    
    QTranslator* tr_qt = new QTranslator(this);
    bool result = tr_qt->load(QString("qt_") % locale, FileManager::staticPath(FileManager::Translations));
    if (result) {
        installTranslator(tr_qt);
        __translators << tr_qt;
    } else {
        qDebug() << "Failed loading" << locale << "translation for Qt";
        tr_qt->deleteLater();
    }
    
    QTranslator* tr = new QTranslator(this);
    result = tr->load(QString("vatsinator-") % locale, FileManager::staticPath(FileManager::Translations));
    Q_ASSERT(result);
    installTranslator(tr);
    __translators << tr;
}
