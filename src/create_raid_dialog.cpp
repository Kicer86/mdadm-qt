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

CreateRaidDialog::CreateRaidDialog(IFileSystem* fs, QWidget* parent) :
    QDialog(parent),
    m_disksView(nullptr),
    m_selectedDisksView(nullptr),
    m_disksModel(),
    m_selectedDisksModel(),
    m_cbTypes(nullptr),
    m_sbDevNumber(nullptr),
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

    QPushButton *buttonAdd = new QPushButton(tr("->"));
    QPushButton *buttonRemove = new QPushButton(tr("<-"));

    m_cbTypes = new QComboBox;
    m_cbTypes->addItems(m_raidTypes.keys());
    m_cbTypes->setDisabled(true);

    m_sbDevNumber = new QSpinBox;

    /* TODO: get ranges from other class */
    m_sbDevNumber->setRange(0, 127);

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    QPushButton *buttonCreate = new QPushButton(tr("Create"));

    m_disksView = new QListView;
    m_selectedDisksView = new QListView;

    systemDisksLayout->addWidget(labelDisks);
    systemDisksLayout->addWidget(m_disksView);

    buttonDiskLayout->addStretch();
    buttonDiskLayout->addWidget(buttonAdd);
    buttonDiskLayout->addWidget(buttonRemove);
    buttonDiskLayout->addStretch();

    selectedDisksLayout->addWidget(labelSelectedDisks);
    selectedDisksLayout->addWidget(m_selectedDisksView);

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
    mainLayout->addLayout(buttonCreateLayout);

    DiskController dc(fs);
    ExcludeUsedFilter diskFilter;

    auto disks = dc.listDisks(diskFilter);

    for (const auto& disk : disks)
    {
        QStandardItem* item = new QStandardItem(disk->toString());
        item->setData(disk->devPath());
        m_disksModel.appendRow(item);
    }

    for (int i=0; i<3; ++i)
    {
        QStandardItem* item = new QStandardItem(tr("missing - %1").arg(i+1));
        item->setData(QString("missing"));
        m_disksModel.appendRow(item);
    }

    m_disksModel.sort(0);

    connect(buttonAdd, &QPushButton::clicked, this,
            &CreateRaidDialog::addElements);
    connect(buttonRemove, &QPushButton::clicked, this,
            &CreateRaidDialog::removeElements);

    connect(buttonCancel, &QPushButton::clicked, this,
            &CreateRaidDialog::reject);
    connect(buttonCreate, &QPushButton::clicked, this,
            &CreateRaidDialog::accept);

    m_disksView->setModel(&m_disksModel);
    m_selectedDisksView->setModel(&m_selectedDisksModel);

    m_disksView->setSelectionMode(QAbstractItemView::MultiSelection);
    m_selectedDisksView->setSelectionMode(QAbstractItemView::MultiSelection);

    setLayout(mainLayout);
}

void CreateRaidDialog::addElements()
{
    QItemSelectionModel *selectionModel = m_disksView->selectionModel();
    std::vector<QPersistentModelIndex> rows_to_delete;
    int missing = 0;

    for (const auto& elem : selectionModel->selectedIndexes())
    {
        if (!elem.isValid())
            continue;
        QStandardItem* item = m_disksModel.takeItem(elem.row(), elem.column());
        m_selectedDisksModel.appendRow(item);
        rows_to_delete.emplace_back(elem);
    }

    for (const auto& elem: rows_to_delete)
    {
        m_disksModel.removeRow(elem.row());
    }

    for (int i = 0; i < m_selectedDisksModel.rowCount(); ++i)
        if (m_selectedDisksModel.item(i,0)->data() == "missing")
            ++missing;

    m_disksModel.sort(0);
    m_selectedDisksModel.sort(0);
    recalculateType(m_selectedDisksModel.rowCount(), missing);
}

void CreateRaidDialog::removeElements()
{
    QItemSelectionModel *selectionModel =
            m_selectedDisksView->selectionModel();
    std::vector<QPersistentModelIndex> rows_to_delete;
    int missing = 0;

    for (const auto& elem : selectionModel->selectedIndexes())
    {
        if (!elem.isValid())
            continue;

        QStandardItem* item = m_selectedDisksModel
                .takeItem(elem.row(), elem.column());
        m_disksModel.appendRow(item);
        rows_to_delete.emplace_back(elem);
    }

    for (const auto& elem : rows_to_delete)
    {
        m_selectedDisksModel.removeRow(elem.row());
    }

    for (int i = 0; i < m_selectedDisksModel.rowCount(); ++i)
        if (m_selectedDisksModel.item(i,0)->data() == "missing")
            ++missing;

    m_selectedDisksModel.sort(0);
    m_disksModel.sort(0);
    recalculateType(m_selectedDisksModel.rowCount(), missing);
}

void CreateRaidDialog::recalculateType(int total, int missing)
{
    const QStandardItemModel* model =
            qobject_cast<const QStandardItemModel*>(m_cbTypes->model());

    m_cbTypes->setDisabled(total == 0 || missing == total);

    for (int i=0; i< m_cbTypes->count(); ++i)
    {
        const QString &type = m_cbTypes->itemText(i);

        Q_ASSERT(m_raidTypes.contains(type));
        auto item = model->item(i);
        if (total < m_raidTypes.value(type).m_total ||
                missing > m_raidTypes.value(type).m_missing)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
        else
        {
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
        }
    }
}

QStringList CreateRaidDialog::getSelectedDisks() const
{
    QStringList disks;

    for (int row = 0; row < m_selectedDisksModel.rowCount(); ++row)
    {
        QStandardItem *item = m_selectedDisksModel.item(row, 0);
        if (item != nullptr)
            disks.append(item->data().toString());
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
