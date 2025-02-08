#include <gtest/gtest.h>
#include "Buffer.h"

using namespace Heph;
using test_data_t = uint32_t;

class TestBuffer : public Buffer<TestBuffer, test_data_t>
{
public:
    TestBuffer() : Buffer() {}

    TestBuffer(size_t size, BufferFlags flags = BufferFlags::None) : Buffer(size, flags) {}

    TestBuffer(const std::initializer_list<test_data_t>& rhs) : Buffer(rhs.size())
    {
        (void)std::copy(rhs.begin(), rhs.end(), this->begin());
    }

    TestBuffer(const TestBuffer& rhs) : Buffer(rhs) {}

    TestBuffer(TestBuffer&& rhs) noexcept : Buffer(std::move(rhs)) {}

    TestBuffer& operator=(const std::initializer_list<test_data_t>& rhs)
    {
        BufferFlags tempFlags = this->flags;
        this->Release();

        if (rhs.size() > 0)
        {
            this->pData = Buffer::Allocate(rhs.size() * sizeof(test_data_t), BufferFlags::None);
            (void)std::copy(rhs.begin(), rhs.end(), this->begin());
        }

        this->size = rhs.size();
        this->flags = tempFlags;

        return *this;
    }

    TestBuffer& operator=(const TestBuffer& rhs)
    {
        return reinterpret_cast<TestBuffer&>(Buffer::operator=(rhs));
    }

    TestBuffer& operator=(TestBuffer&& rhs) noexcept
    {
        return reinterpret_cast<TestBuffer&>(Buffer::operator=(std::move(rhs)));
    }
};

TEST(HephTest, Buffer_Constructors)
{
    {
        TestBuffer b;
        EXPECT_EQ(b.Size(), 0);
        EXPECT_TRUE(b.begin() == nullptr);
    }

    {
        TestBuffer b(5);
        EXPECT_EQ(b.Size(), 5);

        for (const test_data_t& data : b)
            EXPECT_EQ(data, 0);
    }

    {
        TestBuffer b(5, BufferFlags::AllocUninitialized);
        EXPECT_EQ(b.Size(), 5);
        EXPECT_FALSE(b.begin() == nullptr);
    }

    {
        TestBuffer b = { 1, 5, 3, 4, 7 };
        test_data_t expected[5] = { 1, 5, 3, 4, 7 };

        for (size_t i = 0; i < 5; ++i)
            EXPECT_EQ(b[i], expected[i]);
    }
}

TEST(HephTest, Buffer_Copy)
{
    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2(b1);

        EXPECT_EQ(b1.Size(), b2.Size());

        for (size_t i = 0; i < b1.Size(); ++i)
            EXPECT_EQ(b1[i], b2[i]);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 6, 7, 8 };
        test_data_t expected[] = { 6, 7, 8 };
        size_t expectedSize = b2.Size();

        b1.operator=(b2);

        EXPECT_EQ(b1.Size(), expectedSize);

        for (size_t i = 0; i < expectedSize; ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2;

        b1.operator=(b2);

        EXPECT_EQ(b1.Size(), 0);
        EXPECT_TRUE(b1.begin() == nullptr);
    }

    {
        TestBuffer b1;
        TestBuffer b2 = { 1, 2, 3, 4, 5 };
        test_data_t expected[] = { 1, 2, 3, 4, 5 };
        size_t expectedSize = b2.Size();

        b1.operator=(b2);

        EXPECT_EQ(b1.Size(), expectedSize);

        for (size_t i = 0; i < expectedSize; ++i)
            EXPECT_EQ(b1[i], expected[i]);
    }

    {
        TestBuffer b = { 1, 2, 3 };
        test_data_t expected[] = { 1, 2, 3 };
        size_t expectedSize = b.Size();

        b.operator=(b);

        EXPECT_EQ(b.Size(), expectedSize);

        for (size_t i = 0; i < expectedSize; ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer b = { 1, 2, 3 };
        test_data_t expected[] = { 4, 5, 6, 7 };
        size_t expectedSize = 4;

        b.operator=({ 4, 5, 6, 7 });

        EXPECT_EQ(b.Size(), expectedSize);

        for (size_t i = 0; i < expectedSize; ++i)
            EXPECT_EQ(b[i], expected[i]);
    }

    {
        TestBuffer b = { 1, 2, 3 };

        b.operator=({});

        EXPECT_EQ(b.Size(), 0);
        EXPECT_TRUE(b.begin() == nullptr);
    }
}

