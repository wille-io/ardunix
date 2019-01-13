#ifndef ABSTRACTRAWDEVICEACCESSOR_H	
#define ABSTRACTRAWDEVICEACCESSOR_H

#include "ardunix.h"
#include "AbstractRawDevice.h"

// this class class represents a anonymous device which can be found by a name or a type
class AbstractRawDeviceAccessor
{
	public:
      virtual AbstractRawDevice* getDeviceByType(ubyte1 deviceType) = 0;
		virtual AbstractRawDevice* getDeviceByName(const char* deviceName)  = 0;
};

#endif
