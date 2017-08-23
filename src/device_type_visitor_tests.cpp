
#include <gmock/gmock.h>

#include "device_type_visitor.hpp"
#include "disk.hpp"


TEST(DeviceTypeVisitorTest, DiskDeviceRecognition)
{
    bool is_type_of_disk = false;

    DeviceTypeVisitor visitor;
    visitor.setDiskHandler([&is_type_of_disk](Disk *) { is_type_of_disk = true; } );

    Disk diskDev;

    visitor.goFor(&diskDev);

    EXPECT_TRUE(is_type_of_disk);
}


TEST(DeviceTypeVisitorTest, DiskDeviceRecognitionWithoutHandler)   // no crash etc
{
    bool is_type_of_disk = false;

    DeviceTypeVisitor visitor;
    Disk diskDev;

    visitor.goFor(&diskDev);

    EXPECT_FALSE(is_type_of_disk);
}
