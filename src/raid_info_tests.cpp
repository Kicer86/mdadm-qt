

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"

using testing::_;
using testing::NiceMock;


class RaidInfoOperatorTest: public ::testing::TestWithParam<std::pair<RaidInfo, RaidInfo>>
{

};


TEST_P(RaidInfoOperatorTest, operatorLess)
{
    const auto& raids = GetParam();

    EXPECT_EQ( true,  raids.first  < raids.second );       // 1st < 2nd
    EXPECT_EQ( false, raids.second < raids.first  );       // 2nd !< 1st
}

namespace
{
    const RaidComponentInfo dev1("dev1", RaidComponentInfo::Type::Normal, 128);
    const RaidComponentInfo dev2("dev2", RaidComponentInfo::Type::Normal, 129);
    const RaidComponentInfo dev3("dev3", RaidComponentInfo::Type::Normal, 130);
    const RaidComponentInfo dev4("dev4", RaidComponentInfo::Type::Normal, 131);
    const RaidComponentInfo dev5("dev5", RaidComponentInfo::Type::Normal, 132);
    const RaidComponentInfo dev6("dev6", RaidComponentInfo::Type::Normal, 133);
    const RaidComponentInfo dev7("dev7", RaidComponentInfo::Type::Normal, 134);
    const RaidComponentInfo dev8("dev8", RaidComponentInfo::Type::Normal, 135);

    const RaidInfo raid1("md1", { dev1, dev2 }, "type 0");
    const RaidInfo raid2("md2", { dev3, dev4, dev5, dev6 }, "type 1");
    const RaidInfo raid3("md1", { dev7, dev8 }, "type 2");
    const RaidInfo raid4("md1", { dev1, dev2 }, "type 2");
}


INSTANTIATE_TEST_CASE_P(RaidsSet,
                        RaidInfoOperatorTest,
                        ::testing::Values(
                            std::make_pair(raid1, raid2),        // different device
                            std::make_pair(raid1, raid3),        // defferent type
                            std::make_pair(raid4, raid3)         // different components
                        )
);
