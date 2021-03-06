
#include <QTextStream>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
#include "imdadm_process_mock.hpp"
#include "iproc_watcher_mock.hpp"
#include "printers_for_gmock.hpp"
#include "raid_info_provider.hpp"

using testing::_;
using testing::DoAll;
using testing::InvokeArgument;
using testing::Return;
using testing::SetArgReferee;
using testing::NiceMock;

// TODO: consider parametric tests here, it may work.

namespace
{
    struct RaidDetails
    {
        QString type;
        QString device;
        QList<RaidComponentInfo> components;

        RaidDetails(const QString& dev,
                    const QList<RaidComponentInfo>& comps,
                    const QString& t):
            type(t),
            device(dev),
            components(comps)
        {

        }
    };

    typedef std::unique_ptr<IFileSystemMock::IFileMock> FilePtr;

    void mockMdstatOutput(IFileSystemMock& filesystem, QTextStream* const outputStream)
    {
        ON_CALL(filesystem, openFile(QString("/proc/mdstat"), QIODevice::ReadOnly |
                                                              QIODevice::Text))
                .WillByDefault(::testing::Invoke(
                            [outputStream](const QString&, QIODevice::OpenMode)
        {
            outputStream->seek(0);
            FilePtr ifile(new NiceMock<IFileSystemMock::IFileMock>);
            ON_CALL(*ifile, getStream()).WillByDefault(Return(outputStream));
            return ifile;
        }));
    }

    void compareListOutput(IRaidInfoProvider* infoProvider,
                        const std::vector<RaidDetails>& expected)
    {
        const std::vector<RaidInfo> raids = infoProvider->listRaids();

        ASSERT_EQ(raids.size(), expected.size());

        for (std::size_t i = 0; i < raids.size(); i++)
        {
            const RaidInfo& element = raids[i];
            const QString& element_dev = element.device();

            std::vector<RaidDetails>::const_iterator it =
                std::find_if(expected.cbegin(), expected.cend(),
                             [&element_dev](const RaidDetails& expect)
                             {
                                 return expect.device == element_dev;
                             });

            ASSERT_NE(it, expected.cend());

            const RaidDetails& expect = *it;

            EXPECT_EQ(element.type(), expect.type);
            EXPECT_EQ(element.device(), expect.device);
            EXPECT_EQ(element.devices(), expect.components);
        }
    }


    struct ProviderSignalWatcher: QObject
    {
        ProviderSignalWatcher(IRaidInfoProvider* provider)
        {
            connect(provider, &IRaidInfoProvider::raidRemoved,
                    this, &ProviderSignalWatcher::raidRemoved);

            connect(provider, &IRaidInfoProvider::raidAdded,
                    this, &ProviderSignalWatcher::raidAdded);

            connect(provider, &IRaidInfoProvider::raidChanged,
                    this, &ProviderSignalWatcher::raidChanged);
        }

        MOCK_METHOD1(raidRemoved, void(const RaidId &));
        MOCK_METHOD1(raidAdded, void(const RaidId &));
        MOCK_METHOD1(raidChanged, void(const RaidId &));
    };

}


TEST(RaidInfoProviderTests,
     listInactiveRaid0)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4]\n"
                         "md1 : inactive sdf[1](S)\n"
                         "      130048 blocks super 1.2\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput);

    mockMdstatOutput(filesystem, &outputStream);

    RaidInfoProvider infoProvider(&filesystem, &procWatcher);

    const QList<RaidComponentInfo> components =
    {
        RaidComponentInfo("sdf", RaidComponentInfo::Type::Spare, 1)
    };
    const std::vector<RaidDetails> expectedOutput =
    {
        RaidDetails("md1", components, "")
    };

    compareListOutput(&infoProvider, expectedOutput);
}


TEST(RaidInfoProviderTests,
     listActiveRaid5)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4]\n"
                         "md0 : active raid5 sdb[1] sdc[3] sdd[0]\n"
                         "      5860270080 blocks super 1.2 level 5, "
                         "512k chunk, algorithm 2 [3/3] [UUU]\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    RaidInfoProvider infoProvider(&filesystem, &procWatcher);

    const QList<RaidComponentInfo> components =
    {
        RaidComponentInfo("sdb", RaidComponentInfo::Type::Normal, 1),
        RaidComponentInfo("sdc", RaidComponentInfo::Type::Normal, 3),
        RaidComponentInfo("sdd", RaidComponentInfo::Type::Normal, 0),
    };
    const std::vector<RaidDetails> expectedOutput =
    {
        RaidDetails("md0", components, "raid5")
    };

    compareListOutput(&infoProvider, expectedOutput);
}


TEST(RaidInfoProviderTests,
     listActiveRaid0Raid1Raid6)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "md11 : active raid0 sdm[1] sdl[0]\n"
                         "       260096 blocks super 1.2 512k chunks\n"
                         "\n"
                         "md8 : active raid6 sdk[3] sdj[2] sdi[1] sdh[0]\n"
                         "      260096 blocks super 1.2 level 6, 512k chunk, "
                         "algorithm 2 [4/4] [UUUU]\n"
                         "\n"
                         "md3 : active raid1 sdg[1] sdf[0]\n"
                         "      130880 blocks super 1.2 [2/2] [UU]\n"
                         "\n"
                         "unused devices: <none>\n");

    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    RaidInfoProvider infoProvider(&filesystem, &procWatcher);

    const QList<RaidComponentInfo> components1 =
    {
        RaidComponentInfo("sdm", RaidComponentInfo::Type::Normal, 1),
        RaidComponentInfo("sdl", RaidComponentInfo::Type::Normal, 0),
    };
    const QList<RaidComponentInfo> components2 =
    {
        RaidComponentInfo("sdk", RaidComponentInfo::Type::Normal, 3),
        RaidComponentInfo("sdj", RaidComponentInfo::Type::Normal, 2),
        RaidComponentInfo("sdi", RaidComponentInfo::Type::Normal, 1),
        RaidComponentInfo("sdh", RaidComponentInfo::Type::Normal, 0),
    };
    const QList<RaidComponentInfo> components3=
    {
        RaidComponentInfo("sdg", RaidComponentInfo::Type::Normal, 1),
        RaidComponentInfo("sdf", RaidComponentInfo::Type::Normal, 0),
    };
    const std::vector<RaidDetails> expectedOutput =
    {
        RaidDetails("md11", components1, "raid0"),
        RaidDetails("md8", components2, "raid6"),
        RaidDetails("md3", components3, "raid1")
    };

    compareListOutput(&infoProvider, expectedOutput);
}


TEST(RaidInfoProviderTests, listNoRaids)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "\n"
                         "unused devices: <none>\n");

    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    RaidInfoProvider infoProvider(&filesystem, &procWatcher);

    const std::vector<RaidDetails> expectedOutput;

    compareListOutput(&infoProvider, expectedOutput);
}


TEST(RaidInfoProviderTests, listDegradedRaid)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "md8 : active (auto-read-only) raid6 sdk[3] sdj[2] "
                         "sdh[0]\n"
                         "      260096 blocks super 1.2 level 6, 512k chunk, "
                         "algorithm 2 [4/3] [U_UU]\n"
                         "\n"
                         "unused devices: <none>\n");

    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    RaidInfoProvider raidInfoProvider(&filesystem, &procWatcher);

    const QList<RaidComponentInfo> components =
    {
        RaidComponentInfo("sdk", RaidComponentInfo::Type::Normal, 3),
        RaidComponentInfo("sdj", RaidComponentInfo::Type::Normal, 2),
        RaidComponentInfo("sdh", RaidComponentInfo::Type::Normal, 0),
    };
    const std::vector<RaidDetails> expectedOutput =
    {
        RaidDetails("md8", components, "raid6"),
    };

    compareListOutput(&raidInfoProvider, expectedOutput);
}


TEST(RaidInfoProviderTests, reactsOnNewRaid)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString emptyMdstat;
    QString oneRaidMdstat("Personalities : [raid6] [raid5] [raid4] [raid0] "
                          "[raid1] [raid10]\n"
                          "md8 : active (auto-read-only) raid6 sdk[3] sdj[2] "
                          "sdh[0]\n"
                          "      260096 blocks super 1.2 level 6, 512k chunk, "
                          "algorithm 2 [4/3] [U_UU]\n"
                          "\n"
                          "unused devices: <none>\n");

    QString mdstatContent = emptyMdstat;
    QTextStream mdstatContentStream(&mdstatContent, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &mdstatContentStream);

    RaidInfoProvider raidInfoProvider(&filesystem, &procWatcher);
    ProviderSignalWatcher watcher(&raidInfoProvider);

    EXPECT_CALL(watcher, raidAdded(RaidId("md8")));

    // replace mdstat's content and reload
    mdstatContent = oneRaidMdstat;
    emit procWatcher.changed("/proc/mdstat");
}


TEST(RaidInfoProviderTests, reactsOnRaidRemoval)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString threeRaidsMdstat("Personalities : [raid6] [raid5] [raid4] [raid0] "
                             "[raid1] [raid10]\n"
                             "md11 : active raid0 sdm[1] sdl[0]\n"
                             "       260096 blocks super 1.2 512k chunks\n"
                             "\n"
                             "md8 : active raid6 sdk[3] sdj[2] sdi[1] sdh[0]\n"
                             "      260096 blocks super 1.2 level 6, 512k chunk, "
                             "algorithm 2 [4/4] [UUUU]\n"
                             "\n"
                             "md3 : active raid1 sdg[1] sdf[0]\n"
                             "      130880 blocks super 1.2 [2/2] [UU]\n"
                             "\n"
                             "unused devices: <none>\n");

    QString oneRaidMdstat("Personalities : [raid6] [raid5] [raid4] [raid0] "
                          "[raid1] [raid10]\n"
                          "md8 : active raid6 sdk[3] sdj[2] sdi[1] sdh[0]\n"
                          "      260096 blocks super 1.2 level 6, 512k chunk, "
                          "algorithm 2 [4/4] [UUUU]\n"
                          "\n"
                          "unused devices: <none>\n");

    QString mdstatContent = threeRaidsMdstat;
    QTextStream mdstatContentStream(&mdstatContent, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &mdstatContentStream);

    RaidInfoProvider raidInfoProvider(&filesystem, &procWatcher);
    ProviderSignalWatcher watcher(&raidInfoProvider);

    EXPECT_CALL(watcher, raidRemoved(RaidId("md11")));
    EXPECT_CALL(watcher, raidRemoved(RaidId("md3")));

    // replace mdstat's content and reload
    mdstatContent = oneRaidMdstat;
    emit procWatcher.changed("/proc/mdstat");
}


TEST(RaidInfoProviderTests, reactsOnRaidDegradation)
{
    NiceMock<IFileSystemMock> filesystem;
    IProcWatcherMock procWatcher;

    QString threeRaidsMdstat("Personalities : [raid6] [raid5] [raid4] [raid0] "
                             "[raid1] [raid10]\n"
                             "md11 : active raid0 sdm[1] sdl[0]\n"
                             "       260096 blocks super 1.2 512k chunks\n"
                             "\n"
                             "md8 : active raid6 sdk[3] sdj[2] sdi[1] sdh[0]\n"
                             "      260096 blocks super 1.2 level 6, 512k chunk, "
                             "algorithm 2 [4/4] [UUUU]\n"
                             "\n"
                             "md3 : active raid1 sdg[1] sdf[0]\n"
                             "      130880 blocks super 1.2 [2/2] [UU]\n"
                             "\n"
                             "unused devices: <none>\n");

    QString degradatedRaids("Personalities : [raid6] [raid5] [raid4] [raid0] "
                            "[raid1] [raid10]\n"
                            "md11 : active raid0 sdm[1] sdl[0]\n"
                            "       260096 blocks super 1.2 512k chunks\n"
                            "\n"
                            "md8 : active raid6 sdk[3] sdi[1] sdh[0]\n"
                            "      260096 blocks super 1.2 level 6, 512k chunk, "
                            "algorithm 2 [4/4] [UU_U]\n"
                            "\n"
                            "md3 : active raid1 sdg[1] sdf[0]\n"
                            "      130880 blocks super 1.2 [2/2] [UU]\n"
                            "\n"
                            "unused devices: <none>\n");

    QString mdstatContent = threeRaidsMdstat;
    QTextStream mdstatContentStream(&mdstatContent, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &mdstatContentStream);

    RaidInfoProvider raidInfoProvider(&filesystem, &procWatcher);
    ProviderSignalWatcher watcher(&raidInfoProvider);

    EXPECT_CALL(watcher, raidChanged(RaidId("md8")));

    // replace mdstat's content and reload
    mdstatContent = degradatedRaids;
    emit procWatcher.changed("/proc/mdstat");
}
