
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <vector>

#include "raid_info.hpp"
#include "objects_ids.hpp"

struct IRaidInfoProvider
{
    virtual ~IRaidInfoProvider() = default;

    virtual std::vector<RaidInfo> listRaids() const = 0;
    virtual RaidInfo getInfoFor(const RaidId &) const = 0;

    /*
     * device types in mdstat:
     * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/md/md.c#n7711
     */

    virtual QString raidDevice(const RaidId &) const = 0;
    virtual QList<RaidComponentInfo> blockDevices(const RaidId &) const = 0;
    virtual QString raidType(const RaidId &) const = 0;

    // TODO: refactor, base on RaidId
    virtual bool listComponents(const QString& raid_device,
                                QStringList& block_devices) const = 0;
};


#endif
