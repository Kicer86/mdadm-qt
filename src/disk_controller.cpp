#include "disk_controller.hpp"
#include <QDirIterator>
#include <QStringList>

DiskController::DiskController()
{

}

std::vector<Disk> DiskController::listDisks(const DiskFilter& df) const
{
    std::vector<Disk> disks;
    QDirIterator di("/sys/block", QStringList("sd*"), QDir::Dirs |
                    QDir::NoDotAndDotDot);
    while (di.hasNext())
    {
        di.next();
        Disk disk(di.fileName());
        if (df(disk))
            disks.push_back(std::move(disk));
    }

    return disks;
}
