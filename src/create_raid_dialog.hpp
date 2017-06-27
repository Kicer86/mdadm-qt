#ifndef CREATE_RAID_DIALOG_HPP
#define CREATE_RAID_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>

class QListView;

class CreateRaidDialog : public QDialog
{
    QListView* m_disksView;
    QListView* m_selectedDisksView;
    QStandardItemModel m_disksModel;
    QStandardItemModel m_selectedDisksModel;

    void addElements();
    void removeElements();

public:
    CreateRaidDialog(QWidget *parent = Q_NULLPTR);
};

#endif // CREATE_RAID_DIALOG_HPP
