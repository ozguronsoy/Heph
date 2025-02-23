#include "Exceptions/InvalidStateException.h"

namespace Heph
{
	InvalidStateException::InvalidStateException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* InvalidStateException::Clone() const noexcept
    {
        return new InvalidStateException(*this);
    }

	std::string InvalidStateException::Name() const noexcept
	{
		return "InvalidStateException";
	}
}