#include "Exceptions/NotSupportedException.h"

namespace Heph
{
	NotSupportedException::NotSupportedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotSupportedException::Name() const noexcept
	{
		return "NotSupportedException";
	}

	void NotSupportedException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<NotSupportedException>(*this));
	}
}