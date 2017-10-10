#ifndef IRAIDINFOPROVIDER_MOCK_HPP
#define IRAIDINFOPROVIDER_MOCK_HPP

#include <gmock/gmock.h>

#include "iraid_info_provider.hpp"

struct IRaidInfoProviderMock: IRaidInfoProvider
{
    MOCK_CONST_METHOD0(listRaids, std::vector<RaidId>());
    MOCK_CONST_METHOD1(getInfoFor, RaidInfo(const RaidId &));
    MOCK_CONST_METHOD2(listComponents, bool(const QString& raid_device,
                                            QStringList& block_devices));
};

#endif // IRAIDINFOPROVIDER_MOCK_HPP
