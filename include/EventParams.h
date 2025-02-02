#ifndef HEPH_EVENT_PARAMS
#define HEPH_EVENT_PARAMS

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
		/** Pointer to the arguments, can be null. */
		EventArgs* pArgs;

		/** Pointer to the result, can be null. */
		EventResult* pResult;

		/** @copydoc Event::userArgs */
		const UserEventArgs& userArgs;
		
		/**
		 * @copydoc constructor
		 * 
		 * @param userArgs @copydetails userArgs
		 */
		explicit EventParams(const UserEventArgs& userArgs);
	};
}

#endif