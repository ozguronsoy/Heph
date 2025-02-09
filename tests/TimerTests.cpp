#include <gtest/gtest.h>
#include "Timer.h"
#include "Stopwatch.h"

#define TIMER_ARG_KEY "Number" 

using namespace Heph;

void HandleTimerOverflow(const EventParams& params)
{
    int& n = *reinterpret_cast<int*>(params.userArgs.at(TIMER_ARG_KEY));
    n++;
}

TEST(HephTest, Timer)
{
    constexpr std::chrono::milliseconds TEST_TIMER_PERIOD = std::chrono::milliseconds(200);
    constexpr std::chrono::milliseconds TEST_DURATION = std::chrono::milliseconds(1000);
    int n = 0;

    Timer timer(TEST_TIMER_PERIOD);
    timer.timeoutEvent += HandleTimerOverflow;
    timer.timeoutEvent.userArgs[TIMER_ARG_KEY] = reinterpret_cast<void*>(&n);

    Stopwatch sw;
    sw.Reset();

    timer.Start();
    std::this_thread::sleep_for(TEST_DURATION);
    timer.Stop();

    const int dt = sw.DeltaTime() * 1e-6;
    const int expected = dt / TEST_TIMER_PERIOD.count();

    EXPECT_EQ(n, expected);
}