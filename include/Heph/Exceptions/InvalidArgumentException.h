#ifndef HEPH_INVALID_ARGUMENT_EXCEPTION_H
#define HEPH_INVALID_ARGUMENT_EXCEPTION_H

#include "Heph/Utils.h"
#include "Heph/Exceptions/Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an argument passed to a method is invalid. */
	class HEPH_API InvalidArgumentException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		InvalidArgumentException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~InvalidArgumentException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif