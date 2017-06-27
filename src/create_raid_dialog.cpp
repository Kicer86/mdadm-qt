#include "create_raid_dialog.hpp"
#include "disk_controller.hpp"

#include <QtDebug>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QPushButton>
#include <QListView>

CreateRaidDialog::CreateRaidDialog(QWidget *parent) :
    QDialog(parent),
    m_disksView(nullptr),
    m_selectedDisksView(nullptr),
    m_disksModel(),
    m_selectedDisksModel()
{
    QHBoxLayout *disksLayout = new QHBoxLayout;
    QVBoxLayout *buttonLayout = new QVBoxLayout;

    QPushButton *buttonAdd = new QPushButton(tr("->"));
    QPushButton *buttonRemove = new QPushButton(tr("<-"));

    m_disksView = new QListView;
    m_selectedDisksView = new QListView;

    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonAdd);
    buttonLayout->addWidget(buttonRemove);
    buttonLayout->addStretch();

    disksLayout->addWidget(m_disksView);
    disksLayout->addLayout(buttonLayout);
    disksLayout->addWidget(m_selectedDisksView);

    DiskController dc;
    auto disks = dc.listDisks();

    for (const auto& disk : disks) {
        QStandardItem* item = new QStandardItem(disk.toString());
        item->data(0) = disk.name();
        m_disksModel.appendRow(item);
    }

    m_disksModel.sort(0);

    connect(buttonAdd, &QPushButton::clicked, this, &CreateRaidDialog::addElements);
    connect(buttonRemove, &QPushButton::clicked, this, &CreateRaidDialog::removeElements);

    m_disksView->setModel(&m_disksModel);
    m_selectedDisksView->setModel(&m_selectedDisksModel);

    m_disksView->setSelectionMode(QAbstractItemView::MultiSelection);
    m_selectedDisksView->setSelectionMode(QAbstractItemView::MultiSelection);

    setLayout(disksLayout);
}

void CreateRaidDialog::addElements() {
    QItemSelectionModel *selectionModel = m_disksView->selectionModel();
    for (const auto& elem : selectionModel->selectedIndexes()) {
        QStandardItem* item = m_disksModel.takeItem(elem.row(), elem.column());
        m_selectedDisksModel.appendRow(item);
    }
    m_disksModel.sort(0);
    m_selectedDisksModel.sort(0);
}

void CreateRaidDialog::removeElements() {
    QItemSelectionModel *selectionModel =
            m_selectedDisksView->selectionModel();
    for (const auto& elem : selectionModel->selectedIndexes()) {
        QStandardItem* item = m_selectedDisksModel
                .takeItem(elem.row(), elem.column());
        m_disksModel.appendRow(item);
    }
    m_selectedDisksModel.sort(0);
    m_disksModel.sort(0);
}
