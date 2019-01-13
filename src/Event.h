#ifndef EVENT_H
#define EVENT_H

#include "ardunix.h"
#include "AbstractEventClass.h"

// this class represents an event. the event can be recognized by the event type and/pr the event class, which is added to an event list
class Event
{
	public:
		Event(ubyte1 eventType, AbstractEventClass* eventClass);		

		ubyte1 eventType;
		AbstractEventClass* eventClass;
};

#endif
