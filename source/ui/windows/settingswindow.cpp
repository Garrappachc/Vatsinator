/*
    settingswindow.cpp
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

#include <QtWidgets>

#ifdef Q_OS_MAC
# include <QtMacExtras/QtMacExtras>
#endif

#include "storage/languagemanager.h"
#include "storage/settingsmanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/widgets/mapwidget.h"
#include "ui/userinterface.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/pages/networkpage.h"
#include "ui/pages/viewpage.h"
#include "ui/pages/mappage.h"
#include "vatsinatorapplication.h"

#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget* parent) : BaseWindow(parent) {
  setupUi(this);
  
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
  
#ifdef Q_OS_MAC
  __macToolBar = new QMacToolBar(this);
  CategoryList->setVisible(false);
  OKCancelButtonBox->setVisible(false);
  
  __macMapper = new QSignalMapper(this);
  connect(__macMapper, SIGNAL(mapped(int)), SwappingWidget, SLOT(setCurrentIndex(int)));
  
  connect(__macMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), [this](int i) {
    QMacToolBarItem* item = __macToolBar->items().at(i);
    setWindowTitle(item->text());
  });
  
  connect(SwappingWidget, &QStackedWidget::currentChanged, this, &SettingsWindow::__resizeToMinimum);
#else
  connect(CategoryList, SIGNAL(currentRowChanged(int)), SwappingWidget, SLOT(setCurrentIndex(int)));
  connect(OKCancelButtonBox,                    SIGNAL(clicked(QAbstractButton*)),
          this,                                 SLOT(__handleButton(QAbstractButton*)));
#endif
  
  {
    MiscellaneousPage* p = new MiscellaneousPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
    connect(p, &MiscellaneousPage::settingsChanged, this, &SettingsWindow::settingsChanged);
  }
  {
    NetworkPage* p = new NetworkPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
    connect(p, &NetworkPage::settingsChanged, this, &SettingsWindow::settingsChanged);
  }
  {
    ViewPage* p = new ViewPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
    connect(p, &ViewPage::settingsChanged, this, &SettingsWindow::settingsChanged);
  }
  {
    MapPage* p = new MapPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
    connect(p, &MapPage::settingsChanged, this, &SettingsWindow::settingsChanged);
  }

#ifdef Q_OS_MAC
  window()->winId();
  __macToolBar->attachToWindow(window()->windowHandle());
#endif
  
  connect(this, &SettingsWindow::settingsApplied, vApp()->settingsManager(), &SettingsManager::saveSettings);
  connect(this, &SettingsWindow::restoreDefaults, vApp()->settingsManager(), &SettingsManager::restoreDefaults);

#ifdef Q_OS_MAC
  connect(this, &SettingsWindow::settingsChanged, vApp()->settingsManager(), &SettingsManager::updateSettings);
#endif
}

void
SettingsWindow::closeEvent(QCloseEvent *event) {
#ifdef Q_OS_MAC
  emit settingsApplied();
#endif
  
  event->accept();
}

void
SettingsWindow::__addPage(const QString& element, const QString& icon, QWidget* page) {
  QIcon listIcon(icon);
  SwappingWidget->addWidget(page);
  
#ifdef Q_OS_MAC
  int i = __macToolBar->items().size();
  
  QMacToolBarItem* item = __macToolBar->addItem(listIcon, element);
  item->setSelectable(true);
  connect(item, SIGNAL(activated()), __macMapper, SLOT(map()));
  __macMapper->setMapping(item, i);
  
  if (i == 0)
    setWindowTitle(element);
  
#else
  
  QListWidgetItem *item = new QListWidgetItem(CategoryList);
  item->setIcon(listIcon);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  
  item->setText(element);
  
  if (CategoryList->count() == 1)
    item->setSelected(true);
#endif
}

void
SettingsWindow::__handleButton(QAbstractButton* button) {
  if (OKCancelButtonBox->button(QDialogButtonBox::RestoreDefaults) == button)
    emit restoreDefaults();
  else if (OKCancelButtonBox->button(QDialogButtonBox::Apply) == button
        || OKCancelButtonBox->button(QDialogButtonBox::Ok) == button)
    emit settingsApplied();
}

void
SettingsWindow::__resizeToMinimum() {
  int w = width();
  int h = minimumSizeHint().height();
  resize(w, h);
}

