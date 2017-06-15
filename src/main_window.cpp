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

#include <QListWidget>


MainWindow::MainWindow():
    QMainWindow(),
    m_mdadmProcess(),
    m_mdadmController(&m_mdadmProcess),
    m_arrays(nullptr)
{
    m_arrays = new QListWidget(this);

    setCentralWidget(m_arrays);

    refreshArraysList();
}


MainWindow::~MainWindow()
{

}


void MainWindow::refreshArraysList()
{
    m_arrays->clear();

    m_mdadmController.listRaids([this](const std::vector<RaidInfo>& raids)
    {
        for(const RaidInfo& raid: raids)
        {
            const QString& raid_device = raid.raid_device;
            m_arrays->addItem(raid_device);
        }
    });
}
