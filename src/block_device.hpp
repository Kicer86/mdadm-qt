#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <QString>

#include "iblock_device.hpp"

struct IFileSystem;

class BlockDevice : public IBlockDevice
{
public:
    BlockDevice(const QString &, IFileSystem *);
    BlockDevice(BlockDevice&&) = default;

    size_t size() const;
    size_t sizeInSectorUnits() const;
    unsigned logicalBlockSize() const;
    bool isUsed() const;

protected:
    QString m_name;
    unsigned m_logical_block_size;
    size_t m_size;
    IFileSystem* m_fileSystem;

    size_t getSize() const;
    unsigned getLogicalBlockSize() const;

private:
    BlockDevice(const BlockDevice&) = delete;
    BlockDevice& operator=(const BlockDevice&) = delete;

};

#endif // BLOCK_DEVICE_H
