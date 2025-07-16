#ifndef HEPH_EVENT_H
#define HEPH_EVENT_H

#include "Heph/Utils.h"
#include "Heph/EventParams.h"
#include <vector>
#include <cstdint>
#include <functional>

/** @file */

namespace Heph
{
	/** @brief Class for managing callback functions. */
	class HEPH_API Event final
	{
	public:
		/** Function for handling events. */
		using Handler = std::function<void(EventParams&)>;

	private:
		std::vector<Handler> eventHandlers;

	public:
		/** @copydoc default_constructor */
		Event();

		/** @copydoc Invoke(const EventArgs*, EventResult*) const */
		void operator()(const EventArgs* pArgs, EventResult* pResult) const;
		/** @copydoc SetHandler(Handler) */
		Event& operator=(Handler handler);
		/** @copydoc AddHandler(Handler) */
		Event& operator+=(Handler handler);

		/** Gets the number of event handlers. */
		size_t Size() const;

		/** Removes all the event handlers than adds the provided one. */
		void SetHandler(Handler handler);

		/** Adds the event handler. */
		void AddHandler(Handler handler);

		/** Removes all event handlers. */
		void Clear();

		/**
		 * Raises the event.
		 *
		 * @param args Event args or ``std::nullopt``.
		 * @param result Event result or ``std::nullopt``.
		 */
		void Invoke(const EventArgs* pArgs, EventResult* pResult) const;
	};
}

#endif