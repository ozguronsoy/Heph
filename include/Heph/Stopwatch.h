#ifndef HEPH_STOPWATCH_H
#define HEPH_STOPWATCH_H

#include "Heph/Utils.h"
#include <chrono>

/** @file */

/** Resets the thread local Stopwatch instance. */
#define HEPH_SW_RESET	Heph::Stopwatch::Instance().Reset()

/** Gets the elapsed time since the last reset of the thread local Stopwatch instance in nanoseconds. */
#define HEPH_SW_DT		Heph::Stopwatch::Instance().DeltaTime()

/** Gets the elapsed time since the last reset of the thread local Stopwatch instance in seconds. */
#define HEPH_SW_DT_S	(HEPH_SW_DT * 1e-9)

/** Gets the elapsed time since the last reset of the thread local Stopwatch instance in milliseconds. */
#define HEPH_SW_DT_MS	(HEPH_SW_DT * 1e-6)

/** Gets the elapsed time since the last reset of the thread local Stopwatch instance in microseconds. */
#define HEPH_SW_DT_US	(HEPH_SW_DT * 1e-3)

/** Gets the elapsed time since the last reset of the thread local Stopwatch instance in nanoseconds. */
#define HEPH_SW_DT_NS	HEPH_SW_DT

namespace Heph
{
    /** @brief Class for measuring time. */
    class HEPH_API Stopwatch final
    {
        using time_point = std::chrono::steady_clock::time_point;

    private:
        time_point tp;

    public:
        /** @copydoc default_constructor */
		Stopwatch();

		/** Restarts the stopwatch. */
		void Reset();

		/** Gets the elapsed time since the last reset in nanoseconds. */
		double DeltaTime() const;

		/** @brief Gets the thread local instance. */
		static Stopwatch& Instance() noexcept;
    };
}

#endif