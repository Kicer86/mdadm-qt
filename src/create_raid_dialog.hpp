#ifndef CREATE_RAID_DIALOG_HPP
#define CREATE_RAID_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QStringList>

class QListView;
class QComboBox;
class QSpinBox;

class CreateRaidDialog : public QDialog
{  
    QListView* m_disksView;
    QListView* m_selectedDisksView;
    QStandardItemModel m_disksModel;
    QStandardItemModel m_selectedDisksModel;

    QComboBox *m_cbTypes;
    QSpinBox *m_sbDevNumber;

    const QStringList m_raidTypes;

    void addElements();
    void removeElements();
    void createRaid();

public:
    CreateRaidDialog(QWidget *parent = Q_NULLPTR);
};

#endif // CREATE_RAID_DIALOG_HPP
