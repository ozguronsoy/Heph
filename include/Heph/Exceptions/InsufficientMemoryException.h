#ifndef HEPH_INSUFFICIENT_MEMORY_EXCEPTION_H
#define HEPH_INSUFFICIENT_MEMORY_EXCEPTION_H

#include "Heph/Utils.h"
#include "Heph/Exceptions/Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an allocation fails due to insufficient memory. */
	class HEPH_API InsufficientMemoryException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		InsufficientMemoryException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~InsufficientMemoryException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif