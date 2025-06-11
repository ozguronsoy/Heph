#include <gtest/gtest.h>
#include "ArithmeticBuffer.h"
#include "RealBuffer.h"

using namespace Heph;
using test_data_t = double;
template<size_t NDims>
using ArithmeticTestBuffer = RealBuffer<test_data_t, NDims>;

TEST(HephTest, ArithmeticBuffer_MinMax)
{
    {
        ArithmeticTestBuffer<1> b = { 1, -65, 27, 31, 18, 3 };
        EXPECT_EQ(b.Min(), -65);
        EXPECT_EQ(b.Max(), 31);
        EXPECT_EQ(b.AbsMax(), 65);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, -65}, {27, 31}, {18, 3} };
        EXPECT_EQ(b.Min(), -65);
        EXPECT_EQ(b.Max(), 31);
        EXPECT_EQ(b.AbsMax(), 65);
    }
}

TEST(HephTest, ArithmeticBuffer_Rms)
{
    {
        ArithmeticTestBuffer<1> b = { 1, -65, 27, 31, 18, 3 };
        EXPECT_NEAR(b.Rms(), 32.27, 0.005);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, -65}, {27, 31}, {18, 3} };
        EXPECT_NEAR(b.Rms(), 32.27, 0.005);
    }
}

TEST(HephTest, ArithmeticBuffer_Invert)
{
    {
        constexpr test_data_t expected[6] = { -1, 65, -27, -31, -18, 3 };
        ArithmeticTestBuffer<1> b = { 1, -65, 27, 31, 18, -3 };

        b.Invert();
        for (size_t i = 0; i < b.ElementCount(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        constexpr test_data_t expected[3][2] = { {-1, 65}, {-27, -31}, {-18, 3} };
        ArithmeticTestBuffer<2> b = { {1, -65}, {27, 31}, {18, -3} };

        b.Invert();
        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }
}

TEST(HephTest, ArithmeticBuffer_Operators_Element)
{
    {
        ArithmeticTestBuffer<1> b = { 1, 2, 3, 4, 5 };

        b += 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) + 10.0);
    }

    {
        ArithmeticTestBuffer<1> b = { 1, 2, 3, 4, 5 };

        b -= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) - 10.0);
    }

    {
        ArithmeticTestBuffer<1> b = { 1, 2, 3, 4, 5 };

        b *= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) * 10.0);
    }

    {
        ArithmeticTestBuffer<1> b = { 1, 2, 3, 4, 5 };

        b /= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) / 10.0);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, 2}, {3, 4}, {5, 6} };

        b += 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) + 10.0);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, 2}, {3, 4}, {5, 6} };

        b -= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) - 10.0);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, 2}, {3, 4}, {5, 6} };

        b *= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) * 10.0);
    }

    {
        ArithmeticTestBuffer<2> b = { {1, 2}, {3, 4}, {5, 6} };

        b /= 10;

        int i = 0;
        for (const auto& element : b)
            EXPECT_EQ(element, (++i) / 10.0);
    }
}

TEST(HephTest, ArithmeticBuffer_Operators_Buffer)
{
    {
        constexpr test_data_t expected[4] = { 6, 8, 10, 12 };
        ArithmeticTestBuffer<1> b1 = { 1, 2, 3, 4 };
        ArithmeticTestBuffer<1> b2 = { 5, 6, 7, 8 };
        ArithmeticTestBuffer<1> b3;

        b1 += b2;
        EXPECT_THROW(b1 += b3, InvalidOperationException);

        for (size_t i = 0; i < b1.ElementCount(); ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        constexpr test_data_t expected[4] = { -4, -5, -6, -7 };
        ArithmeticTestBuffer<1> b1 = { 1, 2, 3, 4 };
        ArithmeticTestBuffer<1> b2 = { 5, 7, 9, 11 };
        ArithmeticTestBuffer<1> b3;

        b1 -= b2;
        EXPECT_THROW(b1 -= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.ElementCount(); ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        constexpr test_data_t expected[4] = { 5, 12, 21, 32 };
        ArithmeticTestBuffer<1> b1 = { 1, 2, 3, 4 };
        ArithmeticTestBuffer<1> b2 = { 5, 6, 7, 8 };
        ArithmeticTestBuffer<1> b3;

        b1 *= b2;
        EXPECT_THROW(b1 *= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.ElementCount(); ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        constexpr test_data_t expected[4] = { 1 / 5.0, 2 / 6.0, 3 / 7.0, 4 / 8.0 };
        ArithmeticTestBuffer<1> b1 = { 1, 2, 3, 4 };
        ArithmeticTestBuffer<1> b2 = { 5, 6, 7, 8 };
        ArithmeticTestBuffer<1> b3;

        b1 /= b2;
        EXPECT_THROW(b1 /= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.ElementCount(); ++i)
            EXPECT_NEAR(b1[i], expected[i], 0.005);
    }

    {
        constexpr test_data_t expected[4][2] = { {10, 12}, {14, 16}, {18, 20}, {22, 24} };
        ArithmeticTestBuffer<2> b1 = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
        ArithmeticTestBuffer<2> b2 = { {9, 10}, {11, 12}, {13, 14}, {15, 16} };
        ArithmeticTestBuffer<2> b3;

        b1 += b2;
        EXPECT_THROW(b1 += b3, InvalidOperationException);

        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected[i][j]);
    }

    {
        constexpr test_data_t expected[4][2] = { {-8, -9}, {-10, -11}, {-12, -13}, {-14, -15} };
        ArithmeticTestBuffer<2> b1 = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
        ArithmeticTestBuffer<2> b2 = { {9, 11}, {13, 15}, {17, 19}, {21, 23} };
        ArithmeticTestBuffer<2> b3;

        b1 -= b2;
        EXPECT_THROW(b1 -= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected[i][j]);
    }

    {
        constexpr test_data_t expected[4][2] = { {9, 20}, {33, 48}, {65, 84}, {105, 128} };
        ArithmeticTestBuffer<2> b1 = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
        ArithmeticTestBuffer<2> b2 = { {9, 10}, {11, 12}, {13, 14}, {15, 16} };
        ArithmeticTestBuffer<2> b3;

        b1 *= b2;
        EXPECT_THROW(b1 *= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected[i][j]);
    }

    {
        constexpr test_data_t expected[4][2] = { {1 / 9.0, 2 / 10.0}, {3 / 11.0, 4 / 12.0}, {5 / 13.0, 6 / 14.0}, {7 / 15.0, 8 / 16.0} };
        ArithmeticTestBuffer<2> b1 = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
        ArithmeticTestBuffer<2> b2 = { {9, 10}, {11, 12}, {13, 14}, {15, 16} };
        ArithmeticTestBuffer<2> b3;

        b1 /= b2;
        EXPECT_THROW(b1 /= b3, InvalidOperationException);

        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_NEAR((b1[i, j]), expected[i][j], 0.005);
    }
}