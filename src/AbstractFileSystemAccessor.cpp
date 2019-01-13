#include "AbstractFileSystemAccessor.h"
#include "EventController.h"

AbstractFileSystemAccessor::AbstractFileSystemAccessor(ubyte1 fileSystemType)
{
	this->rawBlockDeviceAccessor = 0;
	this->fileSystemType = fileSystemType;

	deviceVFI = 0;
	initialized = false;
	valid = false;

	eventController.registerEvent(ardunix::events::etGetVFIManager, this);
	eventController.fireEvent(ardunix::events::etGetVFIManager, 0); // null-pointer - because this class needs a VFI manager

	eventController.registerEvent(ardunix::events::etGetRawBlockDeviceAccessor, this);
	eventController.fireEvent(ardunix::events::etGetRawBlockDeviceAccessor, 0); // null-pointer - because this class needs a RBDA
}

AbstractFileSystemAccessor::~AbstractFileSystemAccessor()
{
	this->fileSystemType = 0;
}

void AbstractFileSystemAccessor::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetRawBlockDeviceAccessor:
			if (ptr)
				this->rawBlockDeviceAccessor = (RawBlockDeviceAccessor*)ptr; // get a RBDA if pointer is valid
			return;
	}
}
