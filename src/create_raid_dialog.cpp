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

#include <QButtonGroup>
#include <QComboBox>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QSpinBox>

#include "create_raid_dialog.hpp"
#include "disk_controller.hpp"
#include "exclude_used_filter.hpp"

namespace
{
    struct MissingDevice {
        const char* path = "missing";
        const QString representation =
                QObject::tr("Missing Device Representation");
    };
}

CreateRaidDialog::CreateRaidDialog(IFileSystem* fs, QWidget* parent) :
    QDialog(parent),
    m_disksView(nullptr),
    m_selectedDisksView(nullptr),
    m_spareDisksView(nullptr),
    m_disksModel(),
    m_selectedDisksModel(),
    m_spareDisksModel(),
    m_raidTypesComboBox(nullptr),
    m_DevNumberSpinBox(nullptr),
    m_labelDiskCount(nullptr),
    m_raidTypes({{RAID0, {1,0}},
                 {RAID1, {2,127}},
                 {RAID4, {3,1}},
                 {RAID5, {3,1}},
                 {RAID6, {4,2}}})
{
    QVBoxLayout *systemDisksLayout = new QVBoxLayout;
    QVBoxLayout *selectedDisksLayout = new QVBoxLayout;
    QHBoxLayout *disksLayout = new QHBoxLayout;
    QFormLayout *optionsLayout = new QFormLayout;
    QHBoxLayout *buttonCreateLayout = new QHBoxLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *labelDisks = new QLabel(tr("available disks:"));
    QLabel *labelSelectedDisks = new QLabel(tr("disks for RAID:"));
    QLabel *labelSpareDisks = new QLabel(tr("spare disks:"));

    m_raidTypesComboBox = new QComboBox;
    m_raidTypesComboBox->addItem(tr("RAID0"), RAID0);
    m_raidTypesComboBox->addItem(tr("RAID1"), RAID1);
    m_raidTypesComboBox->addItem(tr("RAID4"), RAID4);
    m_raidTypesComboBox->addItem(tr("RAID5"), RAID5);
    m_raidTypesComboBox->addItem(tr("RAID6"), RAID6);
    m_raidTypesComboBox->setDisabled(true);

    m_DevNumberSpinBox = new QSpinBox;

    /* TODO: get ranges from other class */
    m_DevNumberSpinBox->setRange(0, 127);

    m_labelDiskCount = new QLabel();

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    QPushButton *buttonCreate = new QPushButton(tr("Create"));

    m_disksView = constructViewAndAttachModel(&m_disksModel);
    m_selectedDisksView = constructViewAndAttachModel(&m_selectedDisksModel);
    m_spareDisksView = constructViewAndAttachModel(&m_spareDisksModel);

    systemDisksLayout->addWidget(labelDisks);
    systemDisksLayout->addWidget(m_disksView);

    selectedDisksLayout->addWidget(labelSelectedDisks);
    selectedDisksLayout->addWidget(m_selectedDisksView);
    selectedDisksLayout->addWidget(labelSpareDisks);
    selectedDisksLayout->addWidget(m_spareDisksView);

    disksLayout->addLayout(systemDisksLayout);
    disksLayout->addLayout(createDiskManagementButtons());
    disksLayout->addLayout(selectedDisksLayout);

    optionsLayout->addRow(new QLabel(tr("Type:")), m_raidTypesComboBox);
    optionsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    optionsLayout->addRow(new QLabel(tr("MD device:")), m_DevNumberSpinBox);

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
        item->setData(DeviceType::Physical, DiskItemData::DeviceType);
        m_disksModel.appendRow(item);
    }

    m_disksModel.sort(0);

    connect(buttonCancel, &QPushButton::clicked, this,
            &CreateRaidDialog::reject);
    connect(buttonCreate, &QPushButton::clicked, this,
            &CreateRaidDialog::accept);

    updateCounters(0, 0);

    setLayout(mainLayout);
}

QListView*
CreateRaidDialog::constructViewAndAttachModel(QStandardItemModel *model)
{
    QListView* listView = new QListView;
    listView->setModel(model);

    listView->setSelectionMode(QAbstractItemView::MultiSelection);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    return listView;
}

QBoxLayout* CreateRaidDialog::createDiskManagementButtons()
{
    QVBoxLayout *buttonDiskLayout = new QVBoxLayout;

    QPushButton *buttonAdd = new QPushButton(tr("->"));
    QPushButton *buttonRemove = new QPushButton(tr("<-"));
    QPushButton *buttonAddMissing = new QPushButton(tr("+ MISSING"));
    QPushButton *buttonAddSpare = new QPushButton(tr("+ SPARE"));
    QPushButton *buttonRemoveSpare = new QPushButton(tr("- SPARE"));

    buttonAdd->setDisabled(true);
    buttonRemove->setDisabled(true);
    buttonAddSpare->setDisabled(true);
    buttonRemoveSpare->setDisabled(true);

    buttonDiskLayout->addStretch();
    buttonDiskLayout->addWidget(buttonAdd);
    buttonDiskLayout->addWidget(buttonRemove);
    buttonDiskLayout->addWidget(buttonAddMissing);
    buttonDiskLayout->addStretch();
    buttonDiskLayout->addWidget(buttonAddSpare);
    buttonDiskLayout->addWidget(buttonRemoveSpare);
    buttonDiskLayout->addStretch();

    connect(buttonAddMissing, &QPushButton::clicked,
            [this]()
    {
        MissingDevice missing;
        QStandardItem* item = new QStandardItem(missing.representation);
        item->setData(missing.path, DiskItemData::Path);
        item->setData(DeviceType::Virtual, DiskItemData::DeviceType);
        m_selectedDisksModel.appendRow(item);

        this->recalculateRaidType();
    });

    connect(buttonAdd, &QPushButton::clicked, this,
            &CreateRaidDialog::addElements);
    connect(buttonRemove, &QPushButton::clicked, this,
            &CreateRaidDialog::removeElements);
    connect(buttonAddSpare, &QPushButton::clicked, this,
            &CreateRaidDialog::addSpares);
    connect(buttonRemoveSpare, &QPushButton::clicked, this,
            &CreateRaidDialog::removeSpares);

    connect(m_raidTypesComboBox,
            static_cast<void(QComboBox::*)(const QString&)>
                (&QComboBox::currentIndexChanged),
            [buttonAddSpare](const QString& type)
    {
        buttonAddSpare->setDisabled(type == "RAID0");
    });

    connect(m_disksView->selectionModel(),
            static_cast<void(QItemSelectionModel::*)(const QItemSelection&,
                                                     const QItemSelection&)>
                (&QItemSelectionModel::selectionChanged),
            [this, buttonAdd, buttonAddSpare](const QItemSelection&,
                                              const QItemSelection&)
    {
        const auto selection = this->m_disksView->selectionModel()->selection();
        const auto add_disabled = selection.empty();
        const auto current_type =
                static_cast<RaidType>(
                    this->m_raidTypesComboBox->currentData().toInt());
        const auto spare_disabled = (selection.empty() ||
                                      current_type == RAID0);
        buttonAdd->setDisabled(add_disabled);
        buttonAddSpare->setDisabled(spare_disabled);
    });

    connect(m_selectedDisksView->selectionModel(),
            static_cast<void(QItemSelectionModel::*)(const QItemSelection&,
                                                     const QItemSelection&)>
            (&QItemSelectionModel::selectionChanged),
            [this, buttonRemove](const QItemSelection&,
                                 const QItemSelection&)
    {
        const auto selection =
                this->m_selectedDisksView->selectionModel()->selection();
        buttonRemove->setDisabled(selection.empty());

    });

    connect(m_spareDisksView->selectionModel(),
            static_cast<void(QItemSelectionModel::*)(const QItemSelection&,
                                                     const QItemSelection&)>
            (&QItemSelectionModel::selectionChanged),
            [this, buttonRemoveSpare](const QItemSelection&,
                                      const QItemSelection&)
    {
        const auto selection =
                this->m_spareDisksView->selectionModel()->selection();
        buttonRemoveSpare->setDisabled(selection.empty());

    });

    return buttonDiskLayout;
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

    emit selectionModel->selectionChanged(QItemSelection(), QItemSelection());

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
    recalculateRaidType();
}

