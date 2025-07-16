#include "Heph/Exceptions/ExternalException.h"
#include <algorithm>

namespace Heph
{
	ExternalException::ExternalException(const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage)
		: Exception(method, message),
		externalSource(externalSource), externalMessage(externalMessage)
	{
	}

	ICloneable* ExternalException::Clone() const noexcept
    {
        return new ExternalException(*this);
    }

	std::string ExternalException::FormattedMessage() const noexcept
    {
		const std::string baseMessage = Exception::FormattedMessage();
		return baseMessage + "\nexternal source: " + this->externalSource + "\nexternal message: " + this->externalMessage + "\n";
    }

	std::string ExternalException::Name() const noexcept
	{
		return "ExternalException";
	}

	const std::string& ExternalException::ExternalSource() const
	{
		return this->externalSource;
	}

	const std::string& ExternalException::ExternalMessage() const
	{
		return this->externalMessage;
	}
}