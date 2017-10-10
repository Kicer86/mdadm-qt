
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <vector>

#include "raidinfo.hpp"

struct IRaidInfoProvider
{
    typedef QString RaidId;

    virtual ~IRaidInfoProvider() = default;

    virtual std::vector<RaidId> listRaids() const = 0;
    virtual RaidInfo getInfoFor(const RaidId &) const = 0;

    // TODO: refactor, base on RaidId
    virtual bool listComponents(const QString& raid_device,
                                QStringList& block_devices) const = 0;
};


#endif
