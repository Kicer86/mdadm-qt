#include "disk_controller.hpp"
#include <QDirIterator>
#include <QStringList>

#include "idisk_filter.hpp"

DiskController::DiskController()
{

}

std::vector<Disk> DiskController::listDisks(const IDiskFilter& filter) const
{
    std::vector<Disk> disks;
    QDirIterator di("/sys/block", QStringList("sd*"), QDir::Dirs |
                    QDir::NoDotAndDotDot);
    while (di.hasNext())
    {
        di.next();
        Disk disk(di.fileName());
        if (filter(disk))
            disks.push_back(std::move(disk));
    }

    return disks;
}
