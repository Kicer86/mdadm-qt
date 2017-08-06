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

#include <QInputDialog>
#include <QTabWidget>
#include <QTableView>
#include <QMenuBar>

#include "create_raid_dialog.hpp"
#include "disk_controller.hpp"
#include "empty_filter.hpp"


MainWindow::MainWindow():
    QMainWindow(),
    m_fileSystem(),
    m_mdadmProcess(),
    m_mdadmController(&m_mdadmProcess, &m_fileSystem),
    m_raidsModel(),
    m_disksModel(),
    m_viewTabs(nullptr),
    m_raidsView(nullptr),
    m_disksView(nullptr)
{
    // raids tab
    m_raidsView = new QTableView(this);
    m_raidsView->setModel(m_raidsModel.model());
    m_raidsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_raidsView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_raidsView->setContextMenuPolicy(Qt::CustomContextMenu);

    // disks tab
    m_disksModel.setHorizontalHeaderLabels( { tr("device"), tr("type"), tr("status") } );

    m_disksView = new QTableView(this);
    m_disksView->setModel(&m_disksModel);
    m_disksView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_disksView->setSelectionMode(QAbstractItemView::SingleSelection);

    // menus & shortcuts
    auto raidMenu = menuBar()->addMenu(tr("&Raid"));
    QAction *actionCreate = new QAction(tr("&New"), this);
    QAction *actionQuit = new QAction(tr("&Quit"), this);

    actionCreate->setShortcut(Qt::CTRL + Qt::Key_N);
    actionQuit->setShortcut(Qt::CTRL + Qt::Key_Q);

    connect(actionCreate, &QAction::triggered, this, &MainWindow::createRaid);
    connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);

    raidMenu->addAction(actionCreate);
    raidMenu->addAction(actionQuit);

    auto viewMenu = menuBar()->addMenu(tr("&View"));
    QAction *actionReload = new QAction(tr("&Reload"), this);

    actionReload->setShortcut(Qt::Key_F5);

    connect(actionReload, &QAction::triggered, this, &MainWindow::refreshArraysList);

    viewMenu->addAction(actionReload);

    // setup tabs
    m_viewTabs = new QTabWidget(this);
    m_viewTabs->addTab(m_raidsView, tr("RAIDs"));
    m_viewTabs->addTab(m_disksView, tr("Disks"));

    setCentralWidget(m_viewTabs);

    // refresh stuf
    refreshArraysList();
    refreshDisksList();

    m_raidsView->sortByColumn(0, Qt::AscendingOrder);
    m_raidsView->setSortingEnabled(true);

    m_disksView->sortByColumn(0, Qt::AscendingOrder);
    m_disksView->setSortingEnabled(true);

    connect(&m_mdadmController, &MDAdmController::raidCreated, this,
            &MainWindow::refreshArraysList);
    connect(m_raidsView, &QTableView::customContextMenuRequested, this,
            &MainWindow::contextMenu);
}


MainWindow::~MainWindow()
{

}


void MainWindow::contextMenu(const QPoint& pos)
{
    const QModelIndex index = m_raidsView->indexAt(pos);

    if (!index.isValid())
        return;

    const RaidInfo& raid = m_raidsModel.infoForRow(index.row());
    const QString& device = raid.raid_device;

    QMenu *raidOptions = new QMenu(this);
    QAction *actionRemove = new QAction("Remove " + device, this);

    connect(actionRemove, qOverload<bool>(&QAction::triggered),
            [device, this](bool)
    {
        if (!device.isNull())
            this->removeRaid(device);
    });

    raidOptions->addAction(actionRemove);
    raidOptions->popup(m_raidsView->viewport()->mapToGlobal(pos));

}

bool MainWindow::removeRaid(const QString& raidDevice)
{
    /* FIXME temporary solution */
    const QString CONFIRM_TEXT = tr("confirm");
    bool ret;

    const QString text = QInputDialog::getText(this,
                       tr("Remove software RAID"),
                       tr("<b>Warning!</b><br /><br />"
                          "This operation will remove <b>%1</b> RAID device"
                          " from the system and clear metadata on all of its "
                          "components.<br /><br />Please enter <b>%2</b> word"
                          " to confirm this operation")
                                         .arg(raidDevice)
                                         .arg(CONFIRM_TEXT),
                       QLineEdit::Normal,
                       "",
                       &ret);
     if (ret && text == CONFIRM_TEXT)
     {
        ret = m_mdadmController.removeRaid("/dev/" + raidDevice);
     }
     return ret;
}

void MainWindow::refreshArraysList()
{
    auto load = std::bind(&RaidsModel::load, &m_raidsModel, std::placeholders::_1);

    m_mdadmController.listRaids(load);
}


void MainWindow::refreshDisksList()
{
    const int rows = m_disksModel.rowCount();
    m_disksModel.removeRows(0, rows);     // .clear() would clear headers also

    const DiskController dc(&m_fileSystem);
    const auto disks = dc.listDisks(EmptyFilter());

    for(const std::unique_ptr<IBlockDevice>& blk_dev: disks)
    {
        QStandardItem* device_item = new QStandardItem(blk_dev->devPath());
        QStandardItem* type_item = new QStandardItem("disk");
        QStandardItem* status_item = new QStandardItem(blk_dev->isUsed()? tr("in use"): tr("ok"));  // TODO: 'ok' is not nice (?)

        const QList<QStandardItem *> row = { device_item, type_item, status_item };
        m_disksModel.appendRow(row);
    }
}


void MainWindow::createRaid()
{
    CreateRaidDialog createRaidDialog(&m_fileSystem, this);
    const auto ret = createRaidDialog.exec();

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
