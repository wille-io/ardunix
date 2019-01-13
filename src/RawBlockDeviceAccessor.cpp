#include "RawBlockDeviceAccessor.h"
#include "EventController.h"

AbstractRawDevice* RawBlockDeviceAccessor::getDeviceByType(ubyte1 deviceType)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::eeprom:
		case ardunix::filesystem::devicetype::eeprom1:
		case ardunix::filesystem::devicetype::eeprom2: return (AbstractRawDevice*)&this->eepromRBDD;
		case ardunix::filesystem::devicetype::sdcard: return (AbstractRawDevice*)&this->sdcardRBDD;
	}

	return 0; // null-pointer
}

AbstractRawDevice* RawBlockDeviceAccessor::getDeviceByName(const char *deviceName)
{
	if (!strcmp(deviceName, this->eepromRBDD.deviceName)) // device found
		return (AbstractRawDevice*)&this->eepromRBDD;

	if (!strcmp(deviceName, this->sdcardRBDD.deviceName))
		return (AbstractRawDevice*)&this->sdcardRBDD;

	return 0; // null-pointer
}

ubyte1 RawBlockDeviceAccessor::getByte(ubyte1& deviceType, ubyte8 pos)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::eeprom:
		case ardunix::filesystem::devicetype::eeprom1:
		case ardunix::filesystem::devicetype::eeprom2: return this->eepromRBDD.getByte(pos);
		case ardunix::filesystem::devicetype::sdcard: return this->sdcardRBDD.getByte(pos);
	}

	eventController.fireEvent(ardunix::events::etStdOutFlash, ((void*)F("RBDA: device type not supported")));

	return 0; // null-pointer
}

void RawBlockDeviceAccessor::setByte(ubyte1& deviceType, int pos, ubyte1& val)
{
	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::eeprom:
		case ardunix::filesystem::devicetype::eeprom1:
		case ardunix::filesystem::devicetype::eeprom2: this->eepromRBDD.setByte(pos, val);
		case ardunix::filesystem::devicetype::sdcard: this->sdcardRBDD.setByte(pos, val);
	}
}

ubyte1 RawBlockDeviceAccessor::getLatestError(byte& deviceType)
{
	ubyte1 error = 255; // device not found  TODO: reference on error to avoid more source code

	switch (deviceType)
	{
		case ardunix::filesystem::devicetype::eeprom:
		case ardunix::filesystem::devicetype::eeprom1:
		case ardunix::filesystem::devicetype::eeprom2: error = this->eepromRBDD.error; this->eepromRBDD.error = 0; break;
		case ardunix::filesystem::devicetype::sdcard: error = this->sdcardRBDD.error; this->sdcardRBDD.error = 0; break;
	}

	return error;
}
