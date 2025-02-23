#include "Exceptions/NotImplementedException.h"

namespace Heph
{
	NotImplementedException::NotImplementedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* NotImplementedException::Clone() const noexcept
    {
        return new NotImplementedException(*this);
    }

	std::string NotImplementedException::Name() const noexcept
	{
		return "NotImplementedException";
	}
}