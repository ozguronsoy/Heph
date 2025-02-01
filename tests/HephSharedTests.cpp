#include <gtest/gtest.h>
#include "HephShared.h"

TEST(HephTest, Shared_Version)
{
    EXPECT_FALSE(HEPH_VERSION_MAJOR == 0 && HEPH_VERSION_MINOR == 0 && HEPH_VERSION_PATCH == 0) << "invalid version.";

    EXPECT_EQ(Heph::version.major, HEPH_VERSION_MAJOR) << "run-time version does not match the compile-time version."; 
    EXPECT_EQ(Heph::version.minor, HEPH_VERSION_MINOR) << "run-time version does not match the compile-time version."; 
    EXPECT_EQ(Heph::version.patch, HEPH_VERSION_PATCH) << "run-time version does not match the compile-time version."; 
}