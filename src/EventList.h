#ifndef EVENTLIST_H
#define EVENTLIST_H

#include "ardunix.h"
#include "List.h"
#include "Event.h"

// this class inherites a list with the class template 'Event*' and can add, find and remove events
class EventList //: public List<Event*> // AENDERUNG: vorher mit ListKiller, war aber nicht schoen.. arbeiten mit void* eben so wenig
{
	public:
		EventList();
		~EventList();

      void appendEvent(Event* event);
      EventList* getEvents(AbstractEventClass* eventClass);
      EventList* getEvents(ubyte1 eventType);

      void printEventList();

      void deleteEvent(Event* event); // delete event by the same event-pointers !
      void deleteEvent(ubyte1 eventType, AbstractEventClass* eventClass); // delete event by it's eventType and the corresponding class
      void deleteEventsByClass(AbstractEventClass* eventClass);

      List<Event*> list;
};

#endif
