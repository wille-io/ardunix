#include "List.h"
#include "EventList.h"

//template class List<Event*>;

EventList::EventList()
{

}

EventList::~EventList()
{
   //ListKiller(Event*)
}

void EventList::appendEvent(Event* event)
{
   list.appendEntry(event);
   this->printEventList();
}

EventList* EventList::getEvents(AbstractEventClass* eventClass)
{
   EventList* eventList = 0; // only allocate a new list if there is one or more events

   Event* event = list.getFirstEntry();

   while (event)
   {
      if (event->eventClass == eventClass)
      {
         if (!eventList)
            eventList = new EventList;

         eventList->list.appendEntry(event); // add event to new list, so that the calling function has a thread safe list (because no one else works with it at the same time)
      }

      event = list.getNextEntry();
   }

   if (eventList)
      eventList->printEventList();
   return eventList; // returns 0 if there are no results and a EventList if there are some
}

EventList* EventList::getEvents(ubyte1 eventType)
{
   EventList* eventList = 0; // only allocate a new list if there are one or more events

   Event* event = list.getFirstEntry();

   while (event)
   {
      if (event->eventType == eventType)
      {
         if (!eventList)
            eventList = new EventList;

         eventList->list.appendEntry(event); // add event to new list, so that the calling function has a thread safe list (because no one else works with it at the same time)
      }

      event = list.getNextEntry();
   }

   if (eventList)
      eventList->printEventList();
   return eventList; // returns 0 if there are no results and a EventList if there are some
}

void EventList::printEventList()
{
   return;

   Event* event = list.getFirstEntry();

	/*if (event)
      std::cerr << "Current event list:" << std::endl;
   else
      std::cerr << "Current event list is empty!" << std::endl;
*/
   while (event)
   {
		//std::cerr << "Event>  type: " << (int)event->eventType << "  class: " << event->eventClass << std::endl;
      event = list.getNextEntry();
   }

	//std::cerr << "END OF LIST" << std::endl << std::endl;
}

void EventList::deleteEvent(ubyte1 eventType, AbstractEventClass* eventClass)
{
   Event* event = list.getFirstEntry();

   while (event)
   {
      if (event->eventType == eventType && event->eventClass == eventClass) // if pointers are the same
      {
         //std::cerr << "found event! eventType = " << (int)eventType << "  eventClass = " << eventClass << std::endl;
         list.deleteEntry(event);
         this->printEventList();
         return; // ???????????????????????????????????????????????????????????????????????????????????????????????
      }

      event = list.getNextEntry();
   }
}

void EventList::deleteEvent(Event* event)
{
   Event* ev = list.getFirstEntry();

   while (ev)
   {
      if (ev == event) // if pointers are the same
      {
         list.deleteEntry(event);
         this->printEventList();
         return; // ???????????????????????????????????????????????????????????????????????????????????????????????
      }

      ev = list.getNextEntry();
   }
}

void EventList::deleteEventsByClass(AbstractEventClass* eventClass)
{
   //std::cerr << "deleteEventsByClass" << std::endl;

   EventList* eventList = this->getEvents(eventClass);
   eventList->printEventList();

   if (!eventList)
      return;

   Event* event = eventList->list.getFirstEntry();

   while (event)
   {
      if (event->eventClass == eventClass)
         this->deleteEvent(event); // add event to new list, so that the calling function has a thread safe list (because no one else works with it at the same time)

      event = eventList->list.getNextEntry();
   }
}











