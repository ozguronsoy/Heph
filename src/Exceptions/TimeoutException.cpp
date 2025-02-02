#include "Exceptions/TimeoutException.h"

namespace Heph
{
	TimeoutException::TimeoutException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string TimeoutException::Name() const noexcept
	{
		return "TimeoutException";
	}

	void TimeoutException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<TimeoutException>(*this));
	}
}