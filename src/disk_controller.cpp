#include "disk_controller.hpp"

#include <QStringList>

#include "idisk_filter.hpp"
#include "iblock_device.hpp"
#include "ifilesystem.hpp"

#include "disk.hpp"

DiskController::DiskController(IFileSystem* filesystem): m_fileSystem(filesystem)
{

}

std::vector<std::unique_ptr<IBlockDevice>> DiskController::listDisks(const IDiskFilter& filter) const
{
    std::vector<std::unique_ptr<IBlockDevice>> disks;

    const std::deque<QString> files = m_fileSystem->listDir("/sys/block", "sd*", QDir::Dirs | QDir::NoDotAndDotDot);

    for(const QString& file_name: files)
    {
        std::unique_ptr<Disk> disk(new Disk(file_name, m_fileSystem));
        if (filter(*disk))
            disks.emplace_back(std::move(disk));
    }

    return disks;
}