void CreateRaidDialog::removeElements()
{
    move(m_selectedDisksView, m_selectedDisksModel, m_disksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        if (static_cast<enum DeviceType>(
                    item->data(DiskItemData::DeviceType).toInt()) == Physical)
            model.appendRow(item);
        else
            delete item;
    });
    recalculateRaidType();
}

void CreateRaidDialog::addSpares()
{
    move(m_disksView, m_disksModel, m_spareDisksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        model.appendRow(item);
    });
    recalculateRaidType();
}

void CreateRaidDialog::removeSpares()
{
    move(m_spareDisksView, m_spareDisksModel, m_disksModel,
         [](QStandardItemModel& model, QStandardItem* item)
    {
        model.appendRow(item);
    });
    recalculateRaidType();
}

void CreateRaidDialog::recalculateRaidType()
{
    const QStandardItemModel* model =
            qobject_cast<const QStandardItemModel*>(
                m_raidTypesComboBox->model());

    const auto total = static_cast<unsigned>(m_selectedDisksModel.rowCount());
    const auto spares = m_spareDisksModel.rowCount() > 0;
    unsigned missing = getMissingCount();

    const auto& disable = (total == 0 || missing == total ||
                           (spares && total == 1));

    m_raidTypesComboBox->setDisabled(disable);

    if (!disable)
    {
        int last_enabled = 0;
        bool current_disabled = false;
        const int current = m_raidTypesComboBox->currentIndex();

        for (int i=0; i< m_raidTypesComboBox->count(); ++i)
        {
            const RaidType type =
                    static_cast<RaidType>(
                        m_raidTypesComboBox->itemData(i).toInt());

            Q_ASSERT(m_raidTypes.contains(type));
            auto item = model->item(i);
            if (total < m_raidTypes.value(type).m_total ||
                    missing > m_raidTypes.value(type).m_missing ||
                    (type == RAID0 && spares))
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
            m_raidTypesComboBox->setCurrentIndex(last_enabled);
    }

    updateCounters(total, missing);
}

QStringList
CreateRaidDialog::getDisksFromModel(const QStandardItemModel& model) const
{
    QStringList disks;

    for (int row = 0; row < model.rowCount(); ++row)
    {
        QStandardItem *item = model.item(row, 0);
        if (item != nullptr)
            disks.append(item->data(DiskItemData::Path).toString());
    }

    return disks;
}

QStringList CreateRaidDialog::getSelectedDisks() const
{
    return getDisksFromModel(m_selectedDisksModel);
}

QStringList CreateRaidDialog::getSelectedSpares() const {
    return getDisksFromModel(m_spareDisksModel);
}

CreateRaidDialog::RaidType CreateRaidDialog::getType() const
{
    return static_cast<RaidType>(m_raidTypesComboBox->currentData().toInt());
}

unsigned CreateRaidDialog::getMDNumber() const
{
    return static_cast<unsigned>(m_DevNumberSpinBox->value());
}

unsigned CreateRaidDialog::getMissingCount() const
{
    unsigned missing = 0;
    const auto total = m_selectedDisksModel.rowCount();
    for (int i = 0; i < total; ++i) {
        auto item = m_selectedDisksModel.item(i, 0);
        if (static_cast<enum DeviceType>(
                    item->data(DiskItemData::DeviceType).toInt()) == Virtual)
        {
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
