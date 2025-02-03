#include <gtest/gtest.h>
#include "UUID.h"

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
        const NativeUUID native = uuid1;
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
}