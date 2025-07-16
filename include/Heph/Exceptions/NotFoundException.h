#ifndef HEPH_NOT_FOUND_EXCEPTION_H
#define HEPH_NOT_FOUND_EXCEPTION_H

#include "Heph/Utils.h"
#include "Heph/Exceptions/Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when a search fails. */
	class HEPH_API NotFoundException : public Exception
	{
	public:
		/** @copydoc Exception(const std::string&,const std::string&) */
		NotFoundException(const std::string& method, const std::string& message);
		/** @copydoc destructor */
		virtual ~NotFoundException() = default;

        virtual ICloneable* Clone() const noexcept override;
		virtual std::string Name() const noexcept override;
	};
}

#endif