#ifndef HEPH_EXTERNAL_EXCEPTION_H
#define HEPH_EXTERNAL_EXCEPTION_H

#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/** @brief Raised when an operation from an external library/API fails. */
	class HEPH_API ExternalException : public Exception
	{
	private:
		/** @brief Name of the external source that caused the exception. */
		std::string externalSource;

		/** @brief Description of the exception provided by the external source. */
		std::string externalMessage;

	public:
		/**
		 * @copydoc Exception(const std::string&,const std::string&)
		 *
		 * @param externalSource @copybrief externalSource
		 * @param externalMessage @copybrief externalMessage
		 */
		ExternalException(const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);

		/** @copydoc destructor */
		virtual ~ExternalException() = default;

		virtual ICloneable* Clone() const noexcept override;
		virtual std::string FormattedMessage() const noexcept override;
		virtual std::string Name() const noexcept override;

		/** Gets the external source. */
		const std::string& ExternalSource() const;

		/** Gets the external message. */
		const std::string& ExternalMessage() const;
	};
}

#endif