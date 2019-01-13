#pragma once

#include "ardunix.h"
#include "RawBlockDeviceAccessor.h"
#include "FAT16FileSystemAccessor.h"
#include "ArdunixFileSystemAccessor.h"

// this class contains a list of file systems which can be read or written to by ardunix
class FileSystemManager : public AbstractEventClass
{
	public:
		RawBlockDeviceAccessor*  rawBlockDeviceAccessor; // get from Event

		// filesystems:
		//FAT16FileSystemAccessor* fat16FileSystemAccessor;
		ArdunixFileSystemAccessor* ardunixFileSystemAccessor;

		FileSystemManager();
		~FileSystemManager();
		void handleEvent(ubyte1 eventType, void* ptr);
		VFI* getVFI(ubyte1 deviceID, VFI* parentVFI, char* nodeName); // ONLY GET VFI FOR __NEXT__ VFI, NOT FOR ALL IN nodeName !!
		List<VFI*>* getChildrenListOfVFI(VFI* vfi);
		List<VFI*>* getListOfNonVFIPathOfNode(ubyte1 deviceID, char* path);
		VFI* initializeFilesystem(ubyte1 fileSystemType, VFI* deviceVFI); //ubyte1 deviceID);

		ubyte1 getFileByte(VFI* vfiOfFile, ubyte8& pos);
		void   setFileByte(VFI* vfiOfFile, ubyte2 &pos, ubyte1 &value);
};
