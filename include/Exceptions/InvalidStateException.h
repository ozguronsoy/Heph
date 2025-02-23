#ifndef HEPH_INVALID_STATE_EXCEPTION_H
#define HEPH_INVALID_STATE_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when the object state is invalid. */
	class HEPH_API InvalidStateException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		InvalidStateException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~InvalidStateException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif