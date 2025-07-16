#include "Heph/Exceptions/TimeoutException.h"

namespace Heph
{
	TimeoutException::TimeoutException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* TimeoutException::Clone() const noexcept
    {
        return new TimeoutException(*this);
    }

	std::string TimeoutException::Name() const noexcept
	{
		return "TimeoutException";
	}
}