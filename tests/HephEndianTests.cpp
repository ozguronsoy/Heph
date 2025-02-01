#include <gtest/gtest.h>
#include "HephEndian.h"

TEST(HephTest, Endian_Swap)
{
    std::endian e = std::endian::little;
    
    EXPECT_EQ(Heph::SwapEndian<uint16_t>(0x1234, e), 0x3412);
    EXPECT_EQ(e, std::endian::big);

    EXPECT_EQ(Heph::SwapEndian<uint32_t>(0x12345678, e), 0x78563412);
    EXPECT_EQ(e, std::endian::little);

    EXPECT_EQ(Heph::SwapEndian<uint64_t>(0x123456789ABCDEF0), 0xF0DEBC9A78563412);
}