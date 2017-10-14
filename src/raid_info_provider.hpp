/*
 * Raid information hub
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

#ifndef RAIDINFOPROVIDER_HPP
#define RAIDINFOPROVIDER_HPP

#include <functional>
#include <map>

#include "iraid_info_provider.hpp"

struct IFileSystem;

class RaidInfoProvider: public IRaidInfoProvider, public IRaidInfoDataProvider
{
    public:
        RaidInfoProvider(IFileSystem *);
        RaidInfoProvider(const RaidInfoProvider &);
        RaidInfoProvider(RaidInfoProvider &&);
        virtual ~RaidInfoProvider();

        RaidInfoProvider& operator=(const RaidInfoProvider &);
        RaidInfoProvider& operator=(RaidInfoProvider &&);

        // overrides
        // IRaidInfoProvider
        std::vector<RaidInfo> listRaids() const override;
        RaidInfo getInfoFor(const RaidId & ) const override;

        // IRaidInfoDataProvider
        const QString& raidDevice(const RaidId &) const override;
        const QList<RaidComponentInfo>& blockDevices(const RaidId &) const override;
        const QString& raidType(const RaidId &) const override;

        bool listComponents(const QString& raid_device,
                            QStringList& block_devices) const override;

    private:
        struct RaidData
        {
            QString type;
            QString device;
            QList<RaidComponentInfo> components;
        };

        mutable std::map<RaidId, RaidData> m_raids;
        IFileSystem* m_fileSystem;

        void reCache() const;
        std::map<RaidId, RaidData> readRaids() const;
};

#endif // RAIDINFOPROVIDER_HPP
