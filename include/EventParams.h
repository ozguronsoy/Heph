#ifndef HEPH_EVENT_PARAMS_H
#define HEPH_EVENT_PARAMS_H

#include "HephShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <string>
#include <unordered_map>

/** @file */

namespace Heph
{
	typedef std::unordered_map<std::string, void*> UserEventArgs; 

	/** @brief Stores the information required to handle an event. */
	struct HEPH_API EventParams final
	{
		/** @brief Pointer to the arguments, can be null. */
		EventArgs* pArgs;

		/** @brief Pointer to the result, can be null. */
		EventResult* pResult;

		/** @copydoc Event::userArgs */
		const UserEventArgs& userArgs;
		
		/**
		 * @copydoc constructor
		 * 
		 * @param userArgs @copybrief userArgs
		 */
		explicit EventParams(const UserEventArgs& userArgs);
	};
}

#endif