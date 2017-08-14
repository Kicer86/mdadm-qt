#ifndef IBLOCK_DEVICE_H
#define IBLOCK_DEVICE_H

#include <QString>

enum units { BYTES, SECTORS };

struct IDeviceVisitor;

class IBlockDevice
{
public:
    virtual ~IBlockDevice() = default;
    virtual size_t size() const = 0;
    virtual size_t sizeInSectorUnits() const = 0;
    virtual unsigned logicalBlockSize() const = 0;
    virtual bool isUsed() const = 0;
    virtual bool isPhysical() const = 0;
    virtual QString devPath() const = 0;
    virtual QString toString() const = 0;

    virtual void accept(IDeviceVisitor  *) = 0;
};

#endif // IBLOCK_DEVICE_H
