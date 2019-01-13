#ifndef ABSTRACTRAWBLOCKDEVICE_H
#define ABSTRACTRAWBLOCKDEVICE_H

#include "ardunix.h"
#include "AbstractRawDevice.h"

// this class represents a block device, which can be written on on read from
class AbstractRawBlockDevice : public AbstractRawDevice
{
	public:
		AbstractRawBlockDevice(ubyte1 deviceType, const char* deviceName);
		virtual ubyte1 getByte(ubyte8& pos) = 0;
      virtual void setByte(int& pos, byte& val) = 0;
};

#endif
