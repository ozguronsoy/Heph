#include "Exceptions/NotFoundException.h"

namespace Heph
{
	NotFoundException::NotFoundException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotFoundException::Name() const noexcept
	{
		return "NotFoundException";
	}

	void NotFoundException::AddToExceptions() const
	{
		Exception::ExceptionListInstance().push_back(std::make_unique<NotFoundException>(*this));
	}
}