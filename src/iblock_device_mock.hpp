
#ifndef IBLOCK_DEVICE_MOCK_HPP
#define IBLOCK_DEVICE_MOCK_HPP

#include <gmock/gmock.h>

#include "iblock_device.hpp"

struct IBlockDeviceMock: IBlockDevice
{
    MOCK_CONST_METHOD0(size, size_t());
    MOCK_CONST_METHOD0(sizeInSectorUnits, size_t());
    MOCK_CONST_METHOD0(logicalBlockSize, unsigned());
    MOCK_CONST_METHOD0(isUsed, bool());
    MOCK_CONST_METHOD0(devPath, QString());
    MOCK_CONST_METHOD0(toString, QString());
};

#endif
