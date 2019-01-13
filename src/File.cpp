#include "File.h"
#include "EventController.h"
#include "VFI.h"

File::File()
{
	file = 0;
	vfiManager = 0;
	rawBlockDeviceAccessor = 0;

	eventController.registerEvent(ardunix::events::etGetRawBlockDeviceAccessor, this);
	eventController.fireEvent(ardunix::events::etGetRawBlockDeviceAccessor, 0); // null-pointer - because this class needs a RCDA and is not sending one

	eventController.registerEvent(ardunix::events::etGetVFIManager, this);
	eventController.fireEvent(ardunix::events::etGetVFIManager, 0); // null-pointer - because this class needs a RCDA and is not sending one
}

/*File::~File()
{

}*/

void File::handleEvent(ubyte1 eventType, void *ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetRawBlockDeviceAccessor:
		{
			if (ptr)
				this->rawBlockDeviceAccessor = (RawBlockDeviceAccessor*)ptr;
			return;
		}

		case ardunix::events::etGetVFIManager:
		{
			if (ptr)
			{
				this->vfiManager = (VFIManager*)ptr;
			}
			return;
		}
	}
}

ubyte2 File::getWord(ubyte8 pos) // only a helper function for the virtual machine
{
	converter con;
	con.UBYTE1[0] = this->getByte(pos);
	con.UBYTE1[1] = this->getByte(pos + 1);

	return con.UBYTE2;
}

bool File::openFile(VFI* file) // VFI keeps data of the requested file. which offset and on which device (see lba)
{
	if (!this->rawBlockDeviceAccessor || !file)
		return false;

	if (!file->fileType == ardunix::filesystem::filetype::file) // TODO: allow devices
		return false;

	this->file = file;

	if (rawBlockDeviceAccessor->getDeviceByType(file->deviceID))
		return true;

	return false;


	/* only for devices:
	if (!this->rawBlockDeviceAccessor->getDeviceByType(this->file->deviceID))	// check wheter this file is a block device
	{
		this->rawBlockDeviceAccessor = 0; // device not found? delete rbda ! TODO: WHY DELETE?!??!! are you CRAZY? it's like you crank the door handle up only because someone forgot his keys once lol
		return false;
	}

	return true;
	*/
}

ubyte1 File::getByte(ubyte8 pos)
{
	// TODO: support devices

	if (vfiManager)
	{
		return vfiManager->fileSystemManager.getFileByte(file, pos);
	}
	else
		return 'k'; // error handling?

	/* only for devices:
	if (this->rawBlockDeviceAccessor)
	{
		// TODO: THIS WILL NEVER WORK !!! if pos runs out of the filesystem's block for the file, we read from unkown places... we have to ask the filesystem here where to read!!
		// SO DO NOT OPEN A FILE FROM A FILESYSTEM (for now)!
		return this->rawBlockDeviceAccessor->getByte(file->deviceID, (pos + this->file->lba)); // relative position to eeprom file
	}
	return 0;
	*/
}

void File::setByte(int pos, ubyte1 value)
{
	// not implemented yet ..
	return;

	/*if (this->rawBlockDeviceAccessor)
		this->rawBlockDeviceAccessor->setByte(file->deviceID, (pos + this->file->lba), value);*/
}
