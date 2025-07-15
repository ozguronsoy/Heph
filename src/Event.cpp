#include "Event.h"
#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
	Event::Event()
		: eventHandlers(0)
	{
	}

	void Event::operator()(const EventArgs* pArgs, EventResult* pResult) const
	{
		this->Invoke(pArgs, pResult);
	}

	Event& Event::operator=(Handler handler)
	{
		this->SetHandler(handler);
		return *this;
	}

	Event& Event::operator+=(Handler handler)
	{
		this->AddHandler(handler);
		return *this;
	}

	size_t Event::Size() const
	{
		return this->eventHandlers.size();
	}

	void Event::SetHandler(Handler handler)
	{
		this->eventHandlers.clear();
		this->AddHandler(handler);
	}

	void Event::AddHandler(Handler handler)
	{
		if (handler)
			this->eventHandlers.push_back(handler);
	}

	void Event::Clear()
	{
		this->eventHandlers.clear();
	}

	void Event::Invoke(const EventArgs* pArgs, EventResult* pResult) const
	{
		const EventArgs defaultArgs;
		EventResult defaultResult;

		EventParams eventParams(
			((pArgs != nullptr) ? (*pArgs) : (defaultArgs)),
			((pResult != nullptr) ? (*pResult) : (defaultResult))
		);

		for (Handler handler : this->eventHandlers)
		{
			handler(eventParams);
			if (eventParams.Result<EventResult>().isHandled) return;
		}
	}
}