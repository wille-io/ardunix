#ifndef ABSTRACTRAWCHARDEVICE_H
#define ABSTRACTRAWCHARDEVICE_H

#include "ardunix.h"
#include "AbstractRawDevice.h"

// this class represents a block device, which can be written on on read from
class AbstractRawCharDevice : public AbstractRawDevice
{
	public:
		AbstractRawCharDevice(ubyte1 deviceType, const char* deviceName);
		virtual ubyte1 receiveByte() = 0;
      virtual ubyte1 transmitByte(byte& val) = 0;
		virtual ubyte1 bytesAvailable() = 0;
};


#endif
