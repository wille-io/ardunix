#include "EventController.h"

EventController eventController;

EventController::EventController()
{

}
EventController::~EventController()
{

}

void EventController::registerEvent(ubyte1 eventType, AbstractEventClass* eventClass)
{
	eventList.appendEvent(new Event(eventType, eventClass)); // add a new Event class to the eventList
}

void EventController::classUnsubscribe(AbstractEventClass* eventClass)
{
   eventList.deleteEventsByClass(eventClass);
}

void EventController::unsubscribeEvent(ubyte1 eventType, AbstractEventClass* eventClass)
{
   eventList.deleteEvent(eventType, eventClass);
}

void EventController::fireEvent(ubyte1 eventType, void* ptr)
{			
	//Event* event = eventList.getEvent(eventType); // find the correspondenting event - returns 0 if invalid index

   EventList* tempEventList = eventList.getEvents((byte)eventType);

   if (!tempEventList)
      return;

   Event* event = tempEventList->list.getFirstEntry();

	int counter = 0; // this counter HAS to be more than 2 because: sender receives event, receiver receives event (+ other receivers)
	// 0 = invalid event (event is not specified in ardunix because not even the sender received it)
	// 1 = event not subscribed (sender received it, but nobody has subscribed to the event)
	// 2 = perfectly fine
	// ... = more receivers received the event

	while (event)
	{
		if (counter > 24)
		{
			//std::cerr << "event controller: event has more than 24 subscribers. cancelled after subscriber #24" << std::endl;
			return;
		}

		if (event->eventType == eventType) // for safety issues - check if the eventType fits to the
		{
			counter ++;
         AbstractEventClass* eventClass = (AbstractEventClass*)event->eventClass; // Kann es sein, dass schon eine Klasse gelsöcht sein kann, obwohl sie in unserer neuen Liste steht?!?!?!
         if (eventClass)
			{
            eventClass->handleEvent(eventType, ptr);
				// return; never return, more classes could hasve subscribed
			}
		}

      event = tempEventList->list.getNextEntry();
	}


	//if (counter < 2)
	//   std::cerr << "Invalid event   counter: " << counter << std::endl;

	/*switch (counter) THIS WILL END IN A INFINITE LOOP IF NOTHING HAS SUBSCRIBED TO THEM !!!
{
	case 0: fireEvent(ardunix::events::etLogError, (void*)F("event controller: unknown event (not implemented in ardunix)"));
	case 1: fireEvent(ardunix::events::etLogError, (void*)F("event controller: no class has subscribed to that event"));
}*/
		
}
