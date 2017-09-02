
#include <gmock/gmock.h>

#include "missing.hpp"
#include "iblock_device_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::Return;


TEST(MissingTest, returnsSizeValuesOfZero)
{
    Missing missing;

    EXPECT_EQ(missing.size(), 0);
    EXPECT_EQ(missing.sizeInSectorUnits(), 0);
    EXPECT_EQ(missing.logicalBlockSize(), 0);
}


TEST(MissingTest, returnsFalseForIsUsed)
{
    Missing missing;

    EXPECT_FALSE(missing.isUsed());
}


TEST(MissingTest, returnsFalseForIsPhysical)
{
    Missing missing;

    EXPECT_FALSE(missing.isPhysical());
}


TEST(MissingTest, returnsMissingAsPath)
{
    Missing missing;

    EXPECT_TRUE(missing.devPath() == "missing");
}


TEST(MissingTest, returnsProperStringRepresentation)
{
    Missing missing;

    EXPECT_TRUE(missing.toString() == "Missing device representation");
}


TEST(MissingTest, returnsCorrectValuesForInterface)
{
    IBlockDevice *missing = new(Missing);

    EXPECT_EQ(missing->size(), 0);
    EXPECT_EQ(missing->sizeInSectorUnits(), 0);
    EXPECT_EQ(missing->logicalBlockSize(), 0);
    EXPECT_FALSE(missing->isPhysical());
    EXPECT_FALSE(missing->isUsed());
    EXPECT_TRUE(missing->devPath() == "missing");
    EXPECT_TRUE(missing->toString() == "Missing device representation");

    delete missing;
}
