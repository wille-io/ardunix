#include "EEPROMRawBlockDeviceDriver.h"

EEPROMRawBlockDeviceDriver::EEPROMRawBlockDeviceDriver() :
	AbstractRawBlockDevice((int)ardunix::filesystem::devicetype::eeprom, ArdunixHeader::eeprom_string)
{

}

ubyte1 EEPROMRawBlockDeviceDriver::init()
{
	return 1; // init'd successfully
}

ubyte1 EEPROMRawBlockDeviceDriver::getByte(ubyte8& pos)
{
	return EEPROM.read(pos);
}

void EEPROMRawBlockDeviceDriver::setByte(int& pos, byte& val)
{
	EEPROM.write(pos, val);
}
