#include "disk_controller.hpp"
#include <QDirIterator>
#include <QStringList>

#include "idisk_filter.hpp"
#include "iblock_device.hpp"

#include "disk.hpp"

DiskController::DiskController(IFileSystem* filesystem): m_fileSystem(filesystem)
{

}

std::vector<std::unique_ptr<IBlockDevice>> DiskController::listDisks(const IDiskFilter& filter) const
{
    std::vector<std::unique_ptr<IBlockDevice>> disks;
    QDirIterator di("/sys/block", QStringList("sd*"), QDir::Dirs |
                    QDir::NoDotAndDotDot);
    while (di.hasNext())
    {
        di.next();
        std::unique_ptr<Disk> disk(new Disk(di.fileName(), m_fileSystem));
        if (filter(*disk))
            disks.emplace_back(std::move(disk));
    }

    return disks;
}
