#ifndef HEPH_INSUFFICIENT_MEMORY_EXCEPTION_H
#define HEPH_INSUFFICIENT_MEMORY_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an allocation fails due to insufficient memory. */
	class HEPH_API InsufficientMemoryException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		InsufficientMemoryException(const std::string& method, const std::string& message);

		virtual std::string Name() const noexcept override;
	
	protected:
		virtual void AddToExceptions() const override;
	};
}

#endif