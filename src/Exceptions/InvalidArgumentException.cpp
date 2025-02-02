#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
	InvalidArgumentException::InvalidArgumentException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InvalidArgumentException::Name() const noexcept
	{
		return "InvalidArgumentException";
	}

	void InvalidArgumentException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<InvalidArgumentException>(*this));
	}
}