#include <gtest/gtest.h>
#include "Buffer.h"

using namespace Heph;
using test_data_t = int;

template<size_t NDimensions>
    requires (NDimensions > 0 && NDimensions <= 2)
class TestBuffer : public Buffer<test_data_t, NDimensions>
{
    using Base = Buffer<test_data_t, NDimensions>;
    using typename Base::iterator;
    using typename Base::const_iterator;
    using typename Base::buffer_size_t;
    using initializer_list = std::conditional_t<NDimensions == 1, std::initializer_list<test_data_t>, std::initializer_list<std::initializer_list<test_data_t>>>;

public:
    TestBuffer() : Base() {}

    template<typename... Args>
    TestBuffer(BufferFlags flags = BufferFlags::None, Args... args) : Base(flags, std::forward<Args>(args)...) {}

    TestBuffer(const initializer_list& rhs) : Base()
    {
        *this = rhs;
    }

    TestBuffer(const TestBuffer& rhs) : Base(rhs) {}

    TestBuffer(TestBuffer&& rhs) noexcept : Base(std::move(rhs)) {}

    constexpr TestBuffer& operator=(const initializer_list& rhs)
    {
        this->Release();

        if (rhs.size() > 0)
        {
            if constexpr (NDimensions == 1)
            {
                this->pData = Base::Allocate(rhs.size() * sizeof(test_data_t), BufferFlags::AllocUninitialized);
                (void)std::copy(rhs.begin(), rhs.end(), this->begin());
                this->bufferSize = rhs.size();
            }
            else
            {
                const size_t s1 = rhs.begin()->size();
                if (s1 == 0) return *this;

                this->bufferSize[0] = rhs.size();
                this->bufferSize[1] = s1;

                this->CalcStrides();

                this->pData = Base::Allocate(this->ElementCount() * sizeof(test_data_t), BufferFlags::AllocUninitialized);
                iterator it = this->begin();
                for (const auto& il : rhs)
                {
                    if (il.size() != s1)
                    {
                        HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Invalid initializer list.");
                    }

                    (void)std::copy(il.begin(), il.end(), it);
                    it += { 0, s1 };
                }
            }
        }

        return *this;
    }

    TestBuffer& operator=(const TestBuffer& rhs)
    {
        return reinterpret_cast<TestBuffer&>(Base::operator=(rhs));
    }

    TestBuffer& operator=(TestBuffer&& rhs) noexcept
    {
        return reinterpret_cast<TestBuffer&>(Base::operator=(std::move(rhs)));
    }

    void Transpose(auto... perm)
    {
        static_assert(sizeof...(perm) == NDimensions, "Invalid number of perm parameters.");
        static_assert((std::is_convertible_v<decltype(perm), size_t> && ...), "Invalid type for perm parameters, must be convertible to size_t.");

        const buffer_size_t permArray = { std::forward<size_t>(static_cast<size_t>(perm))... };
        this->Transpose(permArray);
    }

    void Transpose(const buffer_size_t& perm)
    {
        Base::Transpose(*this, *this, perm);
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
        TestBuffer<1> b1(BufferFlags::None, 5);
        TestBuffer<2> b2(BufferFlags::None, 3, 2);
        const TestBuffer<1> cb1(BufferFlags::None, 5);
        const TestBuffer<2> cb2(BufferFlags::None, 3, 2);

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

    {
        TestBuffer<1> b1(BufferFlags::Circular, 5);
        TestBuffer<2> b2(BufferFlags::Circular, 3, 2);
        const TestBuffer<1> cb1(BufferFlags::Circular, 5);
        const TestBuffer<2> cb2(BufferFlags::Circular, 3, 2);

        for (const test_data_t& element : b1)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : b2)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : cb1)
            EXPECT_EQ(element, 0);

        for (const test_data_t& element : cb2)
            EXPECT_EQ(element, 0);
    }

    {
        constexpr std::array<test_data_t, 5> b1_expected = { 1, 2, 3, 4, 5 };
        TestBuffer<1> b1 = { 1, 2, 3, 4, 5 };
        TestBuffer<2> b2 = { {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12} };

        EXPECT_EQ(b1.Size(), 5);
        EXPECT_EQ(b2.Size(0), 3);
        EXPECT_EQ(b2.Size(1), 4);

        for (size_t i = 0; i < b1_expected.size(); ++i)
            EXPECT_EQ(b1[i], b1_expected[i]);

        for (size_t i = 0, k = 1; i < b2.Size(0); ++i)
            for (size_t j = 0; j < b2.Size(1); ++j, ++k)
                EXPECT_EQ((b2[i, j]), k);

        b1.SetFlags(BufferFlags::Circular);
        b2.SetFlags(BufferFlags::Circular);

        EXPECT_EQ(b1[b1.Size() + 2], b1[2]);
        EXPECT_EQ(b1[b1.Size() + 3], b1[3]);

        EXPECT_EQ((b2[0, b2.Size(1) + 2]), (b2[0, 2]));
        EXPECT_EQ((b2[0, b2.Size(1) + 3]), (b2[0, 3]));

        EXPECT_EQ((b2[b2.Size(0) + 1, 0]), (b2[1, 0]));
        EXPECT_EQ((b2[b2.Size(0) + 2, 0]), (b2[2, 0]));
    }
}

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

TEST(HephTest, Buffer_Transpose)
{
    {
        TestBuffer<1> b1;
        TestBuffer<2> b2;

        EXPECT_THROW(b1.Transpose(0), InvalidOperationException);
        EXPECT_THROW(b2.Transpose(0, 2), InvalidArgumentException);
        EXPECT_THROW(b2.Transpose(1, 1), InvalidArgumentException);
    }

    {
        test_data_t expected[3][2] = { {1, 4}, {2, 5}, {3, 6} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6} };

        b.SetFlags(BufferFlags::TransposeInPlace);
        b.Transpose(1, 0);

        EXPECT_EQ(b.Size(0), 3);
        EXPECT_EQ(b.Size(1), 2);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
    }

    {
        test_data_t expected[3][2] = { {1, 4}, {2, 5}, {3, 6} };
        TestBuffer<2> b = { {1, 2, 3}, {4, 5, 6} };

        b.Transpose(1, 0);

        EXPECT_EQ(b.Size(0), 3);
        EXPECT_EQ(b.Size(1), 2);

        for (size_t i = 0; i < b.Size(0); ++i)
            for (size_t j = 0; j < b.Size(1); ++j)
                EXPECT_EQ((b[i, j]), expected[i][j]);
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