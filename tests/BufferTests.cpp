#include <gtest/gtest.h>
#include "Heph/Buffers/Buffer.h"

using namespace Heph;
using test_data_t = int;

template<size_t NDimensions>
    requires (NDimensions > 0 && NDimensions <= 2)
class TestBuffer : public Buffer<test_data_t, NDimensions>
{
public:
    using Base = Buffer<test_data_t, NDimensions>;
    using typename Base::iterator;
    using typename Base::const_iterator;
    using typename Base::buffer_size_t;

    using Base::Base;
    using Base::operator=;

public:
    template<typename... Args>
    TestBuffer(Args... args) : Base(std::forward<Args>(args)...) {}

    TestBuffer& operator<<=(size_t n)
    {
        Base::ShiftLeft(*this, n);
        return *this;
    }

    TestBuffer& operator>>=(size_t n)
    {
        Base::ShiftRight(*this, n);
        return *this;
    }

    TestBuffer SubBuffer(size_t index, size_t size)
    {
        TestBuffer result;
        Base::SubBuffer(*this, result, index, size);
        return result;
    }

    void Prepend(const TestBuffer& b)
    {
        Base::Prepend(*this, b);
    }

    void Append(const TestBuffer& b)
    {
        Base::Append(*this, b);
    }

    void Insert(const TestBuffer& b, size_t index)
    {
        Base::Insert(*this, b, index);
    }

    void Cut(size_t index, size_t size)
    {
        Base::Cut(*this, index, size);
    }

    void Replace(const TestBuffer& b, size_t index, size_t bIndex, size_t size)
    {
        Base::Replace(*this, b, index, bIndex, size);
    }

    void Transpose(Enum<TransposeMode> mode, auto... perm)
    {
        static_assert(sizeof...(perm) == NDimensions, "Invalid number of perm parameters.");
        static_assert((std::is_convertible_v<decltype(perm), size_t> && ...), "Invalid type for perm parameters, must be convertible to size_t.");

        const buffer_size_t permArray = { std::forward<size_t>(static_cast<size_t>(perm))... };
        this->Transpose(mode, permArray);
    }

    void Transpose(Enum<TransposeMode> mode, const buffer_size_t& perm)
    {
        Base::Transpose(*this, *this, perm, mode);
    }

    void Resize(auto... newSize)
    {
        static_assert(sizeof...(newSize) > 0 && sizeof...(newSize) <= NDimensions, "Invalid number of newSize parametrs");
        static_assert((std::is_convertible_v<decltype(newSize), size_t> && ...), "Invalid type for newSize parameters, must be convertible to size_t.");

        const buffer_size_t ns = { std::forward<size_t>(static_cast<size_t>(newSize))... };
        Base::Resize(*this, ns);
    }

    void Reverse(size_t dim)
    {
        Base::Reverse(*this, dim);
    }
};

