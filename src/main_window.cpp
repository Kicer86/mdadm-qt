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
#include "create_raid_dialog.hpp"

#include <QTableView>
#include <QMenuBar>

MainWindow::MainWindow():
    QMainWindow(),
    m_mdadmProcess(),
    m_mdadmController(&m_mdadmProcess),
    m_raidsView(nullptr),
    m_raidsModel()
{
    m_raidsModel.setHorizontalHeaderLabels( { tr("raid device"), tr("type"), tr("block devices") } );

    m_raidsView = new QTableView(this);
    m_raidsView->setModel(&m_raidsModel);

    auto raidMenu = menuBar()->addMenu(tr("&Raid"));
    QAction *actionCreate = new QAction(tr("&New"));
    QAction *actionQuit = new QAction(tr("&Quit"));

    actionCreate->setShortcut(Qt::CTRL + Qt::Key_N);
    actionQuit->setShortcut(Qt::CTRL + Qt::Key_Q);

    connect(actionCreate, &QAction::triggered, this, &MainWindow::createRaid);
    connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);

    raidMenu->addAction(actionCreate);
    raidMenu->addAction(actionQuit);

    auto viewMenu = menuBar()->addMenu(tr("&View"));
    QAction *actionReload = new QAction(tr("&Reload"));

    actionReload->setShortcut(Qt::Key_F5);

    connect(actionReload, &QAction::triggered, this, &MainWindow::refreshArraysList);

    viewMenu->addAction(actionReload);

    setCentralWidget(m_raidsView);

    refreshArraysList();
}


MainWindow::~MainWindow()
{

}


void MainWindow::refreshArraysList()
{
    const int rows = m_raidsModel.rowCount();
    m_raidsModel.removeRows(0, rows);     // .clear() would clear headers also

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

void MainWindow::createRaid()
{
    CreateRaidDialog createRaidDialog(this);
    auto ret = createRaidDialog.exec();

    if (ret == QDialog::Accepted)
    {
        const QMap<QString, MDAdmController::Type> typeMap =
        {
            { "RAID0", MDAdmController::Type::Raid0 },
            { "RAID1", MDAdmController::Type::Raid1 },
            { "RAID4", MDAdmController::Type::Raid4 },
            { "RAID5", MDAdmController::Type::Raid5 },
            { "RAID6", MDAdmController::Type::Raid6 }
        };
        const auto disks = createRaidDialog.getSelectedDisks();
        const auto type = createRaidDialog.getType();
        const auto mdNumber = createRaidDialog.getMDNumber();

        Q_ASSERT(typeMap.contains(type));

        m_mdadmController.createRaid(QString("/dev/md%1").arg(mdNumber),
                                     typeMap.value(type),
                                     disks);
    }
}
