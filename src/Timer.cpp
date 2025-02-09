#include "Timer.h"
#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
    Timer::Timer(std::chrono::milliseconds period)
        : period(period), running(false)
    {
    }

    Timer::Timer(Timer&& rhs) noexcept
        : thread(std::move(rhs.thread)),
        running(std::move(rhs.running)),
        period(std::move(rhs.period)),
        timeoutEvent(std::move(rhs.timeoutEvent))
    {
    }

    Timer::~Timer()
    {
        this->Stop();
    }

    Timer& Timer::operator=(Timer&& rhs) noexcept
    {
        if (&rhs != this)
        {
            this->thread = std::move(rhs.thread);
            this->running = std::move(rhs.running);
            this->period = std::move(rhs.period);
            this->timeoutEvent = std::move(rhs.timeoutEvent);
        }

        return *this;
    }

    void Timer::Start()
    {
        if (!this->running)
        {
            this->running = true;
            this->thread = std::thread(&Timer::Run, this);
        }
    }

    void Timer::Stop()
    {
        if (this->running)
        {
            this->running = false;
            this->thread.join();
        }
    }

    bool Timer::IsRunning() const
    {
        return this->running;
    }

    void Timer::Run() const
    {
        while (this->running)
        {
            this->timeoutEvent.Invoke(nullptr, nullptr);
            std::this_thread::sleep_for(this->period);
        }
    }
}