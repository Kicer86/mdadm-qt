/*
 * Dialog for RAID creation.
 * Copyright (C) 2017  Arkadiusz Bubała <arek2407066@gmail.com>
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

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QComboBox>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QSpinBox>

#include "create_raid_dialog.hpp"
#include "disk_controller.hpp"
#include "exclude_used_filter.hpp"
#include "missing.hpp"

CreateRaidDialog::CreateRaidDialog(IFileSystem* fs, QWidget* parent) :
    QDialog(parent),
    m_disksView(nullptr),
    m_selectedDisksView(nullptr),
    m_spareDisksView(nullptr),
    m_disksModel(),
    m_selectedDisksModel(),
    m_spareDisksModel(),
    m_cbTypes(nullptr),
    m_sbDevNumber(nullptr),
    m_labelDiskCount(nullptr),
    m_raidTypes({{"RAID0", {1,0}},
                 {"RAID1", {2,127}},
                 {"RAID4", {3,1}},
                 {"RAID5", {3,1}},
                 {"RAID6", {4,2}}})
{
    QVBoxLayout *systemDisksLayout = new QVBoxLayout;
    QVBoxLayout *buttonDiskLayout = new QVBoxLayout;
    QVBoxLayout *selectedDisksLayout = new QVBoxLayout;
    QHBoxLayout *disksLayout = new QHBoxLayout;
    QFormLayout *optionsLayout = new QFormLayout;
    QHBoxLayout *buttonCreateLayout = new QHBoxLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *labelDisks = new QLabel(tr("available disks:"));
    QLabel *labelSelectedDisks = new QLabel(tr("disks for RAID:"));
    QLabel *labelSpareDisks = new QLabel(tr("spare disks:"));

    QPushButton *buttonAdd = new QPushButton(tr("->"));
    QPushButton *buttonRemove = new QPushButton(tr("<-"));
    QPushButton *buttonAddMissing = new QPushButton(tr("+ MISSING"));
    QPushButton *buttonAddSpare = new QPushButton(tr("+ SPARE"));
    QPushButton *buttonRemoveSpare = new QPushButton(tr("- SPARE"));


    m_cbTypes = new QComboBox;
    m_cbTypes->addItems(m_raidTypes.keys());
    m_cbTypes->setDisabled(true);

    m_sbDevNumber = new QSpinBox;

    /* TODO: get ranges from other class */
    m_sbDevNumber->setRange(0, 127);

    m_labelDiskCount = new QLabel();

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    QPushButton *buttonCreate = new QPushButton(tr("Create"));

    m_disksView = new QListView;
    m_selectedDisksView = new QListView;
    m_spareDisksView = new QListView;

    systemDisksLayout->addWidget(labelDisks);
    systemDisksLayout->addWidget(m_disksView);

    buttonDiskLayout->addStretch();
    buttonDiskLayout->addWidget(buttonAdd);
    buttonDiskLayout->addWidget(buttonRemove);
    buttonDiskLayout->addWidget(buttonAddMissing);
    buttonDiskLayout->addStretch();
    buttonDiskLayout->addWidget(buttonAddSpare);
    buttonDiskLayout->addWidget(buttonRemoveSpare);
    buttonDiskLayout->addStretch();

    selectedDisksLayout->addWidget(labelSelectedDisks);
    selectedDisksLayout->addWidget(m_selectedDisksView);
    selectedDisksLayout->addWidget(m_spareDisksView);

    disksLayout->addLayout(systemDisksLayout);
    disksLayout->addLayout(buttonDiskLayout);
    disksLayout->addLayout(selectedDisksLayout);

    optionsLayout->addRow(new QLabel(tr("Type:")), m_cbTypes);
    optionsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    optionsLayout->addRow(new QLabel(tr("MD device:")), m_sbDevNumber);

    buttonCreateLayout->addStretch();
    buttonCreateLayout->addWidget(buttonCancel);
    buttonCreateLayout->addWidget(buttonCreate);

    mainLayout->addLayout(disksLayout);
    mainLayout->addLayout(optionsLayout);
    mainLayout->addWidget(m_labelDiskCount);
    mainLayout->addLayout(buttonCreateLayout);

    DiskController dc(fs);
    ExcludeUsedFilter diskFilter;

    auto disks = dc.listDisks(diskFilter);

    for (const auto& disk : disks)
    {
        QStandardItem* item = new QStandardItem(disk->toString());
        item->setData(disk->devPath(), DiskItemData::Path);
        item->setData(disk->isPhysical(), DiskItemData::IsPhysical);
        m_disksModel.appendRow(item);
    }

    m_disksModel.sort(0);

    connect(buttonAddMissing, &QPushButton::clicked,
            [this, buttonAddMissing, &dc]()
    {
        std::unique_ptr<Missing> missing(new Missing());
        QStandardItem* item = new QStandardItem(missing->toString());
        item->setData(missing->devPath(), DiskItemData::Path);
        item->setData(missing->isPhysical(), DiskItemData::IsPhysical);
        m_selectedDisksModel.appendRow(item);

        this->recalculateType();
    });

    connect(buttonAdd, &QPushButton::clicked, this,
            &CreateRaidDialog::addElements);
    connect(buttonRemove, &QPushButton::clicked, this,
            &CreateRaidDialog::removeElements);
    connect(buttonAddSpare, &QPushButton::clicked, this,
            &CreateRaidDialog::addSpares);
    connect(buttonRemoveSpare, &QPushButton::clicked, this,
            &CreateRaidDialog::removeSpares);

    connect(buttonCancel, &QPushButton::clicked, this,
            &CreateRaidDialog::reject);
    connect(buttonCreate, &QPushButton::clicked, this,
            &CreateRaidDialog::accept);

    m_disksView->setModel(&m_disksModel);
    m_selectedDisksView->setModel(&m_selectedDisksModel);
    m_spareDisksView->setModel(&m_spareDisksModel);

    m_disksView->setSelectionMode(QAbstractItemView::MultiSelection);
    m_selectedDisksView->setSelectionMode(QAbstractItemView::MultiSelection);
    m_spareDisksView->setSelectionMode(QAbstractItemView::MultiSelection);

    updateCounters(0, 0);

    setLayout(mainLayout);
}

