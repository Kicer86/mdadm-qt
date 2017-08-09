#ifndef CREATE_RAID_DIALOG_HPP
#define CREATE_RAID_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QMap>

class QLabel;
class QListView;
class QComboBox;
class QSpinBox;

struct IFileSystem;

struct RaidLimits {
    unsigned m_total;
    unsigned m_missing;
};

class CreateRaidDialog : public QDialog
{
    enum DiskItemData { PATH = Qt::UserRole,
                        IS_PHYSICAL };

    QListView* m_disksView;
    QListView* m_selectedDisksView;
    QStandardItemModel m_disksModel;
    QStandardItemModel m_selectedDisksModel;

    QComboBox *m_cbTypes;
    QSpinBox *m_sbDevNumber;
    QLabel *m_labelDiskCount;

    const QMap<QString, RaidLimits> m_raidTypes;

    void addElements();
    void removeElements();

    void recalculateType();

public:
    CreateRaidDialog(IFileSystem *, QWidget *parent = Q_NULLPTR);
    CreateRaidDialog(const CreateRaidDialog &) = delete;
    CreateRaidDialog& operator=(const CreateRaidDialog &) = delete;

    QStringList getSelectedDisks() const;
    QString getType() const;
    unsigned getMDNumber() const;
    unsigned getMissingCount() const;
    void updateCounters(unsigned, unsigned);
};

#endif // CREATE_RAID_DIALOG_HPP
