#ifndef ABSTRACTEVENTCLASS_H
#define ABSTRACTEVENTCLASS_H

#include "ardunix.h"

// a class which needs to receive events needs to inherite from this class
class AbstractEventClass //
{
	public:
      ~AbstractEventClass();
		virtual void handleEvent(ubyte1 eventType, void* ptr) = 0; // this function has to be at a static position in program code. to archive this, an abtract class puts this function on a static position in program code
};

#endif
