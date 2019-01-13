#include "RawCharDeviceAccessor.h"

AbstractRawDevice* RawCharDeviceAccessor::getDeviceByType(ubyte1 deviceType)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::uart: return (AbstractRawDevice*)&this->uartRCDD;
	}

	return 0; // null-pointer - device not found
}

AbstractRawDevice* RawCharDeviceAccessor::getDeviceByName(const char* deviceName)
{
	if (!strcmp(deviceName, this->uartRCDD.deviceName)) // device found
		return (AbstractRawDevice*)&this->uartRCDD;

	return 0; // null-pointer - device not found
}

ubyte1 RawCharDeviceAccessor::receiveByte(byte& deviceType)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::uart: return this->uartRCDD.receiveByte();
		default: return 0; // device not found
	}
}

ubyte1 RawCharDeviceAccessor::transmitByte(byte& deviceType, ubyte1 val)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::uart: return this->uartRCDD.transmitByte(val);
		default : return 1;
	}
}

ubyte1 RawCharDeviceAccessor::bytesAvailable(byte& deviceType)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::uart: return this->uartRCDD.bytesAvailable();
		default : return 1;
	}
}

ubyte1 RawCharDeviceAccessor::getLatestError(byte& deviceType)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::uart: return this->uartRCDD.error;
		default : return 1;
	}
}
