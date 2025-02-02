#include "Exceptions/InsufficientMemoryException.h"

namespace Heph
{
	InsufficientMemoryException::InsufficientMemoryException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InsufficientMemoryException::Name() const noexcept
	{
		return "InsufficientMemoryException";
	}

	void InsufficientMemoryException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<InsufficientMemoryException>(*this));
	}
}