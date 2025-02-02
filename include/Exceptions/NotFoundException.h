#ifndef HEPH_NOT_FOUND_EXCEPTION_H
#define HEPH_NOT_FOUND_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when a search fails. */
	class HEPH_API NotFoundException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		NotFoundException(const std::string& method, const std::string& message);

		virtual std::string Name() const noexcept override;

	protected:
		virtual void AddToExceptions() const override;
	};
}

#endif