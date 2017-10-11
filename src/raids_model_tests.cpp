


#include <gmock/gmock.h>

#include "raids_model.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::NiceMock;


class RaidsModelTests: public testing::Test
{
    public:

    protected:
        const RaidComponentInfo dev1 = RaidComponentInfo
        (
            "dev1",
            RaidComponentInfo::Type::Normal,
            128
        );

        const RaidComponentInfo dev2 = RaidComponentInfo
        (
            "dev2",
            RaidComponentInfo::Type::Spare,
            129
        );

        const RaidComponentInfo dev3 = RaidComponentInfo
        (
            "dev3",
            RaidComponentInfo::Type::Normal,
            130
        );

        const RaidComponentInfo dev4 = RaidComponentInfo
        (
            "dev4",
            RaidComponentInfo::Type::Spare,
            131
        );

        const RaidComponentInfo dev5 = RaidComponentInfo
        (
            "dev5",
            RaidComponentInfo::Type::Normal,
            132
        );

        const RaidComponentInfo dev6 = RaidComponentInfo
        (
            "dev6",
            RaidComponentInfo::Type::Spare,
            133
        );

        const RaidComponentInfo dev7 = RaidComponentInfo
        (
            "dev7",
            RaidComponentInfo::Type::Normal,
            134
        );

        const RaidComponentInfo dev8 = RaidComponentInfo
        (
            "dev8",
            RaidComponentInfo::Type::Spare,
            135
        );

        const RaidInfo raid1 = RaidInfo
        (
            "md1",
            { dev1, dev2 },
             "type 0"
        );

        const RaidInfo raid2 = RaidInfo
        (
            "md2",
            { dev3, dev4, dev5, dev6 },
             "type 1"
        );

        const RaidInfo raid3 = RaidInfo
        (
            "md3",
            { dev7, dev8 },
             "type 2"
        );
};


TEST_F(RaidsModelTests, isEmptyWhenConstructed)
{
    IRaidInfoProviderMock raidInfoProvider;
    RaidsModel model(&raidInfoProvider);

    QAbstractItemModel* qt_model = model.model();

    EXPECT_EQ(false, qt_model->canFetchMore(QModelIndex()));
    EXPECT_EQ(0, qt_model->rowCount());
    EXPECT_EQ(3, qt_model->columnCount());
}


TEST_F(RaidsModelTests, addingRaidsToEmptyModel)
{
    RaidsModel model;

    const std::vector<RaidInfo> raids = {raid1, raid2};

    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    if (qt_model->canFetchMore(QModelIndex()))
       qt_model->fetchMore(QModelIndex());

    EXPECT_EQ(3, qt_model->columnCount());
    ASSERT_EQ(2, qt_model->rowCount());

    const QModelIndex first_raid = qt_model->index(0, 0);
    const QModelIndex second_raid = qt_model->index(1, 0);

    if (qt_model->canFetchMore(first_raid))
        qt_model->fetchMore(first_raid);

    if (qt_model->canFetchMore(second_raid))
        qt_model->fetchMore(second_raid);

    EXPECT_EQ(2, qt_model->rowCount(first_raid));
    EXPECT_EQ(4, qt_model->rowCount(second_raid));
}


struct ModelSignalWatcher: QObject
{
    ModelSignalWatcher(QAbstractItemModel* model)
    {
        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &ModelSignalWatcher::rowsRemoved);

        connect(model, &QAbstractItemModel::rowsInserted,
                this, &ModelSignalWatcher::rowsInserted);

        connect(model, &QAbstractItemModel::dataChanged,
                this, &ModelSignalWatcher::dataChanged);
    }

    MOCK_METHOD3(rowsRemoved, void(const QModelIndex &parent, int first, int last));
    MOCK_METHOD3(rowsInserted, void(const QModelIndex &parent, int first, int last));
    MOCK_METHOD3(dataChanged, void(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight,
                                   const QVector<int> &roles));
};


TEST_F(RaidsModelTests, modelItemsHaveExpectedTypes)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    const QModelIndex raid1Idx = qt_model->index(0, 0);
    const QModelIndex raid3Idx = qt_model->index(2, 0);

    EXPECT_EQ(RaidsModel::Raid, model.getTypeFor(raid1Idx));
    EXPECT_EQ(RaidsModel::Raid, model.getTypeFor(raid3Idx));

    const QModelIndex raid1CompIdx = qt_model->index(1, 0, raid1Idx);
    const QModelIndex raid3CompIdx = qt_model->index(1, 0, raid3Idx);

    EXPECT_EQ(RaidsModel::Component, model.getTypeFor(raid1CompIdx));
    EXPECT_EQ(RaidsModel::Component, model.getTypeFor(raid3CompIdx));
}


TEST_F(RaidsModelTests, gettingRaidsInfo)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    const QModelIndex raid1Idx = qt_model->index(0, 0);
    const QModelIndex raid2Idx = qt_model->index(1, 0);
    const QModelIndex raid3Idx = qt_model->index(2, 0);

    EXPECT_EQ(raid1, model.infoForRaid(raid1Idx));
    EXPECT_EQ(raid2, model.infoForRaid(raid2Idx));
    EXPECT_EQ(raid3, model.infoForRaid(raid3Idx));
}


