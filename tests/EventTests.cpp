#include <gtest/gtest.h>
#include "Event.h"
#include "Complex.h"
#include <string>

using namespace Heph;

class EventTest : public testing::Test
{
protected:
	static constexpr const char* THIS_KEY = "this";
	static constexpr const char* PARAM1_KEY = "param1";
	static constexpr const char* PARAM2_KEY = "param2";
	static constexpr const char* PARAM3_KEY = "param3";

protected:
	Event event;

	std::string param1;
	int param2;
	Complex<double> param3;

	bool handler1;
	bool handler2;
	bool handler3;
	bool handler4;

	EventArgs* pArgs;
	EventResult* pResult;

protected:
	EventTest()
	{
		handler1 = false;
		handler2 = false;
		handler3 = false;
		handler4 = false;

		param1 = "Hello World!";
		param2 = 37;
		param3 = Complex<double>(10, 20);

		pArgs = nullptr;
		pResult = nullptr;
	}

	static void Handler1(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userArgs.at(THIS_KEY);

		EXPECT_TRUE(pThis != nullptr);
		EXPECT_TRUE(params.userArgs.find(THIS_KEY) != params.userArgs.end());
		EXPECT_EQ(params.userArgs.at(PARAM1_KEY), &pThis->param1);
		EXPECT_EQ(params.userArgs.at(PARAM2_KEY), &pThis->param2);
		EXPECT_EQ(params.userArgs.at(PARAM3_KEY), &pThis->param3);
		EXPECT_TRUE(params.userArgs.find("DOES NOT EXIST") == params.userArgs.end());

		pThis->handler1 = true;
	}

	static void Handler2(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userArgs.at(THIS_KEY);
		pThis->handler2 = true;
		params.pResult->isHandled = true;
	}

	static void Handler3(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userArgs.at(THIS_KEY);
		pThis->handler3 = true;
	}

	static void Handler4(const EventParams& params)
	{
		EventTest* pThis = (EventTest*)params.userArgs.at(THIS_KEY);

		EXPECT_EQ(params.pArgs, pThis->pArgs);
		EXPECT_EQ(params.pResult, pThis->pResult);

		pThis->handler4 = true;
	}
};

TEST_F(EventTest, HandlerMethods)
{
	EXPECT_EQ(event.Size(), 0);

	event = &Handler1;
	EXPECT_EQ(event.Size(), 1);
	EXPECT_TRUE(event.HandlerExists(&Handler1));
	EXPECT_FALSE(event.HandlerExists(&Handler2));

	event = &Handler2;
	EXPECT_EQ(event.Size(), 1);
	EXPECT_TRUE(event.HandlerExists(&Handler2));
	EXPECT_FALSE(event.HandlerExists(&Handler1));

	event.SetHandler(&Handler3);
	EXPECT_EQ(event.Size(), 1);
	EXPECT_TRUE(event.HandlerExists(&Handler3));
	EXPECT_FALSE(event.HandlerExists(&Handler1));
	EXPECT_FALSE(event.HandlerExists(&Handler2));

	event += &Handler1;
	event.AddHandler(&Handler2);
	EXPECT_EQ(event.Size(), 3);
	EXPECT_TRUE(event.HandlerExists(&Handler1));
	EXPECT_TRUE(event.HandlerExists(&Handler2));
	EXPECT_TRUE(event.HandlerExists(&Handler3));
	EXPECT_EQ(event.GetHandler(0), &Handler3);
	EXPECT_EQ(event.GetHandler(1), &Handler1);
	EXPECT_EQ(event.GetHandler(2), &Handler2);

	event.ClearHandlers();
	EXPECT_EQ(event.Size(), 0);
}

TEST_F(EventTest, Invoke)
{
	event = &Handler1;
	event += &Handler2;
	event += &Handler3;
	event += &Handler4;

	EXPECT_EQ(event.userArgs.size(), 0);
	event.userArgs[THIS_KEY] = this;
	event.userArgs[PARAM1_KEY] = &param1;
	event.userArgs[PARAM2_KEY] = &param2;
	event.userArgs[PARAM3_KEY] = &param3;
	EXPECT_EQ(event.userArgs.size(), 4);

	event.userArgs.erase(PARAM3_KEY);
	EXPECT_EQ(event.userArgs.size(), 3);
	EXPECT_TRUE(event.userArgs.find(PARAM3_KEY) == event.userArgs.end());
	event.userArgs[PARAM3_KEY] = &param3;

	event.Invoke(nullptr, nullptr);

	EXPECT_TRUE(handler1);
	EXPECT_TRUE(handler2);
	EXPECT_FALSE(handler3);
	EXPECT_FALSE(handler4);

	handler1 = false;
	handler2 = false;
	handler3 = false;
	handler4 = false;

	event.RemoveHandler(&Handler2);
	event.RemoveHandler(&Handler3);

	EventArgs args;
	EventResult result;

	pArgs = &args;
	pResult = &result;

	event(&args, &result);

	EXPECT_TRUE(handler1);
	EXPECT_FALSE(handler2);
	EXPECT_FALSE(handler3);
	EXPECT_TRUE(handler4);
}