TEST(HephTest, Buffer_Move)
{
    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };

        test_data_t* expectedBegin = b1.begin();
        size_t expectedSize = b1.Size();

        TestBuffer b2(std::move(b1));

        EXPECT_EQ(b1.Size(), 0);
        EXPECT_TRUE(b1.begin() == nullptr);
        EXPECT_EQ(b2.Size(), expectedSize);
        EXPECT_EQ(b2.begin(), expectedBegin);
    }

    {
        TestBuffer b1;
        TestBuffer b2(std::move(b1));

        EXPECT_EQ(b2.Size(), 0);
        EXPECT_TRUE(b2.begin() == nullptr);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 6, 7 };
        test_data_t* expectedBegin = b2.begin();
        size_t expectedSize = b2.Size();

        b1.operator=(std::move(b2));

        EXPECT_EQ(b1.Size(), expectedSize);
        EXPECT_EQ(b1.begin(), expectedBegin);
        EXPECT_EQ(b2.Size(), 0);
        EXPECT_TRUE(b2.begin() == nullptr);
    }

    {
        TestBuffer b1;
        TestBuffer b2 = { 6, 7 };
        test_data_t* expectedBegin = b2.begin();
        size_t expectedSize = b2.Size();

        b1.operator=(std::move(b2));

        EXPECT_EQ(b1.Size(), expectedSize);
        EXPECT_EQ(b1.begin(), expectedBegin);
        EXPECT_EQ(b2.Size(), 0);
        EXPECT_TRUE(b2.begin() == nullptr);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2;
        test_data_t* expectedBegin = b2.begin();
        size_t expectedSize = b2.Size();

        b1.operator=(std::move(b2));

        EXPECT_EQ(b1.Size(), 0);
        EXPECT_TRUE(b1.begin() == nullptr);
        EXPECT_EQ(b2.Size(), 0);
        EXPECT_TRUE(b2.begin() == nullptr);
    }

    {
        TestBuffer b = { 1, 2, 3 };
        test_data_t* expectedBegin = b.begin();
        size_t expectedSize = b.Size();

        b.operator=(std::move(b));

        EXPECT_EQ(b.Size(), expectedSize);
        EXPECT_EQ(b.begin(), expectedBegin);
    }
}

TEST(HephTest, Buffer_Compare)
{
    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 1, 2, 3, 4, 5 };

        EXPECT_EQ(b1, b2);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 1, 2, 3, 4, 70 };

        EXPECT_NE(b1, b2);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 1, 2, 3 };

        EXPECT_NE(b1, b2);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2 = { 1, 2, 3, 4, 5, 6, 7, 8 };

        EXPECT_NE(b1, b2);
    }

    {
        TestBuffer b1 = { 1, 2, 3, 4, 5 };
        TestBuffer b2;

        EXPECT_NE(b1, b2);
    }
}

TEST(HephTest, Buffer_SubscriptOp)
{
    TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };
    const TestBuffer cb = { 1, 2, 3, 4, 5, 6, 7 };

    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[4], 5);
    EXPECT_EQ(b[6], 7);

    EXPECT_EQ(cb[0], 1);
    EXPECT_EQ(cb[1], 2);
    EXPECT_EQ(cb[4], 5);
    EXPECT_EQ(cb[6], 7);
}

TEST(HephTest, Buffer_Size)
{
    TestBuffer b;

    EXPECT_EQ(b.Size(), 0);

    b = { 1, 2, 3, 4, 5, 6, 7, 8 };

    EXPECT_EQ(b.Size(), 8);
}

