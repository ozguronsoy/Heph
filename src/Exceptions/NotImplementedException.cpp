#include "Exceptions/NotImplementedException.h"

namespace Heph
{
	NotImplementedException::NotImplementedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotImplementedException::Name() const noexcept
	{
		return "NotImplementedException";
	}

	void NotImplementedException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<NotImplementedException>(*this));
	}
}