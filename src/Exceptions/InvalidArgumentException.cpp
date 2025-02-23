#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
	InvalidArgumentException::InvalidArgumentException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* InvalidArgumentException::Clone() const noexcept
    {
        return new InvalidArgumentException(*this);
    }

	std::string InvalidArgumentException::Name() const noexcept
	{
		return "InvalidArgumentException";
	}
}