TEST(HephTest, Buffer_Constructors)
{
    {
        constexpr std::array<size_t, 2> expected_b2Size = { 0, 0 };
        TestBuffer<1> b1;
        TestBuffer<2> b2;

        EXPECT_THROW(b1.Size(1), InvalidArgumentException);
        EXPECT_THROW(b2.Size(2), InvalidArgumentException);

        EXPECT_EQ(b1.ElementCount(), 0);
        EXPECT_EQ(b1.Size(), 0);
        EXPECT_EQ(b1.Size(0), 0);

        EXPECT_EQ(b2.ElementCount(), 0);
        EXPECT_EQ(b2.Size(), expected_b2Size);
        EXPECT_EQ(b2.Size(0), 0);
        EXPECT_EQ(b2.Size(1), 0);
    }

    {
        constexpr std::array<size_t, 2> expected_b2Size = { 3, 2 };
        TestBuffer<1> b1(5);
        TestBuffer<2> b2(3, 2);
        const TestBuffer<1> cb1(5);
        const TestBuffer<2> cb2(3, 2);

        EXPECT_EQ(b1.ElementCount(), 5);
        EXPECT_EQ(b1.Size(), 5);
        EXPECT_EQ(b1.Size(0), 5);

        EXPECT_EQ(b2.ElementCount(), 6);
        EXPECT_EQ(b2.Size(), expected_b2Size);
        EXPECT_EQ(b2.Size(0), 3);
        EXPECT_EQ(b2.Size(1), 2);

        for (const test_data_t& element : b1)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : b2)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : cb1)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : cb2)
            EXPECT_EQ(element, 0);
    }

    // {
    //     TestBuffer<1> b1(BufferFlags::Circular, 5);
    //     TestBuffer<2> b2(BufferFlags::Circular, 3, 2);
    //     const TestBuffer<1> cb1(BufferFlags::Circular, 5);
    //     const TestBuffer<2> cb2(BufferFlags::Circular, 3, 2);

    //     for (const test_data_t& element : b1)
    //         EXPECT_EQ(element, 0);

    //     for (const test_data_t& element : b2)
    //         EXPECT_EQ(element, 0);

    //     for (const test_data_t& element : cb1)
    //         EXPECT_EQ(element, 0);

    //     for (const test_data_t& element : cb2)
    //         EXPECT_EQ(element, 0);
    // }

    // {
    //     TestBuffer<1> b1 = { 1, 2, 3, 4, 5 };
    //     TestBuffer<2> b2 = { {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12} };

    //     EXPECT_EQ(b1.Size(), 5);
    //     EXPECT_EQ(b2.Size(0), 3);
    //     EXPECT_EQ(b2.Size(1), 4);

    //     for (size_t i = 0; i < b1.Size(); ++i)
    //         EXPECT_EQ(b1[i], (i + 1));

    //     for (size_t i = 0, k = 1; i < b2.Size(0); ++i)
    //         for (size_t j = 0; j < b2.Size(1); ++j, ++k)
    //             EXPECT_EQ((b2[i, j]), k);

    //     b1.SetFlags(BufferFlags::Circular);
    //     b2.SetFlags(BufferFlags::Circular);

    //     EXPECT_EQ(b1[b1.Size() + 2], b1[2]);
    //     EXPECT_EQ(b1[b1.Size() + 3], b1[3]);

    //     EXPECT_EQ((b2[0, b2.Size(1) + 2]), (b2[0, 2]));
    //     EXPECT_EQ((b2[0, b2.Size(1) + 3]), (b2[0, 3]));

    //     EXPECT_EQ((b2[b2.Size(0) + 1, 0]), (b2[1, 0]));
    //     EXPECT_EQ((b2[b2.Size(0) + 2, 0]), (b2[2, 0]));
    // }
}

TEST(HephTest, Buffer_Copy)
{
    TestBuffer<2> b1 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12} };
    TestBuffer<2> b2 = b1;
    EXPECT_EQ(b1, b2);
    EXPECT_NO_FATAL_FAILURE((b1[0, 0]));
    EXPECT_NO_FATAL_FAILURE(b1 = b1);
}

TEST(HephTest, Buffer_Move)
{
    TestBuffer<2> b1 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12} };
    TestBuffer<2> b2 = std::move(b1);
    EXPECT_TRUE(b1.IsEmpty());
    EXPECT_FALSE(b2.IsEmpty());
    EXPECT_THROW(b1.At(0, 0), InvalidArgumentException);

    size_t k = 1;
    for (test_data_t element : b2)
    {
        EXPECT_EQ(element, k);
        k++;
    }
}

// TEST(HephTest, Buffer_Circular)
// {
//     {
//         constexpr test_data_t expected[8] = { 1, 2, 3, 4, 1, 2, 3, 4 };
//         TestBuffer<1> b = { 1, 2, 3, 4 };

//         b.SetFlags(BufferFlags::Circular);
//         auto it = b.cend() - 1;
//         for (size_t i = 0; i < 8; ++i, --it)
//         {
//             EXPECT_EQ(b[i], expected[i]);
//             EXPECT_EQ(*it, expected[7 - i]);
//         }
//     }

