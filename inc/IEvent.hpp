// #ifndef __IEVENT__HPP__
// #define __IEVENT__HPP__

#pragma once

class IEvent
{
	public:
		IEvent();
		IEvent(IEvent const & src);
		virtual ~IEvent();
	
		virtual void determinism() = 0;
};

// #endif
