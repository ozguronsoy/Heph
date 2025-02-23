#include "Exceptions/NotSupportedException.h"

namespace Heph
{
	NotSupportedException::NotSupportedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	ICloneable* NotSupportedException::Clone() const noexcept
    {
        return new NotSupportedException(*this);
    }

	std::string NotSupportedException::Name() const noexcept
	{
		return "NotSupportedException";
	}
}