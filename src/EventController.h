#ifndef EVENTCONTROLLER_H
#define EVENTCONTROLLER_H

#include "ardunix.h"
#include "AbstractEventClass.h"
#include "EventList.h"

// this class contains an event list, and can execute, add and remove events
class EventController
{
	public:
      EventList eventList; // event list

		EventController();
		~EventController();
      void registerEvent(ubyte1 eventType, AbstractEventClass* eventClass);
		void fireEvent(ubyte1 eventType, void* ptr = 0); // can be called without pointer
      void unsubscribeEvent(ubyte1 eventType, AbstractEventClass* eventClass);
      void classUnsubscribe(AbstractEventClass* eventClass);
};

extern EventController eventController; // has to be accessible for every (non-event-)class

#endif
