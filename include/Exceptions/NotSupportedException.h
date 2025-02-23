#ifndef HEPH_NOT_SUPPORTED_EXCEPTION_H
#define HEPH_NOT_SUPPORTED_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when a feature is not supported on the current platform, environment, class, or method. */
	class HEPH_API NotSupportedException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		NotSupportedException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~NotSupportedException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif