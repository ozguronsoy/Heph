#ifndef HEPH_EVENT_H
#define HEPH_EVENT_H

#include "HephShared.h"
#include "EventParams.h"
#include <vector>
#include <cinttypes>

/** @file */

namespace Heph
{
	/** Method for handling events. */
	typedef void (*EventHandler)(const EventParams& eventParams);

	/** @brief Class for managing callback functions. */
	class HEPH_API Event final
	{
	private:
		std::vector<EventHandler> eventHandlers;

	public:
		/**
		 * Stores custom data to the event handlers as key/value pairs.
		 * @important This variable only stores the pointers to the arguments, 
		 * thus you have to ensure that the arguments still exist when handling the events.
		 */
		UserEventArgs userArgs;

	public:
		/** @copydoc default_constructor */
		Event();

		/** @copydoc Invoke(EventArgs*, EventResult*) const */
		void operator()(EventArgs* pArgs, EventResult* pResult) const;
		/** @copydoc SetHandler(EventHandler) */
		Event& operator=(EventHandler handler);
		/** @copydoc AddHandler(EventHandler) */
		Event& operator+=(EventHandler handler);
		/** @copydoc RemoveHandler(EventHandler) */
		Event& operator-=(EventHandler handler);

		/** Gets the number of event handlers. */
		size_t Size() const;

		/**
		 * Checks whether the provided event handler is registered.
		 *
		 * @return true if event handler exists, otherwise false.
		 */
		bool HandlerExists(EventHandler handler) const;

		/** 
		 * Gets the event handler at the provided index. 
		 * 
		 * @exception InvalidArgumentException
		 */
		EventHandler GetHandler(size_t index) const;

		/** Removes all the event handlers than adds the provided one. */
		void SetHandler(EventHandler handler);

		/** Adds the event handler. */
		void AddHandler(EventHandler handler);

		/** Removes the event handler. */
		void RemoveHandler(EventHandler handler);

		/** Removes all event handlers. */
		void ClearHandlers();

		/**
		 * Raises the event.
		 *
		 * @param pArgs Pointer to the event args or nullptr.
		 * @param pResult Pointer to the event result or nullptr.
		 */
		void Invoke(EventArgs* pArgs, EventResult* pResult) const;
	};
}

#endif