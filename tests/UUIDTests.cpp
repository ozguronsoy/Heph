#include <gtest/gtest.h>
#include "Heph/UUID.h"
#include "Heph/Exceptions/InvalidArgumentException.h"
#include <unordered_map>

TEST(HephTest, UUID)
{
    {
        const Heph::UUID expected; 
        EXPECT_EQ(Heph::UUID(), expected);
    }

    {
        const Heph::UUID uuid1 = Heph::UUID::Create(); 
        const Heph::UUID uuid2 = Heph::UUID::Create();
        EXPECT_NE(uuid1, uuid2); 
    }

    {
        const Heph::UUID uuid1 = Heph::UUID::Create();
        const Heph::UUID::Native native = uuid1;
        Heph::UUID uuid2(native);
        
        EXPECT_EQ(uuid1, uuid2);

        Heph::UUID uuid3;
        uuid3.Generate();
        EXPECT_NE(uuid3, uuid1);
        uuid3 = uuid1;
        EXPECT_EQ(uuid3, uuid1);
        EXPECT_EQ(uuid3, uuid2);
    }

    {
        const std::string hexStr = "7CF1D2FF-BA63-4724-BD2B-459264826A95";
        const Heph::UUID uuid(hexStr);

        EXPECT_EQ(static_cast<std::string>(uuid), hexStr);
        EXPECT_EQ(Heph::UUID(static_cast<std::string>(uuid)), uuid);
    }

    {
        const std::wstring hexStr = L"7CF1D2FF-BA63-4724-BD2B-459264826A95";
        const Heph::UUID uuid(hexStr);

        EXPECT_EQ(static_cast<std::wstring>(uuid), hexStr);
        EXPECT_EQ(Heph::UUID(static_cast<std::wstring>(uuid)), uuid);
    }

    {
        const std::string invalidHexStr1 = "7CF1D2FF-BA63-4724-BD2B-459264826A9"; // invalid size
        const std::string invalidHexStr2 = "7CF1D2FF-BA63-4724-BD2B*459264826A95"; // invalid dash
        const std::string invalidHexStr3 = "7CF1D2FF-BA63-4724-BDVB-459264826A95"; // invalid char

        Heph::UUID uuid("7CF1D2FF-BA63-4724-BD2B-459264826A95");

        EXPECT_THROW(uuid = invalidHexStr1, Heph::InvalidArgumentException);
        EXPECT_THROW(uuid = invalidHexStr2, Heph::InvalidArgumentException);
        EXPECT_THROW(uuid = invalidHexStr3, Heph::InvalidArgumentException);
    }

    {
        const Heph::UUID uuid1 = Heph::UUID::Create();
        const Heph::UUID uuid2 = Heph::UUID::Create();
        std::unordered_map<Heph::UUID, int> map;

        map[uuid1] = 15;
        map[uuid2] = 30;
        EXPECT_EQ(map[uuid1], 15);
        EXPECT_EQ(map[uuid2], 30);
    }
}