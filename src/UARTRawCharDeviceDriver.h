#ifndef UARTRAWCHARDEVICEDRIVER_H
#define UARTRAWCHARDEVICEDRIVER_H

#include "ardunix.h"
#include "AbstractRawCharDevice.h"

// this class represents the uart char device, which can be written on on read from
class UARTRawCharDeviceDriver : public AbstractRawCharDevice
{
	public:
		UARTRawCharDeviceDriver();
		~UARTRawCharDeviceDriver();

		ubyte1 init();
		ubyte1 receiveByte();
      ubyte1 transmitByte(byte& val);
		ubyte1 bytesAvailable();
};

#endif
