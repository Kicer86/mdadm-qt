
#include <QTextStream>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
#include "imdadm_process_mock.hpp"
#include "iraid_info_provider_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::DoAll;
using testing::InvokeArgument;
using testing::Return;
using testing::SetArgReferee;


TEST(MDAdmControllerTest, usesRightParametersForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md0",
                                        "--level", "stripe", "--raid-devices=2",
                                        "/dev/sda", "/dev/sdc" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md0",
                          MDAdmController::Type::Raid0,
                          QStringList({"/dev/sda", "/dev/sdc"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "stripe", "--raid-devices=2",
                                        "/dev/sda", "/dev/sdc",
                                        "--spare-devices=1", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("error"), false, 2),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid0,
                          QStringList({"/dev/sda", "/dev/sdc"}),
                          QStringList("/dev/sde"));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid1Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "mirror", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdc", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sda", "/dev/sdc", "/dev/sde"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid1Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "mirror", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdc", "/dev/sde",
                                        "--spare-devices=2", "/dev/sdf",
                                        "/dev/sdg"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sda", "/dev/sdc", "/dev/sde"}),
                          QStringList({"/dev/sdf", "/dev/sdg"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid4Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md2",
                                        "--level", "4", "--raid-devices=4",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc",
                                        "/dev/sdd" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md2",
                          MDAdmController::Type::Raid4,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc",
                                       "/dev/sdd"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid4Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md7",
                                        "--level", "4", "--raid-devices=4",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "--spare-devices=1",
                                        "/dev/sdg"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md7",
                          MDAdmController::Type::Raid4,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde"}),
                          QStringList({"/dev/sdg"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid5Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md3",
                                        "--level", "5", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md3",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid5Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md8",
                                        "--level", "5", "--raid-devices=4",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "--spare-devices=2",
                                        "/dev/sdg", "/dev/sdh"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md8",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde"}),
                          QStringList({"/dev/sdg", "/dev/sdh"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid6Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md4",
                                        "--level", "6", "--raid-devices=5",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc",
                                        "/dev/sdd", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md4",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc",
                                       "/dev/sdd", "/dev/sde"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid6Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md2",
                                        "--level", "6", "--raid-devices=5",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "/dev/sdf",
                                        "--spare-devices=3",
                                        "/dev/sdg", "/dev/sdh", "/dev/sdad"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md2",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde", "/dev/sdf"}),
                          QStringList({"/dev/sdg", "/dev/sdh", "/dev/sdad"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid1CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md7", "--level", "mirror",
        "--raid-devices=2", "/dev/sdb", "missing"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md7",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sdb", "missing"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParametersForRaid5CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md8", "--level", "5",
        "--raid-devices=3", "/dev/sdm", "missing", "/dev/sdn"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md8",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sdm", "missing", "/dev/sdn"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParametersForRaid6CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md127", "--level", "6",
        "--raid-devices=4", "/dev/sdl", "/dev/sdm", "missing",
        "missing"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.createRaid("/dev/md127",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sdl", "/dev/sdm", "missing",
                                       "missing"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParameterForRaidStop)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.stopRaid("/dev/md127");
}


TEST(MDAdmControllerTest,
     usesEmptyListForZeroSuperblockFunction)
{
    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    EXPECT_FALSE(controller.zeroSuperblock(QStringList()));
}


TEST(MDAdmControllerTest,
     usesRightParameterForClearingMetadataFromSingleDrive)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb" }));
}


TEST(MDAdmControllerTest,
     usesRightParameterForClearingMetadataFromMultipleDrives)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb",
        "/dev/sdc",
        "/dev/sde"
    };


    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);

    const QStringList devices = { "/dev/sdb", "/dev/sdc", "/dev/sde"};
    EXPECT_TRUE(controller.zeroSuperblock(devices));
}


TEST(MDAdmControllerTest,
     usesRightParameterForRaidRemoval)
{
    IRaidInfoProviderMock raid_info_provider;
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127",
        "--zero-superblock",
        "/dev/sdb",
        "/dev/sdc",
        "/dev/sdd"
    };

    const QStringList slaves = {"/dev/sdb", "/dev/sdc", "/dev/sdd"};
    const QString device_name = "md127";

    EXPECT_CALL(raid_info_provider, listComponents(device_name, _))
        .WillOnce(DoAll(SetArgReferee<1>(slaves), Return(true)));

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, &raid_info_provider, nullptr);
    EXPECT_TRUE(controller.removeRaid("/dev/md127"));
}


TEST(MDAdmControllerTest,
     usesRightParameterForEmptyRaidRemoval)
{
    IMDAdmProcessMock mdadm_process;
    IRaidInfoProviderMock raidInfoProvider;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md4"
    };

    const QString device_name = "md4";

    EXPECT_CALL(raidInfoProvider, listComponents(device_name, _))
        .WillOnce(Return(false));

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, &raidInfoProvider, nullptr);
    EXPECT_TRUE(controller.removeRaid("/dev/md4"));
}


TEST(MDAdmControllerTest,
     simulateProcessCrash)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("crash"), false, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb" }));
}


TEST(MDAdmControllerTest, usesRightParameterForMarkAsFaulty)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "/dev/md10",
        "--fail",
        "/dev/sdo"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.markAsFaulty("/dev/md10", "/dev/sdo");
}


TEST(MDAdmControllerTest, usesRightParameterForReAdd)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "/dev/md2",
        "--re-add",
        "/dev/sdc"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr, nullptr);
    controller.reAdd("/dev/md2", "/dev/sdc");
}

void setUpScanInfoFakeFs(FakeFileSystem& fs, const QString& raid_device,
                         const QMap<QString, QString>& override)
{
    QMap<QString, QString> scanInfoFiles{
        { "last_sync_action", "resync" },
        { "mismatch_cnt", "0" },
        { "reshape_direction", "forwards" },
        { "reshape_position", "none" },
        { "resync_start", "none" },
        { "sync_action", "idle" },
        { "sync_completed", "none" },
        { "sync_max", "max" },
        { "sync_min", "0" },
        { "sync_speed", "none" },
        { "sync_speed_max", "200000 (system)" },
        { "sync_speed_min", "1000 (system)" },
    };

    for (const auto& key : override.keys())
    {
        scanInfoFiles.insert(key, override.value(key));
    }

    for (const auto& key : scanInfoFiles.keys())
    {
        fs.addFile(QString("/sys/block/%1/md/%2").arg(raid_device)
                           .arg(key),
                   scanInfoFiles.value(key));
    }
}

TEST(MDAdmControllerTest, usesRightParameterForIntegrityCheck)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "idle"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.runScan("md2", ScanInfo::ScanType::Check));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Check);
}


TEST(MDAdmControllerTest, usesRightParameterForRepair)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "idle"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.runScan("md2", ScanInfo::ScanType::Repair));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Repair);
}


TEST(MDAdmControllerTest, usesRightParameterForResync)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "idle"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.runScan("md2", ScanInfo::ScanType::Resync));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Resync);
}


TEST(MDAdmControllerTest, usesRightParameterForStopScan)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{
                            {"sync_action", "check"},
                            {"sync_min", "512"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.stopScan("md2"));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Idle);
    EXPECT_TRUE(std::get<0>(controller.getScanData("md2").scan_limits) == 0);
}


TEST(MDAdmControllerTest, getsCorrectRecoveryScanType)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "recovery"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Recovery);
}


TEST(MDAdmControllerTest, usesRightParameterForReshape)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "reshape"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.runScan("md2", ScanInfo::ScanType::Reshape));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Reshape);
}


TEST(MDAdmControllerTest, usesRightParameterForFreeze)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "frozen"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.runScan("md2", ScanInfo::ScanType::Frozen));
    EXPECT_TRUE(controller.getScanData("md2").sync_action == ScanInfo::ScanType::Frozen);
}


TEST(MDAdmControllerTest, usesRightParameterForPause)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{{"sync_action", "check"},
                                               {"sync_min", "512"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_TRUE(controller.pauseScan("md2"));

    const ScanInfo si = controller.getScanData("md2");
    EXPECT_TRUE(si.sync_action == ScanInfo::ScanType::Frozen);
    EXPECT_TRUE(std::get<0>(si.scan_limits) == 512);
}


TEST(MDAdmControllerTest, usesRightParameterForResume)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{
                            {"sync_action", "frozen"},
                            {"last_sync_action", "check"},
                            {"sync_min", "512"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());
    EXPECT_TRUE(controller.resumeScan("md2"));

    ScanInfo scan_info = controller.getScanData("md2");
    EXPECT_TRUE(scan_info.sync_action == ScanInfo::ScanType::Check);
    EXPECT_TRUE(std::get<0>(scan_info.scan_limits) == 512);

}


TEST(MDAdmControllerTest, resumeOfActiveScanShouldFail)
{
    FakeFileSystem fs;
    setUpScanInfoFakeFs(fs, "md2",
                        QMap<QString, QString>{
                            {"sync_action", "repair"},
                            {"last_sync_action", "check"},
                            {"sync_min", "512"}});

    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr, fs.getFileSystem());

    EXPECT_FALSE(controller.resumeScan("md2"));

    ScanInfo scan_info = controller.getScanData("md2");

    EXPECT_TRUE(scan_info.sync_action == ScanInfo::ScanType::Repair);
    EXPECT_TRUE(std::get<0>(scan_info.scan_limits) == 512);
}
