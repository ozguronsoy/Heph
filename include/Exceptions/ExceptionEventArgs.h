#ifndef HEPH_EXCEPTION_EVENT_ARGS_H
#define HEPH_EXCEPTION_EVENT_ARGS_H
#include "HephShared.h"
#include "EventArgs.h"

/** @file */

namespace Heph
{
	class Exception;

	/** @brief Struct for storing the arguments for the exception events. */
	struct HEPH_API ExceptionEventArgs : public EventArgs
	{
		/** The exception that's being raised. */
		const Exception& exception;

		/**
		 * @copydoc constructor
		 *
		 * @param ex @copydetails exception
		 */
		explicit ExceptionEventArgs(const Exception& ex);

		/** @copydoc destructor */
		virtual ~ExceptionEventArgs() = default;
	};
}

#endif