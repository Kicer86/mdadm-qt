
#include <gmock/gmock.h>

#include "objects_ids.hpp"

TEST(StringIdTest, DefaultConstructor)
{
    string_id<int> id;

    EXPECT_EQ(id.value(), "");
}


TEST(StringIdTest, IntializationAtConstruction)
{
    string_id<int> id("123");

    EXPECT_EQ(id.value(), "123");
}


TEST(StringIdTest, LessComparator)
{
    string_id<int> id1("123");
    string_id<int> id2("124");

    EXPECT_TRUE(id1 < id2);
    EXPECT_FALSE(id2 < id1);
}


TEST(StringIdTest, EqualComparator)
{
    string_id<int> id1("123");
    string_id<int> id2("124");
    string_id<int> id3("124");

    EXPECT_TRUE(id2 == id3);
    EXPECT_FALSE(id2 == id1);
}


TEST(StringIdTest, NotEqualComparator)
{
    string_id<int> id1("123");
    string_id<int> id2("124");
    string_id<int> id3("124");

    EXPECT_FALSE(id2 != id3);
    EXPECT_TRUE(id2 != id1);
}
