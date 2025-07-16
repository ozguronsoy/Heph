#include <gtest/gtest.h>
#include "Heph/Event.h"
#include <string>
#include <complex>

using namespace Heph;

struct EventTest : public testing::Test
{
	Event event;

	std::string param1;
	int param2;
	std::complex<double> param3;

	bool handler1;
	bool handler2;
	bool handler3;
	bool handler4;

	EventTest()
	{
		handler1 = false;
		handler2 = false;
		handler3 = false;
		handler4 = false;

		param1 = "Hello World!";
		param2 = 37;
		param3 = std::complex<double>(10, 20);
	}

	void Handler1(EventParams& params)
	{
		this->handler1 = true;
	}

	void Handler2(EventParams& params)
	{
		this->handler2 = true;
		params.Result<EventResult>().isHandled = true;
	}

	void Handler3(EventParams& params)
	{
		this->handler3 = true;
	}

	void Handler4(EventParams& params)
	{
		this->handler4 = true;
	}
};

TEST_F(EventTest, HandlerMethods)
{
	EXPECT_EQ(event.Size(), 0);

	event = std::bind(&EventTest::Handler1, this, std::placeholders::_1);
	EXPECT_EQ(event.Size(), 1);

	event = std::bind(&EventTest::Handler2, this, std::placeholders::_1);
	EXPECT_EQ(event.Size(), 1);

	event.SetHandler(std::bind(&EventTest::Handler3, this, std::placeholders::_1));
	EXPECT_EQ(event.Size(), 1);

	event += std::bind(&EventTest::Handler1, this, std::placeholders::_1);
	event.AddHandler(std::bind(&EventTest::Handler2, this, std::placeholders::_1));
	EXPECT_EQ(event.Size(), 3);

	event.Clear();
	EXPECT_EQ(event.Size(), 0);
}

TEST_F(EventTest, Invoke)
{
	{
		event = std::bind(&EventTest::Handler1, this, std::placeholders::_1);
		event += std::bind(&EventTest::Handler2, this, std::placeholders::_1);
		event += std::bind(&EventTest::Handler3, this, std::placeholders::_1);
		event += std::bind(&EventTest::Handler4, this, std::placeholders::_1);

		event.Invoke(nullptr, nullptr);

		EXPECT_TRUE(handler1);
		EXPECT_TRUE(handler2);
		EXPECT_FALSE(handler3);
		EXPECT_FALSE(handler4);
	}

	{
		handler1 = false;
		handler2 = false;
		handler3 = false;
		handler4 = false;

		event = std::bind(&EventTest::Handler1, this, std::placeholders::_1);
		event += std::bind(&EventTest::Handler3, this, std::placeholders::_1);
		event += std::bind(&EventTest::Handler4, this, std::placeholders::_1);

		EventArgs args;
		EventResult result;
		event.Invoke(&args, &result);

		EXPECT_TRUE(handler1);
		EXPECT_FALSE(handler2);
		EXPECT_TRUE(handler3);
		EXPECT_TRUE(handler4);
	}
}