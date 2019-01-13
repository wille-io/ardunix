#ifndef FILE_H
#define FILE_H

#include "AbstractEventClass.h"
#include "VFI.h"
#include "RawBlockDeviceAccessor.h"
#include "VFIManager.h"

// this class represents an access to general file
class File : public AbstractEventClass
{
	public:
		File();
		//~File();

		void handleEvent(ubyte1 eventType, void *ptr);

		bool openFile(VFI* file);

		ubyte1 getByte(ubyte8 pos);
		void setByte(int pos, ubyte1 value);
		ubyte2 getWord(ubyte8 pos);

		RawBlockDeviceAccessor* rawBlockDeviceAccessor;
		VFIManager* vfiManager;
		VFI* file;
};

#endif // FILE_H
