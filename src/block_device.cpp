#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "block_device.hpp"
#include "utils.hpp"

BlockDevice::BlockDevice(const QString& name, IFileSystem* filesystem) :
    IBlockDevice(),
    m_name(name),
    m_logical_block_size(getLogicalBlockSize()),
    m_size(getSize()),
    m_fileSystem(filesystem)
{

}

size_t BlockDevice::getSize() const
{
    return utils::readValueFromFile<size_t>("/sys/block/" + m_name + "/size");
}

unsigned BlockDevice::getLogicalBlockSize() const
{
    return utils::readValueFromFile<unsigned>("/sys/block/" + m_name
                                              + "/queue/logical_block_size");
}

inline size_t BlockDevice::size() const
{
    return m_size * m_logical_block_size;
}

inline size_t BlockDevice::sizeInSectorUnits() const
{
    return m_size;
}

inline unsigned BlockDevice::logicalBlockSize() const
{
    return m_logical_block_size;
}

bool BlockDevice::isUsed() const {
    const QString dev_path("/dev/" + m_name);
    bool ret = true;
    int fd = open(QFile::encodeName(dev_path), O_RDONLY | O_EXCL);
    if (fd > 0) {
        ret = false;
        close(fd);
    }
    return ret;
}
