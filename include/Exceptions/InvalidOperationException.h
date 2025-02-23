#ifndef HEPH_INVALID_OPERATION_EXCEPTION_H
#define HEPH_INVALID_OPERATION_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an operation is invalid. */
	class HEPH_API InvalidOperationException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		InvalidOperationException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~InvalidOperationException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif