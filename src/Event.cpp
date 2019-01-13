#include "Event.h"
#include "AbstractEventClass.h"

Event::Event(ubyte1 eventType, AbstractEventClass* eventClass)
{
	this->eventType = eventType;
	this->eventClass = eventClass;
}