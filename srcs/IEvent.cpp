#include "IEvent.hpp"

IEvent::IEvent() {
	// std::cout << _ITALIC "IEvent Constructor called" _END << std::endl;
	return;
}

IEvent::IEvent( IEvent const & src ) {
	// std::cout << _ITALIC "IEvent Copy Constructor called" _END << std::endl;
	(void)src;
	return;
}

IEvent::~IEvent() {
	// std::cout << _ITALIC "IEvent Destructor called" << std::endl;
	return;
}

