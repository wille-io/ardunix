#ifndef RAWCHARDEVICEACCESSOR_H	
#define RAWCHARDEVICEACCESSOR_H

#include "ardunix.h"
#include "UARTRawCharDeviceDriver.h"
#include "AbstractRawDeviceAccessor.h"

// this class manages to find and read from and write to a char device
class RawCharDeviceAccessor : public AbstractRawDeviceAccessor
{
	public:
      AbstractRawDevice* getDeviceByType(ubyte1 deviceType);
		AbstractRawDevice* getDeviceByName(const char* deviceName);

		ubyte1 receiveByte(byte& deviceType);
      ubyte1 transmitByte(byte& deviceType, ubyte1 val);
      ubyte1 bytesAvailable(byte& deviceType);
      ubyte1 getLatestError(byte& deviceType);

	private:
		UARTRawCharDeviceDriver uartRCDD;
};

#endif // RAWCHARDEVICEACCESSOR_H
