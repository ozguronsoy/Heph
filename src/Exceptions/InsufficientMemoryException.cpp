#include "Heph/Exceptions/InsufficientMemoryException.h"

namespace Heph
{
	InsufficientMemoryException::InsufficientMemoryException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* InsufficientMemoryException::Clone() const noexcept
    {
        return new InsufficientMemoryException(*this);
    }

	std::string InsufficientMemoryException::Name() const noexcept
	{
		return "InsufficientMemoryException";
	}
}