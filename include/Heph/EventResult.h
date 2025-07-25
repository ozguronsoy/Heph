#ifndef HEPH_EVENT_RESULT_H
#define HEPH_EVENT_RESULT_H

#include "Heph/Utils.h"

/** @file */

namespace Heph
{
	/** @brief Base class for storing the results of an event. */
	struct HEPH_API EventResult
	{
		/**
		 * @brief Indicates the event is handled and no further handlers will be invoked.<br>
		 * Set by the user.
		 */
		bool isHandled;

		/** @copydoc default_constructor */
		EventResult();

		/** @copydoc destructor */
		virtual ~EventResult() = default;
	};
}

#endif