TEST_F(RaidsModelTests, gettingComponentsInfo)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    const QModelIndex raid2Idx = qt_model->index(1, 0);

    const QModelIndex raid1Comp1Idx = qt_model->index(0, 0, raid2Idx);
    const QModelIndex raid1Comp2Idx = qt_model->index(1, 0, raid2Idx);
    const QModelIndex raid1Comp3Idx = qt_model->index(2, 0, raid2Idx);
    const QModelIndex raid1Comp4Idx = qt_model->index(3, 0, raid2Idx);

    EXPECT_EQ(dev3, model.infoForComponent(raid1Comp1Idx));
    EXPECT_EQ(dev4, model.infoForComponent(raid1Comp2Idx));
    EXPECT_EQ(dev5, model.infoForComponent(raid1Comp3Idx));
    EXPECT_EQ(dev6, model.infoForComponent(raid1Comp4Idx));
}


TEST_F(RaidsModelTests, removingRaidsFromModel)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    ModelSignalWatcher signalWatcher(qt_model);

    // top item (raid2)
    EXPECT_CALL(signalWatcher, rowsRemoved(QModelIndex(), 1, 1));

    // raid1's items
    EXPECT_CALL(signalWatcher, rowsRemoved(qt_model->index(1, 0) ,_ ,_))
        .Times(4);

    const std::vector<RaidInfo> raidsAfterChange = {raid1, raid3};
    model.load(raidsAfterChange);
}


TEST_F(RaidsModelTests, appendingRaidsToModel)
{
    const std::vector<RaidInfo> raids = {raid1, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    ModelSignalWatcher signalWatcher(qt_model);

    // top item (raid2) will be added at the end (appended)
    EXPECT_CALL(signalWatcher, rowsInserted(QModelIndex(), 2, 2));

    // raid2's items cannot be observed here for 2 reasons:
    // 1. current algorithm creates item for raid (but doesn't add it to the model yet),
    //    then attaches components to it, and finally appends it to the model.
    //    So model emits only one 'rowsInserted'.
    // 2. We cannot ask model for QModelIndex of 'raid2' index, as
    //    it doesn't exist in model yet.

    const std::vector<RaidInfo> raidsAfterChange = {raid1, raid2, raid3};
    model.load(raidsAfterChange);

    // Make sure last item has 4 leafs
    const QModelIndex raid2Idx = qt_model->index(2, 0);
    EXPECT_EQ(4, qt_model->rowCount(raid2Idx));
}


TEST_F(RaidsModelTests, raidComponentRemoved)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    // Use nice mock.
    // We do not care currently what signals will be emited when
    // refreshing list of components.
    NiceMock<ModelSignalWatcher> signalWatcher(qt_model);

    RaidInfo newRaid2 = raid2;
    newRaid2.block_devices.pop_back();    // drop last component from raid

    const std::vector<RaidInfo> raidsAfterChange = {raid1, newRaid2, raid3};
    model.load(raidsAfterChange);

    // When model is updated, we expect raid2 will have 3 leafs instead of 4
    const QModelIndex raid2Idx = qt_model->index(1, 0);
    EXPECT_EQ(3, qt_model->rowCount(raid2Idx));
}


TEST_F(RaidsModelTests, raidComponentTypeChanged)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    // Use nice mock.
    // We do not care currently what signals will be emited when
    // refreshing list of components.
    NiceMock<ModelSignalWatcher> signalWatcher(qt_model);

    RaidInfo newRaid2 = raid2;
    EXPECT_NE(newRaid2.block_devices.front().type,
              RaidComponentInfo::Type::Journal);

    newRaid2.block_devices.front().type = RaidComponentInfo::Type::Journal;

    const std::vector<RaidInfo> raidsAfterChange = {raid1, newRaid2, raid3};
    model.load(raidsAfterChange);

    const QModelIndex raid2Idx = qt_model->index(1, 0);
    const QModelIndex raid2Comp1Idx = raid2Idx.child(0, 2);  // index to type of first component
    const QVariant data = raid2Comp1Idx.data();
    const QString typeStr = data.toString();

    EXPECT_EQ(typeStr, "journal");

    const RaidComponentInfo& compInfo = model.infoForComponent(raid2Comp1Idx);
    EXPECT_EQ(compInfo.type, RaidComponentInfo::Type::Journal);

    const RaidInfo& raidInfo = model.infoForRaid(raid2Idx);
    EXPECT_EQ(raidInfo.block_devices.front().type, RaidComponentInfo::Type::Journal);
}


TEST_F(RaidsModelTests, raidTypeChanged)
{
    const std::vector<RaidInfo> raids = {raid1, raid2, raid3};

    RaidsModel model;
    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    NiceMock<ModelSignalWatcher> signalWatcher(qt_model);

    const QModelIndex raid2TypeIdx = qt_model->index(1, 1);
    EXPECT_CALL(signalWatcher, dataChanged(raid2TypeIdx, raid2TypeIdx, _))
        .Times(1);

    RaidInfo newRaid2 = raid2;
    newRaid2.raid_type = "new type";

    const std::vector<RaidInfo> raidsAfterChange = {raid1, newRaid2, raid3};
    model.load(raidsAfterChange);

    const RaidInfo& raidInfo = model.infoForRaid(raid2TypeIdx);
    EXPECT_EQ(raidInfo, newRaid2);
}
