
#ifndef IRAIDINFODATAPROVIDER_MOCK_HPP
#define IRAIDINFODATAPROVIDER_MOCK_HPP

#include <gmock/gmock.h>

#include "raid_info.hpp"


struct IRaidInfoDataProviderMock: IRaidInfoDataProvider
{
    MOCK_CONST_METHOD1(raidDevice, const QString& (const RaidId &));
    MOCK_CONST_METHOD1(blockDevices, const QList<RaidComponentInfo>& (const RaidId &));
    MOCK_CONST_METHOD1(raidType, const QString& (const RaidId &));
};

#endif // IRAIDINFODATAPROVIDER_MOCK_HPP

