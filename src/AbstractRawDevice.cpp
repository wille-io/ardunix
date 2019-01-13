#include "AbstractRawDevice.h"

AbstractRawDevice::~AbstractRawDevice()
{
   delete deviceName;
	this->deviceType = 0;
	this->deviceName = 0;
}

AbstractRawDevice::AbstractRawDevice(ubyte1 deviceType, const char *deviceName)
{
	this->deviceType = deviceType;
   this->deviceName = (char*)deviceName;
	this->error = 0;
}