TEST(HephTest, Buffer_Flags)
{
    TestBuffer b;
    EXPECT_EQ(b.Flags(), BufferFlags::None);

    b.SetFlags(BufferFlags::AllocUninitialized);
    EXPECT_EQ(b.Flags(), BufferFlags::AllocUninitialized);

    b.SetFlags(BufferFlags::Circular);
    EXPECT_EQ(b.Flags(), BufferFlags::Circular);
    EXPECT_NE(b.Flags(), BufferFlags::AllocUninitialized | BufferFlags::Circular);

    Enum<BufferFlags> flags = b.Flags();
    flags.Set(BufferFlags::AllocUninitialized);
    b.SetFlags(flags);
    EXPECT_NE(b.Flags(), BufferFlags::Circular);
    EXPECT_NE(b.Flags(), BufferFlags::AllocUninitialized);
    EXPECT_EQ(b.Flags(), BufferFlags::AllocUninitialized | BufferFlags::Circular);
}

TEST(HephTest, Buffer_IsEmpty)
{
    TestBuffer b;

    EXPECT_TRUE(b.IsEmpty());
    EXPECT_FALSE(b);

    b = { 1, 2, 3, 4, 5 };

    EXPECT_FALSE(b.IsEmpty());
    EXPECT_TRUE(b);
}

TEST(HephTest, Buffer_At)
{
    TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };
    const TestBuffer cb = { 1, 2, 3, 4, 5, 6, 7 };

    EXPECT_EQ(b.At(0), 1);
    EXPECT_EQ(b.At(1), 2);
    EXPECT_EQ(b.At(4), 5);
    EXPECT_EQ(b.At(6), 7);

    EXPECT_EQ(cb.At(0), 1);
    EXPECT_EQ(cb.At(1), 2);
    EXPECT_EQ(cb.At(4), 5);
    EXPECT_EQ(cb.At(6), 7);

    EXPECT_THROW(b.At(b.Size()), InvalidArgumentException);
    EXPECT_THROW(cb.At(cb.Size()), InvalidArgumentException);
}

TEST(HephTest, Buffer_Reset)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	b.Reset();

	for (size_t i = 0; i < b.Size(); ++i)
		EXPECT_EQ(b[i], 0);
}

TEST(HephTest, Buffer_Resize)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5, 0, 0, 0 };
		size_t expectedSize = 8;

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3 };
		size_t expectedSize = 3;

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 1, 2, 3, 4, 5 };
		size_t expectedSize = b.Size();

		b.Resize(expectedSize);

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b = { 1, 2, 3, 4, 5 };

		b.Resize(0);

		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}
}

TEST(HephTest, Buffer_Reverse)
{
	{
		TestBuffer b = { 1, 2, 3, 4, 5 };
		test_data_t expected[] = { 5, 4, 3, 2, 1 };
		size_t expectedSize = b.Size();

		b.Reverse();

		EXPECT_EQ(b.Size(), expectedSize);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b[i], expected[i]);
	}

	{
		TestBuffer b;
		b.Reverse();
		EXPECT_EQ(b.Size(), 0);
		EXPECT_TRUE(b.begin() == nullptr);
	}
}

TEST(HephTest, Buffer_Release)
{
	TestBuffer b = { 1, 2, 3, 4, 5, 6, 7 };

	b.Release();

	EXPECT_TRUE(b.begin() == nullptr);
	EXPECT_EQ(b.Size(), 0);

	// test if actually freed?
}

TEST(HephTest, Buffer_begin)
{
	{
		TestBuffer b;
		EXPECT_TRUE(b.begin() == nullptr);
	}

	{
		TestBuffer b(1);
		EXPECT_FALSE(b.begin() == nullptr);
	}
}

TEST(HephTest, Buffer_end)
{
	{
		TestBuffer b;
		EXPECT_TRUE(b.end() == nullptr);
	}

	{
		TestBuffer b(1);
		test_data_t* expected = b.begin() + b.Size();

		EXPECT_EQ(b.end(), expected);
	}
}