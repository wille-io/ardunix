#ifndef RAWBLOCKDEVICEACCESSOR_H
#define RAWBLOCKDEVICEACCESSOR_H

#include "ardunix.h"
#include "AbstractRawDeviceAccessor.h"
#include "EEPROMRawBlockDeviceDriver.h"
#include "SDRawBlockDeviceAccessor.h"

// this class manages to find and read from and write to a block device
class RawBlockDeviceAccessor : public AbstractRawDeviceAccessor
{
	public:
		AbstractRawDevice* getDeviceByType(ubyte1 deviceType);
		AbstractRawDevice* getDeviceByName(const char* deviceName);
		ubyte1 getByte(byte& deviceType, ubyte8 pos);
		void setByte(byte& deviceType, int pos, byte& val);
      ubyte1 getLatestError(byte& deviceType);

	private:
		EEPROMRawBlockDeviceDriver eepromRBDD;
		SDRawBlockDeviceAccessor sdcardRBDD;
};

#endif
