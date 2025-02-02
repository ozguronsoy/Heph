#include "Exceptions/InvalidOperationException.h"

namespace Heph
{
	InvalidOperationException::InvalidOperationException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InvalidOperationException::Name() const noexcept
	{
		return "InvalidOperationException";
	}

	void InvalidOperationException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<InvalidOperationException>(*this));
	}
}