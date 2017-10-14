/*
 * Structure with information about raid.
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
 */

#ifndef RAIDINFO_HPP
#define RAIDINFO_HPP

#include <QString>
#include <QList>

#include "objects_ids.hpp"


struct RaidComponentInfo
{
    enum class Type {
        Normal,
        WriteMostly = 'W',
        Journal = 'J',
        Faulty = 'F',
        Spare = 'S',
        Replacement ='R',
    };

    QString name;
    Type type;
    int descriptor_index;

    RaidComponentInfo(const QString& _name, Type _type, int _descr_nr) :
        name(_name),
        type(_type),
        descriptor_index(_descr_nr)
    {
    }

    RaidComponentInfo(const RaidComponentInfo &) = default;
    RaidComponentInfo(RaidComponentInfo &&) = default;
    RaidComponentInfo& operator=(const RaidComponentInfo &) = default;
    RaidComponentInfo& operator=(RaidComponentInfo &&) = default;

    bool operator==(const RaidComponentInfo &) const;
    bool operator<(const RaidComponentInfo &) const;
};


struct IRaidInfoDataProvider
{
    virtual ~IRaidInfoDataProvider() = default;

    /*
     * device types in mdstat:
     * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/md/md.c#n7711
     */

    virtual const QString& raidDevice(const RaidId &) const = 0;
    virtual const QList<RaidComponentInfo>& blockDevices(const RaidId &) const = 0;
    virtual const QString& raidType(const RaidId &) const = 0;
};


struct RaidInfo
{
    RaidInfo(const IRaidInfoDataProvider *, RaidId);

    RaidInfo(const RaidInfo &) = default;
    RaidInfo(RaidInfo &&) = default;

    RaidInfo& operator=(const RaidInfo &) = default;
    RaidInfo& operator=(RaidInfo &&) = default;

    bool operator==(const RaidInfo&) const;
    bool operator!=(const RaidInfo&) const;
    bool operator<(const RaidInfo&) const;

    const QString& device() const;
    const QString& type() const;
    const QList<RaidComponentInfo>& devices() const;

    private:
        RaidId m_id;
        const IRaidInfoDataProvider* m_provider;
};

#endif // RAIDINFO_HPP
