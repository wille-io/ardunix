#ifndef ABSTRACTRAWDEVICE_H
#define ABSTRACTRAWDEVICE_H

#include "ardunix.h"

// this class represents a anonymous device, which can be written on on read from
class AbstractRawDevice
{
	public:
		AbstractRawDevice(ubyte1 deviceType, const char* deviceName);
		~AbstractRawDevice();

		virtual ubyte1 init() = 0;

		ubyte1 deviceType;
		char*  deviceName;
		ubyte1 error;
};

#endif
