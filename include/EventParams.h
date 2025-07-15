#ifndef HEPH_EVENT_PARAMS_H
#define HEPH_EVENT_PARAMS_H

#include "HephShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <type_traits>

/** @file */

namespace Heph
{
	/** @brief Stores the information required to handle an event. */
	class HEPH_API EventParams final
	{
	private:
		/** @brief Event arguments. */
		const EventArgs& args;

		/** @brief Event result. */
		EventResult& result;

	public:
		/** @copydoc constructor */
		EventParams(const EventArgs& args, EventResult& result);

		/**
		 * Gets the event args.
		 *
		 * @tparam TArgs Type of the event args
		 */
		template<typename TArgs = EventArgs>
			requires std::is_base_of_v<EventArgs, TArgs>
		const TArgs& Args() const
		{
			return reinterpret_cast<const TArgs&>(this->args);
		}

		/**
		 * Gets the event result.
		 *
		 * @tparam TResult Type of the event result
		 */
		template<typename TResult = EventResult>
			requires std::is_base_of_v<EventResult, TResult>
		TResult& Result()
		{
			return reinterpret_cast<TResult&>(this->result);
		}
	};
}

#endif