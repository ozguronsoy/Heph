#ifndef HEPH_TIMER_H
#define HEPH_TIMER_H

#include "HephShared.h"
#include "Event.h"
#include <thread>
#include <chrono>

/** @file */

namespace Heph
{
    /** @brief A basic class for executing tasks periodically on a different thread.  */
    class HEPH_API Timer final
    {
    private:
        /** Thread. */
        std::thread thread;
        /** Indicates the timer is running. */
        bool running;

    public:
        /** Period in milliseconds. */
        std::chrono::milliseconds period;
        /** Raised periodically. */
        Event timeoutEvent;

    public:
        HEPH_DISABLE_COPY(Timer);

        /**
         * @copydoc constructor
         *
         * @param period @copydetails period
         */
        explicit Timer(std::chrono::milliseconds period = std::chrono::milliseconds(100));

        /** @copydoc move_constructor */
        Timer(Timer&& rhs) noexcept;

        /** @copydoc destructor */
        ~Timer();

        Timer& operator=(Timer&& rhs) noexcept;

        /** Starts the timer. */
        void Start();
        /** Stops the timer. */
        void Stop();

        /**
         * Checks whether the timer is running.
         *
         * @return ``true`` if the timer is running, otherwise ``false``.
         */
        bool IsRunning() const;

    private:
        void Run() const;
    };
}

#endif