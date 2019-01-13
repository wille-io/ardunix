#include "UARTRawCharDeviceDriver.h"

UARTRawCharDeviceDriver::~UARTRawCharDeviceDriver()
{
	Serial.end();
}

UARTRawCharDeviceDriver::UARTRawCharDeviceDriver() : AbstractRawCharDevice((int)ardunix::filesystem::devicetype::uart, ArdunixHeader::uart_string)
{

}

ubyte1 UARTRawCharDeviceDriver::init()
{
	Serial.begin(19200); // use the internal serial library with 19200 baud
    //Serial.print(" * UARTRCDD: ");
    //Serial.println("19200 baud ... [done]");

	return 0; // found & initialized device
}

ubyte1 UARTRawCharDeviceDriver::receiveByte()
{
	return Serial.read();
}

ubyte1 UARTRawCharDeviceDriver::transmitByte(ubyte1 &val)
{
	Serial.write(val);

	return 0; // write successful
}

ubyte1 UARTRawCharDeviceDriver::bytesAvailable()
{
	return Serial.available();
}
