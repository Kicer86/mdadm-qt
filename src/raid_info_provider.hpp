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

#include "iraid_info_provider.hpp"

struct IFileSystem;

class RaidInfoProvider: public IRaidInfoProvider
{
    public:
        RaidInfoProvider(IFileSystem *);
        virtual ~RaidInfoProvider();

        // ListResult - callback function for listRaids
        typedef std::function<void(const std::vector<RaidInfo> &)> ListResult;

        // overrides
        std::vector<RaidId> listRaids() const override;
        RaidInfo getInfoFor(const IRaidInfoProvider::RaidId & ) const override;

        // operations
        bool listRaids(const ListResult &) const;       // list raids asynchronicaly, call ListResult when done
        bool listComponents(const QString& raid_device,
                            QStringList& block_devices);

    private:
        mutable std::map<RaidId, RaidInfo> m_infoCache;
        IFileSystem* m_fileSystem;

};

#endif // RAIDINFOPROVIDER_HPP
