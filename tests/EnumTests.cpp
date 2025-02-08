#include <gtest/gtest.h>
#include "Enum.h"
#include <type_traits>

#define ENUM_CLASSIC (false)
#define ENUM_SCOPED (true)

template<typename __enum_int_t, bool scoped>
    requires (std::is_integral_v<__enum_int_t>)
struct EnumTest
{
    enum __CLASSIC_ENUM : __enum_int_t
    {
        None = 0,
        Val1 = 1,
        Val2 = 2,
        Val3 = 4,
        All = 7
    };

    enum class __SCOPED_ENUM : __enum_int_t
    {
        None = 0,
        Val1 = 1,
        Val2 = 2,
        Val3 = 4,
        All = 7
    };

    using __ENUM = std::conditional_t<scoped, __SCOPED_ENUM, __CLASSIC_ENUM>;

    EnumTest()
    {
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

        e1 &= __ENUM::Val2;
        EXPECT_EQ(e1, __ENUM::Val2);

        EXPECT_EQ(~e1, static_cast<__ENUM>(~static_cast<__enum_int_t>(__ENUM::Val2)));
    }
};

TEST(HephTest, Enum)
{
    EnumTest<int8_t, ENUM_CLASSIC>();
    EnumTest<int8_t, ENUM_SCOPED>();
    EnumTest<uint8_t, ENUM_CLASSIC>();
    EnumTest<uint8_t, ENUM_SCOPED>();

    EnumTest<int16_t, ENUM_CLASSIC>();
    EnumTest<int16_t, ENUM_SCOPED>();
    EnumTest<uint16_t, ENUM_CLASSIC>();
    EnumTest<uint16_t, ENUM_SCOPED>();

    EnumTest<int32_t, ENUM_CLASSIC>();
    EnumTest<int32_t, ENUM_SCOPED>();
    EnumTest<uint32_t, ENUM_CLASSIC>();
    EnumTest<uint32_t, ENUM_SCOPED>();

    EnumTest<int64_t, ENUM_CLASSIC>();
    EnumTest<int64_t, ENUM_SCOPED>();
    EnumTest<uint64_t, ENUM_CLASSIC>();
    EnumTest<uint64_t, ENUM_SCOPED>();
}