void CreateRaidDialog::move(const QListView* sourceView,
                            QStandardItemModel& sourceModel,
                            QStandardItemModel& destinationModel,
                            void (*insert)(QStandardItemModel &,
                                              QStandardItem*))
{
    QItemSelectionModel *selectionModel = sourceView->selectionModel();
    std::vector<QPersistentModelIndex> rows_to_delete;

    for (const auto& elem : selectionModel->selectedIndexes())
    {
        if (!elem.isValid())
            continue;
        QStandardItem* item = sourceModel.takeItem(elem.row(), elem.column());

        insert(destinationModel, item);
        rows_to_delete.emplace_back(elem);
    }

    for (const auto& elem: rows_to_delete)
    {
        sourceModel.removeRow(elem.row());
    }

    sourceModel.sort(0);
    destinationModel.sort(0);
}

void CreateRaidDialog::addElements()
{
    move(m_disksView, m_disksModel, m_selectedDisksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        model.appendRow(item);
    });
    recalculateType();
}

void CreateRaidDialog::removeElements()
{
    move(m_selectedDisksView, m_selectedDisksModel, m_disksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        if (item->data(DiskItemData::IsPhysical).toBool())
            model.appendRow(item);
        else
            delete item;
    });
    recalculateType();
}

void CreateRaidDialog::addSpares()
{
    move(m_disksView, m_disksModel, m_spareDisksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        model.appendRow(item);
    });
}

void CreateRaidDialog::removeSpares()
{
    move(m_spareDisksView, m_spareDisksModel, m_disksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        model.appendRow(item);
    });
}

void CreateRaidDialog::recalculateType()
{
    const QStandardItemModel* model =
            qobject_cast<const QStandardItemModel*>(m_cbTypes->model());

    const auto& total = static_cast<unsigned>(m_selectedDisksModel.rowCount());
    unsigned missing = getMissingCount();

    m_cbTypes->setDisabled(total == 0 || missing == total);

    int last_enabled = 0;
    bool current_disabled = false;
    const int current = m_cbTypes->currentIndex();

    for (int i=0; i< m_cbTypes->count(); ++i)
    {
        const QString &type = m_cbTypes->itemText(i);

        Q_ASSERT(m_raidTypes.contains(type));
        auto item = model->item(i);
        if (total < m_raidTypes.value(type).m_total ||
                missing > m_raidTypes.value(type).m_missing)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (i == current)
                current_disabled = true;
        }
        else
        {
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
            last_enabled = i;
        }
    }

    if (current_disabled)
        m_cbTypes->setCurrentIndex(last_enabled);

    updateCounters(total, missing);
}

QStringList CreateRaidDialog::getSelectedDisks() const
{
    QStringList disks;

    for (int row = 0; row < m_selectedDisksModel.rowCount(); ++row)
    {
        QStandardItem *item = m_selectedDisksModel.item(row, 0);
        if (item != nullptr)
            disks.append(item->data(DiskItemData::Path).toString());
    }

    return disks;
}

QString CreateRaidDialog::getType() const
{
    return m_cbTypes->currentText();
}

unsigned CreateRaidDialog::getMDNumber() const
{
    return static_cast<unsigned>(m_sbDevNumber->value());
}

unsigned CreateRaidDialog::getMissingCount() const
{
    unsigned missing = 0;
    const auto& total = m_selectedDisksModel.rowCount();
    for (int i = 0; i < total; ++i) {
        auto item = m_selectedDisksModel.item(i, 0);
        if (!item->data(DiskItemData::IsPhysical).toBool()) {
            ++missing;
        }
    }

    return missing;
}

void CreateRaidDialog::updateCounters(unsigned total, unsigned missing)
{
    m_labelDiskCount->setText(tr("Total / missing devices: %1/%2")
                              .arg(total).arg(missing));
}