//     {
//         constexpr test_data_t expected[8][2] = { {1, 2}, {3, 4}, {5, 6}, {7, 8}, {1, 2}, {3, 4}, {5, 6}, {7, 8} };
//         TestBuffer<2> b = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };

//         b.SetFlags(BufferFlags::Circular);
//         auto it = b.cend() - 1;
//         for (size_t i = 0; i < 8; ++i)
//         {
//             for (size_t j = 0; j < 2; ++j, --it)
//             {
//                 EXPECT_EQ((b[i, j]), expected[i][j]);
//                 EXPECT_EQ(*it, expected[7 - i][1 - j]);
//             }
//         }
//     }
// }

TEST(HephTest, Buffer_Reset)
{
    {
        TestBuffer<1> b = { 1, 2, 3, 4, 5 };
        b.Reset();
        for (const test_data_t& element : b)
            EXPECT_EQ(element, 0);
    }

    {
        TestBuffer<2> b = { {1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15} };
        b.Reset();
        for (const test_data_t& element : b)
            EXPECT_EQ(element, 0);
    }
}

TEST(HephTest, Buffer_ShiftLeft)
{
    {
        constexpr test_data_t expected0[5] = { 1, 2, 3, 4, 5 };
        constexpr test_data_t expected1[5] = { 4, 5, 0, 0, 0 };
        constexpr test_data_t expected2[5] = { 0, 0, 0, 0, 0 };
        const TestBuffer<1> b0 = { 1, 2, 3, 4, 5 };
        TestBuffer<1> b1 = b0;

        b1 <<= 0;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected0[i]);

        b1 = b0;
        b1 <<= 3;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected1[i]);

        b1 = b0;
        b1 <<= 100;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected2[i]);
    }

    {
        constexpr test_data_t expected0[5][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        constexpr test_data_t expected1[5][3] = { {10, 11, 12}, {13, 14, 15}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
        constexpr test_data_t expected2[5][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
        const TestBuffer<2> b0 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        TestBuffer<2> b1 = b0;

        b1 <<= 0;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected0[i][j]);

        b1 = b0;
        b1 <<= 3;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected1[i][j]);

        b1 = b0;
        b1 <<= 100;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected2[i][j]);
    }
}

TEST(HephTest, Buffer_ShiftRight)
{
    {
        constexpr test_data_t expected0[5] = { 1, 2, 3, 4, 5 };
        constexpr test_data_t expected1[5] = { 0, 0, 0, 1, 2 };
        constexpr test_data_t expected2[5] = { 0, 0, 0, 0, 0 };
        const TestBuffer<1> b0 = { 1, 2, 3, 4, 5 };
        TestBuffer<1> b1 = b0;

        b1 >>= 0;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected0[i]);

        b1 = b0;
        b1 >>= 3;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected1[i]);

        b1 = b0;
        b1 >>= 100;
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected2[i]);
    }

    {
        constexpr test_data_t expected0[5][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        constexpr test_data_t expected1[5][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 2, 3}, {4, 5, 6} };
        constexpr test_data_t expected2[5][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
        const TestBuffer<2> b0 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        TestBuffer<2> b1 = b0;

        b1 >>= 0;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected0[i][j]);

        b1 = b0;
        b1 >>= 3;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected1[i][j]);

        b1 = b0;
        b1 >>= 100;
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected2[i][j]);
    }
}

