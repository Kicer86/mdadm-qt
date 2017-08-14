#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "block_device.hpp"
#include "ifilesystem.hpp"
#include "utils.hpp"

namespace
{
    size_t getSize(IFileSystem* fs, const QString& name)
    {
        return utils::readValueFromFile<size_t>(fs,
                                                "/sys/block/" + name + "/size");
    }

    unsigned getLogicalBlockSize(IFileSystem* fs, const QString& name)
    {
        return utils::readValueFromFile<unsigned>(fs,
                                                  "/sys/block/" + name
                                                    + "/queue/logical_block_size");
    }
}


BlockDevice::BlockDevice():
    IBlockDevice(),
    m_name(""),
    m_logical_block_size(0),
    m_size(0),
    m_fileSystem(nullptr)
{
}


BlockDevice::BlockDevice(const QString& name, IFileSystem* filesystem) :
    IBlockDevice(),
    m_name(name),
    m_logical_block_size(getLogicalBlockSize(filesystem, name)),
    m_size(getSize(filesystem, name)),
    m_fileSystem(filesystem)
{

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

bool BlockDevice::isUsed() const
{

    const QString dev_path("/dev/" + m_name);
    const bool used = m_fileSystem->openFile(dev_path)->getStream() == nullptr;

    return used;
}

bool BlockDevice::isPhysical() const
{
    return true;
}
