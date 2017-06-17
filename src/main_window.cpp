/*
 * Main window class
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "main_window.hpp"

#include <QTableView>


MainWindow::MainWindow():
    QMainWindow(),
    m_mdadmProcess(),
    m_mdadmController(&m_mdadmProcess),
    m_raidsView(nullptr),
    m_raidsModel()
{
    m_raidsView = new QTableView(this);
    m_raidsView->setModel(&m_raidsModel);

    setCentralWidget(m_raidsView);

    refreshArraysList();
}


MainWindow::~MainWindow()
{

}


void MainWindow::refreshArraysList()
{
    m_raidsModel.clear();

    m_mdadmController.listRaids([this](const std::vector<RaidInfo>& raids)
    {
        for(const RaidInfo& raid: raids)
        {
            QStandardItem* raid_device_item = new QStandardItem(raid.raid_device);
            QStandardItem* raid_type_item = new QStandardItem(raid.raid_type);
            QStandardItem* raid_blk_devices_item = new QStandardItem(raid.block_devices.join(", "));

            const QList<QStandardItem *> row = { raid_device_item, raid_type_item, raid_blk_devices_item };
            m_raidsModel.appendRow(row);
        }
    });
}