TEST(HephTest, Buffer_SubBuffer)
{
    {
        constexpr test_data_t expected0[3] = { 1, 2, 3 };
        constexpr test_data_t expected1[2] = { 3, 4 };
        constexpr test_data_t expected2[10] = { 4, 5, 0, 0, 0, 0, 0, 0, 0, 0 };
        TestBuffer<1> b = { 1, 2, 3, 4, 5 };

        TestBuffer<1> result = b.SubBuffer(0, 3);
        EXPECT_EQ(result.ElementCount(), 3);
        for (size_t i = 0; i < result.Size(); ++i)
            EXPECT_EQ(result[i], expected0[i]);

        result = b.SubBuffer(2, 2);
        EXPECT_EQ(result.ElementCount(), 2);
        for (size_t i = 0; i < result.Size(); ++i)
            EXPECT_EQ(result[i], expected1[i]);

        result = b.SubBuffer(3, 10);
        EXPECT_EQ(result.ElementCount(), 10);
        for (size_t i = 0; i < result.Size(); ++i)
            EXPECT_EQ(result[i], expected2[i]);
    }

    {
        constexpr test_data_t expected0[2][3] = { { 1, 2, 3 }, { 4, 5, 6 } };
        constexpr test_data_t expected1[2][3] = { { 7, 8, 9 }, { 10, 11, 12 } };
        constexpr test_data_t expected2[10][3] = { {10, 11, 12}, {13, 14, 15}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };

        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };

        TestBuffer<2> result = b.SubBuffer(0, 2);
        EXPECT_EQ(result.Size(0), 2);
        EXPECT_EQ(result.Size(1), 3);
        EXPECT_EQ(result.ElementCount(), 6);
        for (size_t i = 0; i < result.Size(0); ++i)
            for (size_t j = 0; j < result.Size(1); ++j)
                EXPECT_EQ((result[i, j]), expected0[i][j]);

        result = b.SubBuffer(2, 2);
        EXPECT_EQ(result.ElementCount(), 6);
        for (size_t i = 0; i < result.Size(0); ++i)
            for (size_t j = 0; j < result.Size(1); ++j)
                EXPECT_EQ((result[i, j]), expected1[i][j]);

        result = b.SubBuffer(3, 10);
        EXPECT_EQ(result.ElementCount(), 30);
        for (size_t i = 0; i < result.Size(0); ++i)
            for (size_t j = 0; j < result.Size(1); ++j)
                EXPECT_EQ((result[i, j]), expected2[i][j]);
    }
}

TEST(HephTest, Buffer_Prepend)
{
    {
        constexpr test_data_t expected[10] = { 8, 9, 10, 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Prepend(b2);
        EXPECT_EQ(b1.Size(), 10);

        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2;

        b1.Prepend(b2);
        EXPECT_EQ(b1.Size(), 7);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 1);
    }

    {
        TestBuffer<1> b1;
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Prepend(b2);
        EXPECT_EQ(b1.Size(), 3);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 8);
    }

    {
        constexpr test_data_t expected[6] = { 1, 2, 3, 1, 2, 3 };
        TestBuffer<1> b = { 1, 2, 3 };

        EXPECT_NO_THROW(b.Prepend(b));
        EXPECT_EQ(b.Size(), 6);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer<1> b;
        EXPECT_NO_THROW(b.Prepend(b));
        EXPECT_TRUE(b.IsEmpty());
    }

    {
        constexpr test_data_t expected[10][3] = { {22, 23, 24}, {25, 26, 27}, {28, 29, 30}, {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, {19, 20, 21} };
        TestBuffer<2> b1 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, {19, 20, 21} };
        TestBuffer<2> b2 = { {22, 23, 24}, {25, 26, 27}, {28, 29, 30} };
        const size_t expected_element_count = b1.ElementCount() + b2.ElementCount();

        b1.Prepend(b2);
        EXPECT_EQ(b1.Size(0), 10);
        EXPECT_EQ(b1.Size(1), 3);
        EXPECT_EQ(b1.ElementCount(), expected_element_count);
        for (size_t i = 0, k = 1; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b2.Size(1); ++j, ++k)
                EXPECT_EQ((b1[i, j]), expected[i][j]);
    }
}

