#ifndef ABSTRACTFILESYSTEMACCESSOR_H	
#define ABSTRACTFILESYSTEMACCESSOR_H

#include "ardunix.h"
#include "VFI.h"
#include "AbstractEventClass.h"
#include "List.h"
#include "RawBlockDeviceAccessor.h"










// FILE SYSTEM ACCESSORS HAVE TO HANDLE READS AND WRITES WITH GETBYTES WITH SIZES










// this class represents a file system which is on a partition on serval devices
class AbstractFileSystemAccessor : public AbstractEventClass
{
	public:
		AbstractFileSystemAccessor(ubyte1 fileSystemType);
		~AbstractFileSystemAccessor();

		void init(VFI* dev); // vfi of device, so that the fsa knows from which device to read

		void handleEvent(ubyte1 eventType, void* ptr);

		virtual List<VFI*>* getChildrenListOfVFI(VFI* vfi) = 0;
		virtual ubyte1 getFileByte(VFI* vfi, ubyte8& position) = 0; // needs vfi to know where to take the byte from, the accessor has to work with blocks, blocksizes and file fragments
		virtual bool setFileByte(VFI* vfi, int& position, byte& value) = 0; // s.a.a.

		virtual bool validatePartition(VFI* vfi) = 0;
		virtual bool initPartition(VFI* vfi) = 0;

		ubyte1 fileSystemType;

	protected:
		RawBlockDeviceAccessor* rawBlockDeviceAccessor;
		VFI* deviceVFI;
		bool initialized;
		bool valid;
};

#endif
