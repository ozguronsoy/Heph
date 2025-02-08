#include <gtest/gtest.h>
#include "Enum.h"

enum ClassicEnum
{
    None = 0,
    Val1 = 1,
    Val2 = 2,
    Val3 = 4,
    All = 7
};

enum class ScopedEnum
{
    None = 0,
    Val1 = 1,
    Val2 = 2,
    Val3 = 4,
    All = 7
};

TEST(HephTest, Enum_Classic)
{
    using __ENUM = ClassicEnum;

    constexpr Heph::Enum<__ENUM> eAll = __ENUM::All; 

    Heph::Enum<__ENUM> e1(__ENUM::Val2);
    Heph::Enum<__ENUM> e2(__ENUM::Val3);

    EXPECT_FALSE(e1.Test(__ENUM::Val1));
    EXPECT_TRUE(e1.Test(__ENUM::Val2));
    EXPECT_FALSE(e1.Test(__ENUM::Val3));
    EXPECT_FALSE(e1.Test(__ENUM::All));
    EXPECT_NE(e1, e2);

    e1.Set(__ENUM::Val3);
    e2.Set(__ENUM::Val2);

    EXPECT_FALSE(e1.Test(__ENUM::Val1));
    EXPECT_TRUE(e1.Test(__ENUM::Val2));
    EXPECT_TRUE(e1.Test(__ENUM::Val3));
    EXPECT_FALSE(e1.Test(__ENUM::All));
    EXPECT_EQ(e1, e2);

    e1 = eAll;
    EXPECT_EQ(e1, eAll);
    EXPECT_NE(e2, eAll);
}

TEST(HephTest, Enum_Scoped)
{
    using __ENUM = ScopedEnum;

    constexpr Heph::Enum<__ENUM> eAll = __ENUM::All; 

    Heph::Enum<__ENUM> e1(__ENUM::Val2);
    Heph::Enum<__ENUM> e2(__ENUM::Val3);

    EXPECT_FALSE(e1.Test(__ENUM::Val1));
    EXPECT_TRUE(e1.Test(__ENUM::Val2));
    EXPECT_FALSE(e1.Test(__ENUM::Val3));
    EXPECT_FALSE(e1.Test(__ENUM::All));
    EXPECT_NE(e1, e2);

    e1.Set(__ENUM::Val3);
    e2.Set(__ENUM::Val2);

    EXPECT_FALSE(e1.Test(__ENUM::Val1));
    EXPECT_TRUE(e1.Test(__ENUM::Val2));
    EXPECT_TRUE(e1.Test(__ENUM::Val3));
    EXPECT_FALSE(e1.Test(__ENUM::All));
    EXPECT_EQ(e1, e2);

    e1 = eAll;
    EXPECT_EQ(e1, eAll);
    EXPECT_NE(e2, eAll);
}