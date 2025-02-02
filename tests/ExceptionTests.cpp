#include <gtest/gtest.h>
#include "Exceptions/Exceptions.h"

#define CHECK_EX_TYPE(pEx, type) { const type* _ex = dynamic_cast<const type*>(pEx); EXPECT_TRUE(_ex != nullptr); }

using namespace Heph;

class ExceptionTest : public testing::Test
{
protected:
	static constexpr const char* METHOD = "Method";
	static constexpr const char* MESSAGE = "Message";

protected:
	ExceptionTest()
	{
        Exception::ClearExceptions();
        Exception::OnException.userArgs.clear();
		Exception::OnException.ClearHandlers();
	}

	static void Handler1(const EventParams& params)
	{
		ExceptionEventArgs* pArgs = dynamic_cast<ExceptionEventArgs*>(params.pArgs);
		EXPECT_TRUE((&pArgs->exception) != nullptr);
		EXPECT_EQ(pArgs->exception.Method(), ExceptionTest::METHOD);
		EXPECT_EQ(pArgs->exception.Message(), ExceptionTest::MESSAGE);
	}

	static void Handler2(const EventParams& params)
	{
		ExceptionEventArgs* pArgs = (ExceptionEventArgs*)params.pArgs;
		CHECK_EX_TYPE(&pArgs->exception, InvalidArgumentException);
	}

	static void TestThread()
	{
		EXPECT_EQ(Exception::Exceptions().size(), 0);
		HEPH_EXCEPTION_RAISE(Exception, "", "");
		HEPH_EXCEPTION_RAISE(InvalidArgumentException, "", "");
		HEPH_EXCEPTION_RAISE(Exception, "", "");
		EXPECT_EQ(Exception::Exceptions().size(), 3);
	}
};

TEST_F(ExceptionTest, Handler)
{
	Exception::OnException = &Handler1;
	HEPH_EXCEPTION_RAISE(Exception, ExceptionTest::METHOD, ExceptionTest::MESSAGE);
}

TEST_F(ExceptionTest, Name)
{
	EXPECT_EQ(Exception("", "").Name(), "Exception");
	EXPECT_EQ(ExternalException("", "", "", "").Name(), "ExternalException");
	EXPECT_EQ(InsufficientMemoryException("", "").Name(), "InsufficientMemoryException");
	EXPECT_EQ(InvalidArgumentException("", "").Name(), "InvalidArgumentException");
	EXPECT_EQ(InvalidOperationException("", "").Name(), "InvalidOperationException");
	EXPECT_EQ(NotFoundException("", "").Name(), "NotFoundException");
	EXPECT_EQ(NotImplementedException("", "").Name(), "NotImplementedException");
	EXPECT_EQ(NotSupportedException("", "").Name(), "NotSupportedException");
	EXPECT_EQ(TimeoutException("", "").Name(), "TimeoutException");
}

TEST_F(ExceptionTest, Exceptions)
{
	// test thread_local
	EXPECT_EQ(Exception::Exceptions().size(), 0);
	HEPH_EXCEPTION_RAISE(Exception, "", "");
	EXPECT_EQ(Exception::Exceptions().size(), 1);

	std::thread t(&TestThread);
	t.join();

	EXPECT_EQ(Exception::Exceptions().size(), 1);
}

TEST_F(ExceptionTest, LastException)
{
	EXPECT_TRUE(Exception::LastException() == nullptr);

	HEPH_EXCEPTION_RAISE(Exception, "", "");
	EXPECT_TRUE(Exception::LastException() != nullptr);

	Exception::OnException = &Handler2;
	HEPH_EXCEPTION_RAISE(InvalidArgumentException, "", "");
	CHECK_EX_TYPE(Exception::LastException(), InvalidArgumentException);
}

TEST_F(ExceptionTest, ExternalException)
{
	ExternalException e("method", "message", "externalSource", "externalMessage");

	EXPECT_EQ(e.Method(), "method");
	EXPECT_EQ(e.Message(), "message");
	EXPECT_EQ(e.ExternalSource(), "externalSource");
	EXPECT_EQ(e.ExternalMessage(), "externalMessage");
}