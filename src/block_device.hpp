#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include "iblock_device.hpp"

#include <QString>

class BlockDevice : public IBlockDevice
{
public:
    BlockDevice(const QString&);
    BlockDevice(BlockDevice&&) = default;

    size_t size() const;
    size_t sizeInSectorUnits() const;
    unsigned logicalBlockSize() const;
    bool isUsed() const;

protected:
    QString m_name;
    unsigned m_logical_block_size;
    size_t m_size;

    size_t getSize() const;
    unsigned getLogicalBlockSize() const;

private:
    BlockDevice(const BlockDevice&) = delete;
    BlockDevice& operator=(const BlockDevice&) = delete;

};

#endif // BLOCK_DEVICE_H
