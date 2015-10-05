/*
    atclistwindow.cpp
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

#include <QtWidgets>

#include "models/atctablemodel.h"
#include "models/roles.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "atclistwindow.h"

AtcListWindow::AtcListWindow(QWidget* parent) :
    BaseWindow(parent)
{
    setupUi(this);
    AtcTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    connect(AtcTable, &QTableView::doubleClicked, [this](const QModelIndex& index) {
        Q_ASSERT(index.data(InstancePointerRole).isValid());
        Client* const client = reinterpret_cast<Client* const>(index.data(InstancePointerRole).value<void*>());
        wui()->showClientDetails(client);
    });
}

void
AtcListWindow::showEvent(QShowEvent* event)
{
    AtcTable->setModel(vApp()->vatsimDataHandler()->atcs());   
    BaseWindow::showEvent(event);
}
