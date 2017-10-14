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


#include <set>

#include <QStandardItemModel>

#include "mdadm_controller.hpp"


struct IRaidInfoProvider;


class RaidsModel: public QObject
{
    public:
        enum ItemType
        {
            Raid,
            Component,
        };

        RaidsModel(IRaidInfoProvider *);
        RaidsModel(const RaidsModel &) = delete;
        ~RaidsModel();

        RaidsModel& operator=(const RaidsModel &) = delete;

        ItemType getTypeFor(const QModelIndex &) const;
        RaidInfo infoForRaid(const QModelIndex &) const;
        const RaidComponentInfo& infoForComponent(const QModelIndex &) const;
        QAbstractItemModel* model();

    private:
        typedef std::map<QStandardItem *, RaidId> RaidsMap;
        typedef std::map<QStandardItem *, RaidComponentInfo> ComponentsMap;

        QStandardItemModel m_model;
        std::map<QStandardItem *, RaidId> m_infos;
        std::map<QStandardItem *, RaidComponentInfo> m_componentInfos;
        const QMap<RaidComponentInfo::Type, QString> m_diskType;
        IRaidInfoProvider* m_raidInfoProvider;

        RaidsMap::iterator itFor(const RaidId &);
        RaidsMap::const_iterator itFor(const RaidId &) const;

        QStandardItem* itemFor(const RaidId &) const;

        void appendRaid(const RaidId &);
        void removeRaid(const RaidId &);
        void updateRaid(const RaidId &);

        void removeComponentsOf(const RaidId &);

        void appendComponent(QStandardItem *, const RaidComponentInfo &);
};

#endif // RAIDSMODEL_HPP
