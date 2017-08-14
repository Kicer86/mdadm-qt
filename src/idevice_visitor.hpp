
#ifndef IDEVICE_VISITOR
#define IDEVICE_VISITOR

class Disk;
class Missing;


struct IDeviceVisitor
{
    virtual ~IDeviceVisitor() = default;

    virtual void visit(Disk *) = 0;
    virtual void visit(Missing *) = 0;
};

#endif
