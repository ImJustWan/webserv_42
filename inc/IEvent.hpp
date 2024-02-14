#pragma once

class IEvent
{
	public:
		IEvent();
		IEvent(IEvent const & src);
		virtual ~IEvent();
	
		virtual void determinism() = 0;
};
