#include "Event.h"
#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
	Event::Event()
		: eventHandlers(0)
	{
	}

	void Event::operator()(EventArgs* pArgs, EventResult* pResult) const
	{
		this->Invoke(pArgs, pResult);
	}

	Event& Event::operator=(EventHandler handler)
	{
		this->SetHandler(handler);
		return *this;
	}

	Event& Event::operator+=(EventHandler handler)
	{
		this->AddHandler(handler);
		return *this;
	}

	Event& Event::operator-=(EventHandler handler)
	{
		this->RemoveHandler(handler);
		return *this;
	}

	size_t Event::Size() const
	{
		return this->eventHandlers.size();
	}

	bool Event::HandlerExists(EventHandler handler) const
	{
		for (EventHandler h : this->eventHandlers)
			if (h == handler)
				return true;
		return false;
	}

	EventHandler Event::GetHandler(size_t index) const
	{
		if (index >= this->eventHandlers.size())
		{
			HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "Index out of range.");
		}
		return this->eventHandlers[index];
	}
	void Event::SetHandler(EventHandler handler)
	{
		this->eventHandlers.clear();
		this->AddHandler(handler);
	}
	void Event::AddHandler(EventHandler handler)
	{
		if (handler != nullptr)
			this->eventHandlers.push_back(handler);
	}

	void Event::RemoveHandler(EventHandler handler)
	{
		for (size_t i = 0; i < this->eventHandlers.size(); i++)
		{
			if (this->eventHandlers[i] == handler)
			{
				this->eventHandlers.erase(this->eventHandlers.begin() + i);
				return;
			}
		}
	}

	void Event::ClearHandlers()
	{
		this->eventHandlers.clear();
	}

	void Event::Invoke(EventArgs* pArgs, EventResult* pResult) const
	{
		EventArgs defaultArgs;
		EventResult defaultResult;

		EventParams eventParams(this->userArgs);
		eventParams.pArgs = (pArgs != nullptr) ? (pArgs) : (&defaultArgs);
		eventParams.pResult = (pResult != nullptr) ? (pResult) : (&defaultResult);

		for (EventHandler handler : this->eventHandlers)
		{
			handler(eventParams);
			if (eventParams.pResult->isHandled) return;
		}
	}
}