TEST(HephTest, Buffer_Append)
{
    {
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Append(b2);
        EXPECT_EQ(b1.Size(), 10);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 1);
    }

    {
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2;

        b1.Append(b2);
        EXPECT_EQ(b1.Size(), 7);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 1);
    }

    {
        TestBuffer<1> b1;
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Append(b2);
        EXPECT_EQ(b1.Size(), 3);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 8);
    }

    {
        constexpr test_data_t expected[6] = { 1, 2, 3, 1, 2, 3 };
        TestBuffer<1> b = { 1, 2, 3 };

        EXPECT_NO_THROW(b.Append(b));
        EXPECT_EQ(b.Size(), 6);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer<1> b;
        EXPECT_NO_THROW(b.Append(b));
        EXPECT_TRUE(b.IsEmpty());
    }

    {
        TestBuffer<2> b1 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, {19, 20, 21} };
        TestBuffer<2> b2 = { {22, 23, 24}, {25, 26, 27}, {28, 29, 30} };
        const size_t expected_element_count = b1.ElementCount() + b2.ElementCount();

        b1.Append(b2);
        EXPECT_EQ(b1.Size(0), 10);
        EXPECT_EQ(b1.Size(1), 3);
        EXPECT_EQ(b1.ElementCount(), expected_element_count);
        for (size_t i = 0, k = 1; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b2.Size(1); ++j, ++k)
                EXPECT_EQ((b1[i, j]), k);
    }
}

TEST(HephTest, Buffer_Insert)
{
    {
        constexpr test_data_t expected[10] = { 1, 2, 3, 8, 9, 10, 4, 5, 6, 7 };
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Insert(b2, 3);
        EXPECT_EQ(b1.Size(), 10);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b2;

        b1.Insert(b2, 5);
        EXPECT_EQ(b1.Size(), 7);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 1);
    }

    {
        TestBuffer<1> b1;
        TestBuffer<1> b2 = { 8, 9, 10 };

        b1.Insert(b2, 0);
        EXPECT_EQ(b1.Size(), 3);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], i + 8);
    }

    {
        constexpr test_data_t expected[6] = { 1, 2, 1, 2, 3, 3 };
        TestBuffer<1> b = { 1, 2, 3 };

        EXPECT_NO_THROW(b.Insert(b, 2));
        EXPECT_EQ(b.Size(), 6);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer<1> b;
        EXPECT_NO_THROW(b.Insert(b, 0));
        EXPECT_TRUE(b.IsEmpty());
        EXPECT_THROW(b.Insert(b, 1), InvalidArgumentException);
    }

    {
        constexpr test_data_t expected[10][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {22, 23, 24}, {25, 26, 27}, {28, 29, 30}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, {19, 20, 21} };
        TestBuffer<2> b1 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, {19, 20, 21} };
        TestBuffer<2> b2 = { {22, 23, 24}, {25, 26, 27}, {28, 29, 30} };
        const size_t expected_element_count = b1.ElementCount() + b2.ElementCount();

        b1.Insert(b2, 3);
        EXPECT_EQ(b1.Size(0), 10);
        EXPECT_EQ(b1.Size(1), 3);
        EXPECT_EQ(b1.ElementCount(), expected_element_count);
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b2.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected[i][j]);
    }
}

