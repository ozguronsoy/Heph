#ifndef HEPH_NOT_IMPLEMENTED_EXCEPTION_H
#define HEPH_NOT_IMPLEMENTED_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when a method or a class is not fully implemented yet. */
	class HEPH_API NotImplementedException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		NotImplementedException(const std::string& method, const std::string& message);

		virtual std::string Name() const noexcept override;

	protected:
		virtual void AddToExceptions() const override;
	};
}

#endif