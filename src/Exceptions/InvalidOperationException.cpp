#include "Exceptions/InvalidOperationException.h"

namespace Heph
{
	InvalidOperationException::InvalidOperationException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* InvalidOperationException::Clone() const noexcept
    {
        return new InvalidOperationException(*this);
    }

	std::string InvalidOperationException::Name() const noexcept
	{
		return "InvalidOperationException";
	}
}