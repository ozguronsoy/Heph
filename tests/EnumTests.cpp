#include <gtest/gtest.h>
#include "Enum.h"
#include <type_traits>

#define ENUM_CLASSIC (0)
#define ENUM_SCOPED (1)
#define ENUM_INTEGRAL (2)

template<typename __enum_int_t, int __enum_type>
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

    using __ENUM = std::conditional_t<__enum_type == ENUM_SCOPED, __SCOPED_ENUM, std::conditional_t<__enum_type == ENUM_CLASSIC, __CLASSIC_ENUM, __enum_int_t>>;

    EnumTest()
    {
        constexpr Heph::Enum<__ENUM> eAll = static_cast<__ENUM>(__CLASSIC_ENUM::All);
        constexpr Heph::Enum<__ENUM> eVal1 = static_cast<__ENUM>(__CLASSIC_ENUM::Val1);
        constexpr Heph::Enum<__ENUM> eVal2 = static_cast<__ENUM>(__CLASSIC_ENUM::Val2);
        constexpr Heph::Enum<__ENUM> eVal3 = static_cast<__ENUM>(__CLASSIC_ENUM::Val3);

        Heph::Enum<__ENUM> e1(eVal2);
        Heph::Enum<__ENUM> e2(eVal3);

        EXPECT_FALSE(e1.Test(eVal1));
        EXPECT_TRUE(e1.Test(eVal2));
        EXPECT_FALSE(e1.Test(eVal3));
        EXPECT_FALSE(e1.Test(eAll));
        EXPECT_NE(e1, e2);

        e1.Set(eVal3);
        e2.Set(eVal2);

        EXPECT_FALSE(e1.Test(eVal1));
        EXPECT_TRUE(e1.Test(eVal2));
        EXPECT_TRUE(e1.Test(eVal3));
        EXPECT_FALSE(e1.Test(eAll));
        EXPECT_EQ(e1, e2);

        e1 = eAll;
        EXPECT_EQ(e1, eAll);
        EXPECT_NE(e2, eAll);

        e1 &= eVal2;
        EXPECT_EQ(e1, eVal2);

        EXPECT_EQ(~e1, static_cast<__ENUM>(~static_cast<__enum_int_t>(eVal2.value)));
    }
};

TEST(HephTest, Enum)
{
    EnumTest<int8_t, ENUM_CLASSIC>();
    EnumTest<int8_t, ENUM_SCOPED>();
    EnumTest<int8_t, ENUM_INTEGRAL>();
    EnumTest<uint8_t, ENUM_CLASSIC>();
    EnumTest<uint8_t, ENUM_SCOPED>();
    EnumTest<uint8_t, ENUM_INTEGRAL>();

    EnumTest<int16_t, ENUM_CLASSIC>();
    EnumTest<int16_t, ENUM_SCOPED>();
    EnumTest<int16_t, ENUM_INTEGRAL>();
    EnumTest<uint16_t, ENUM_CLASSIC>();
    EnumTest<uint16_t, ENUM_SCOPED>();
    EnumTest<uint16_t, ENUM_INTEGRAL>();

    EnumTest<int32_t, ENUM_CLASSIC>();
    EnumTest<int32_t, ENUM_SCOPED>();
    EnumTest<int32_t, ENUM_INTEGRAL>();
    EnumTest<uint32_t, ENUM_CLASSIC>();
    EnumTest<uint32_t, ENUM_SCOPED>();
    EnumTest<uint32_t, ENUM_INTEGRAL>();

    EnumTest<int64_t, ENUM_CLASSIC>();
    EnumTest<int64_t, ENUM_SCOPED>();
    EnumTest<int64_t, ENUM_INTEGRAL>();
    EnumTest<uint64_t, ENUM_CLASSIC>();
    EnumTest<uint64_t, ENUM_SCOPED>();
    EnumTest<uint64_t, ENUM_INTEGRAL>();
}