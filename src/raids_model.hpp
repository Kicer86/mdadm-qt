/*
 * Model representing list of raid devices
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

#ifndef RAIDSMODEL_HPP
#define RAIDSMODEL_HPP


#include <QStandardItemModel>

#include "mdadm_controller.hpp"

struct RaidData
{
    const RaidInfo& raid_info;
    const QString selected_component;
};

class RaidsModel: public QObject
{
    public:
        enum ItemType
        {
            Raid,
            RaidComponent,
        };
        
        RaidsModel();
        RaidsModel(const RaidsModel &) = delete;
        ~RaidsModel();

        RaidsModel& operator=(const RaidsModel &) = delete;

        void load(const std::vector<RaidInfo> &);

        ItemType getTypeFor(const QModelIndex &) const;
        RaidData infoForIndex(const QModelIndex&) const;
        QAbstractItemModel* model();

    private:
        QStandardItemModel m_model;
        std::map<QStandardItem *, RaidInfo> m_infos;
        const QMap<RaidComponent::Type, QString> m_diskType;

        const RaidInfo& infoFor(const QModelIndex &) const;
};

#endif // RAIDSMODEL_HPP
