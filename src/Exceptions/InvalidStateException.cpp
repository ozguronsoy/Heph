#include "Exceptions/InvalidStateException.h"

namespace Heph
{
	InvalidStateException::InvalidStateException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InvalidStateException::Name() const noexcept
	{
		return "InvalidStateException";
	}

	void InvalidStateException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<InvalidStateException>(*this));
	}
}