TEST(HephTest, Buffer_Cut)
{
    {
        constexpr test_data_t expected[4] = { 1, 2, 6, 7 };
        TestBuffer<1> b = { 1, 2, 3, 4, 5, 6, 7 };

        b.Cut(2, 3);
        EXPECT_EQ(b.ElementCount(), 4);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        constexpr test_data_t expected[4] = { 5, 6, 7 };
        TestBuffer<1> b = { 1, 2, 3, 4, 5, 6, 7 };

        b.Cut(0, 4);
        EXPECT_EQ(b.ElementCount(), 3);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        constexpr test_data_t expected[4] = { 1, 2, 3, 4 };
        TestBuffer<1> b = { 1, 2, 3, 4, 5, 6, 7 };

        b.Cut(4, 10);
        EXPECT_EQ(b.ElementCount(), 4);
        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer<1> b = { 1, 2, 3, 4, 5, 6, 7 };

        EXPECT_THROW(b.Cut(10, 3), InvalidArgumentException);

        b.Cut(0, 100);
        EXPECT_EQ(b.Size(), 0);
        EXPECT_TRUE(b.IsEmpty());
    }

    {
        constexpr test_data_t expected[2][3] = { {1, 2, 3}, {13, 14, 15} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };

        b.Cut(1, 3);
        EXPECT_EQ(b.Size(0), 2);
        EXPECT_EQ(b.Size(1), 3);
        EXPECT_EQ(b.ElementCount(), 6);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }

    {
        constexpr test_data_t expected[1][3] = { {13, 14, 15} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };

        b.Cut(0, 4);
        EXPECT_EQ(b.Size(0), 1);
        EXPECT_EQ(b.Size(1), 3);
        EXPECT_EQ(b.ElementCount(), 3);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }

    {
        constexpr test_data_t expected[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };

        b.Cut(3, 10);
        EXPECT_EQ(b.Size(0), 3);
        EXPECT_EQ(b.Size(1), 3);
        EXPECT_EQ(b.ElementCount(), 9);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }

    {
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };

        EXPECT_THROW(b.Cut(10, 2), InvalidArgumentException);
        b.Cut(0, 100);
        EXPECT_EQ(b.ElementCount(), 0);
        EXPECT_TRUE(b.IsEmpty());
    }
}

TEST(HephTest, Buffer_Replace)
{
    {
        constexpr test_data_t expected0[7] = { 1, 2, 3, 4, 8, 9, 10 };
        constexpr test_data_t expected1[7] = { 1, 9, 10, 4, 5, 6, 7 };
        constexpr test_data_t expected2[7] = { 3, 4, 5, 6, 5, 6, 7 };
        constexpr test_data_t expected3[7] = { 1, 2, 1, 2, 3, 4, 7 };
        TestBuffer<1> b0 = { 1, 2, 3, 4, 5, 6, 7 };
        TestBuffer<1> b1 = b0;
        TestBuffer<1> b2 = { 8, 9, 10 };

        EXPECT_THROW(b1.Replace(b2, 0, 0, 4), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 7, 0, 3), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 0, 3, 0), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 0, 1, 3), InvalidArgumentException);

        b1.Replace(b2, 4, 0, 3);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected0[i]);
        b1 = b0;

        b1.Replace(b2, 1, 1, 2);
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected1[i]);

        b1 = b0;
        EXPECT_NO_THROW(b1.Replace(b1, 0, 2, 4));
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected2[i]);

        b1 = b0;
        EXPECT_NO_THROW(b1.Replace(b1, 2, 0, 4));
        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], expected3[i]);
    }

    {
        constexpr test_data_t expected0[5][3] = { {1, 2, 3}, {4, 5, 6}, {16, 17, 18}, {19, 20, 21}, {22, 23, 24} };
        constexpr test_data_t expected1[5][3] = { {19, 20, 21}, {22, 23, 24}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        TestBuffer<2> b0 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15} };
        TestBuffer<2> b1 = b0;
        TestBuffer<2> b2 = { {16, 17, 18}, {19, 20, 21}, {22, 23, 24} };

        EXPECT_THROW(b1.Replace(b2, 0, 0, 4), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 5, 0, 3), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 0, 3, 0), InvalidArgumentException);
        EXPECT_THROW(b1.Replace(b2, 0, 1, 3), InvalidArgumentException);

        b1.Replace(b2, 2, 0, 3);
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected0[i][j]);
        b1 = b0;

        b1.Replace(b2, 0, 1, 2);
        for (size_t i = 0; i < b1.Size(0); ++i)
            for (size_t j = 0; j < b1.Size(1); ++j)
                EXPECT_EQ((b1[i, j]), expected1[i][j]);
        b1 = b0;
    }
}

