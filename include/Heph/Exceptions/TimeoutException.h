#ifndef HEPH_TIMEOUT_EXCEPTION_H
#define HEPH_TIMEOUT_EXCEPTION_H

#include "Heph/Utils.h"
#include "Heph/Exceptions/Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an operation takes too long to complete and reaches timeout. */
	class HEPH_API TimeoutException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		TimeoutException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~TimeoutException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif