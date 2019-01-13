#include "AbstractEventClass.h"
#include "EventController.h"

AbstractEventClass::~AbstractEventClass()
{
   eventController.classUnsubscribe(this); // delete all events of this class before deleting it!!!
}
