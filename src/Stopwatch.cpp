#include "Stopwatch.h"

namespace Heph
{
	Stopwatch::Stopwatch() : tp(std::chrono::steady_clock::now()) {}
	
	void Stopwatch::Reset()
	{
		this->tp = std::chrono::steady_clock::now();
	}
	
	double Stopwatch::DeltaTime() const
	{
		return (std::chrono::steady_clock::now() - this->tp).count();
	}

	Stopwatch& Stopwatch::Instance() noexcept
	{
		static thread_local Stopwatch instance;
		return instance;
	}
}