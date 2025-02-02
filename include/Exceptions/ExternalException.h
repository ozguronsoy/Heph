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
		/** Name of the external source that caused the exception. */
		std::string externalSource;

		/** Description of the exception provided by the external source. */
		std::string externalMessage;

	public:
		/**
		 * @copydoc Exception(const std::string&,const std::string&)
		 *
		 * @param externalSource @copydetails externalSource
		 * @param externalMessage @copydetails externalMessage
		 */
		ExternalException(const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);

        virtual std::string FormattedMessage() const noexcept override;
		virtual std::string Name() const noexcept override;

		/** Gets the external source. */
		const std::string& ExternalSource() const;

		/** Gets the external message. */
		const std::string& ExternalMessage() const;

	protected:
		virtual void AddToExceptions() const override;
	};
}

#endif