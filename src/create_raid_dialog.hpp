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
public:
    enum RaidType { RAID0, RAID1, RAID4, RAID5, RAID6 };

    CreateRaidDialog(IFileSystem *, QWidget *parent = Q_NULLPTR);
    CreateRaidDialog(const CreateRaidDialog &) = delete;
    CreateRaidDialog& operator=(const CreateRaidDialog &) = delete;

    QStringList getSelectedDisks() const;
    QStringList getSelectedSpares() const;
    RaidType getType() const;
    unsigned getMDNumber() const;
    unsigned getMissingCount() const;
    void updateCounters(unsigned, unsigned);

private:
    enum DiskItemData { Path = Qt::UserRole,
                        IsPhysical };

    QListView* m_disksView;
    QListView* m_selectedDisksView;
    QListView* m_spareDisksView;
    QStandardItemModel m_disksModel;
    QStandardItemModel m_selectedDisksModel;
    QStandardItemModel m_spareDisksModel;

    QComboBox *m_raidTypesComboBox;
    QSpinBox *m_DevNumberSpinBox;
    QLabel *m_labelDiskCount;

    const QMap<RaidType, RaidLimits> m_raidTypes;

    void move(const QListView*, QStandardItemModel&, QStandardItemModel&,
              void (*)(QStandardItemModel &, QStandardItem*));

    /* array components management */
    void addElements();
    void removeElements();

    /* spare management */
    void addSpares();
    void removeSpares();

    QStringList getDisksFromModel(const QStandardItemModel&) const;

    void recalculateRaidType();
};

#endif // CREATE_RAID_DIALOG_HPP
