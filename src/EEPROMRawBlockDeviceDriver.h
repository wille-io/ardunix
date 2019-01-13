#pragma once

#include "ardunix.h"
#include "AbstractRawBlockDevice.h"

// this class represents the eeprom block device, which can be written on on read from
class EEPROMRawBlockDeviceDriver : public AbstractRawBlockDevice
{
	public:
		EEPROMRawBlockDeviceDriver();
		//~EEPROMRawBlockDeviceDriver();

		ubyte1 init();
		ubyte1 getByte(ubyte8& pos);
		void setByte(int& pos, byte& val);
};
