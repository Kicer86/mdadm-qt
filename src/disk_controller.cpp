#include "disk_controller.hpp"
#include <QDirIterator>
#include <QStringList>

DiskController::DiskController()
{

}

bool DiskController::listDisks(const ListResult &result)
{
    std::vector<Disk> disks;
    QDirIterator di("/sys/block", QStringList("sd*"), QDir::Dirs |
                    QDir::NoDotAndDotDot);
    while (di.hasNext())
    {
        di.next();
        disks.emplace_back(di.fileName());
    }

    result(disks);
}
