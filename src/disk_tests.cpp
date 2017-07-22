
#include <QDebug>

#include <gmock/gmock.h>

#include "disk.hpp"
#include "ifilesystem_mock.hpp"

using testing::_;
using testing::Return;

TEST(DiskTest, simpleConstruction)
{
    IFileSystemMock fs;

    Disk disk("sda", &fs);
}
