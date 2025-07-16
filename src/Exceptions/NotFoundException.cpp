#include "Heph/Exceptions/NotFoundException.h"

namespace Heph
{
	NotFoundException::NotFoundException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* NotFoundException::Clone() const noexcept
    {
        return new NotFoundException(*this);
    }

	std::string NotFoundException::Name() const noexcept
	{
		return "NotFoundException";
	}
}