TEST(HephTest, Buffer_Transpose)
{
    {
        TestBuffer<1> b1;
        TestBuffer<2> b2;

        EXPECT_THROW(b1.Transpose(TransposeMode::Normal, 0), InvalidOperationException);
        EXPECT_THROW(b2.Transpose(TransposeMode::Normal, 0, 2), InvalidArgumentException);
        EXPECT_THROW(b2.Transpose(TransposeMode::Normal, 1, 1), InvalidArgumentException);
    }

    {
        test_data_t expected[3][2] = { {1, 4}, {2, 5}, {3, 6} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6} };

        b.Transpose(TransposeMode::InPlace, 1, 0);

        EXPECT_EQ(b.Size(0), 3);
        EXPECT_EQ(b.Size(1), 2);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }

    {
        test_data_t expected[3][2] = { {1, 4}, {2, 5}, {3, 6} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6} };

        b.Transpose(TransposeMode::Normal, 1, 0);

        EXPECT_EQ(b.Size(0), 3);
        EXPECT_EQ(b.Size(1), 2);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }
}

TEST(HephTest, Buffer_Resize)
{
    {
        constexpr test_data_t expected0[8] = { 1, 2, 3, 4, 5, 0, 0, 0 };

        TestBuffer<1> b = { 1, 2, 3, 4, 5 };

        b.Resize(8);
        EXPECT_EQ(b.Size(), 8);

        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected0[i]);

        b.Resize(3);
        EXPECT_EQ(b.Size(), 3);
        EXPECT_TRUE(b[0] == 1 && b[1] == 2 && b[2] == 3);

        b.Resize(0);
        EXPECT_EQ(b.Size(), 0);
        EXPECT_EQ(b.ElementCount(), 0);
        EXPECT_TRUE(b.IsEmpty());
    }

    {
        constexpr TestBuffer<2>::buffer_size_t expected_size0 = { 2, 7 };
        constexpr TestBuffer<2>::buffer_size_t expected_size1 = { 4, 3 };
        constexpr TestBuffer<2>::buffer_size_t expected_size2 = { 0, 0 };
        constexpr test_data_t expected0[2][7] = { {1, 2, 3, 4, 5, 0, 0}, {6, 7, 8, 9, 10, 0, 0} };
        constexpr test_data_t expected1[4][3] = { {1, 2, 3}, { 6, 7, 8 }, {0, 0, 0}, {0, 0, 0} };

        TestBuffer<2> b = { {1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15} };

        b.Resize(2, 7);
        EXPECT_EQ(b.Size(), expected_size0);
        EXPECT_EQ(b.ElementCount(), 14);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected0[i][j]);

        b.Resize(4, 3);
        EXPECT_EQ(b.Size(), expected_size1);
        EXPECT_EQ(b.ElementCount(), 12);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected1[i][j]);

        b.Resize(2, 0);
        EXPECT_EQ(b.Size(), expected_size2);
        EXPECT_EQ(b.ElementCount(), 0);
        EXPECT_TRUE(b.IsEmpty());
    }
}

TEST(HephTest, Buffer_Reverse)
{
    {
        constexpr std::array<test_data_t, 5> expected = { 5, 4, 3, 2, 1 };
        TestBuffer<1> b = { 1, 2, 3, 4, 5 };

        EXPECT_THROW(b.Reverse(1), InvalidArgumentException);
        b.Reverse(0);

        for (size_t i = 0; i < b.Size(); ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        constexpr test_data_t expected0[2][4] = { {5, 6, 7, 8}, {1, 2, 3, 4} };
        constexpr test_data_t expected1[2][4] = { {8, 7, 6, 5}, {4, 3, 2, 1} };
        TestBuffer<2> b = { {1, 2, 3, 4}, {5, 6, 7, 8} };

        EXPECT_EQ(b.Size(0), 2);
        EXPECT_EQ(b.Size(1), 4);

        b.Reverse(0);
        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), (expected0[i][j]));

        b.Reverse(1);
        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), (expected1[i][j]));
    }
}