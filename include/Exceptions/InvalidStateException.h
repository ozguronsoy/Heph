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

		virtual std::string Name() const noexcept override;

	protected:
		virtual void AddToExceptions() const override;
	};
}

#endif