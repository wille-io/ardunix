#include "FileSystemManager.h"
#include "VFIManager.h"

FileSystemManager::FileSystemManager()
{
	rawBlockDeviceAccessor  = 0;
	//fat16FileSystemAccessor = 0;
	ardunixFileSystemAccessor = 0;

	eventController.registerEvent(ardunix::events::etGetRawBlockDeviceAccessor, this);
	eventController.fireEvent(ardunix::events::etGetRawBlockDeviceAccessor);

	eventController.registerEvent(ardunix::events::etFillVFIChildrenList, this);
	// no need to fire event for etFVCL
}

FileSystemManager::~FileSystemManager()
{
	//delete fat16FileSystemAccessor;
	//fat16FileSystemAccessor = 0;

	delete ardunixFileSystemAccessor;
	ardunixFileSystemAccessor = 0;

	rawBlockDeviceAccessor = 0;
}

void FileSystemManager::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetRawBlockDeviceAccessor:
		{
			if (ptr)
				rawBlockDeviceAccessor = (RawBlockDeviceAccessor*)ptr;
			return;
		}

		case ardunix::events::etFillVFIChildrenList:
		{
			if (ptr)
			{
				VFI* vfi = (VFI*)ptr; // get event's vfi
				vfi->setChildrenList(getChildrenListOfVFI(vfi));
			}
			else
			{
#ifdef DEBUG
				Serial.println(F("FSM: no pointer for etFVCL"));
#endif
			}
		}
	}
}

VFI* FileSystemManager::getVFI(ubyte1 deviceID, VFI* parentVFI, char* nodeName) // ONLY GET VFI FOR __NEXT__ VFI, NOT FOR ALL IN nodeName !!
{
	Serial.println("FSM: getVFI: DEPRECATED! DO NOT USE!");
	while(true){}
	return 0;


	/* folgendes muss passieren:
	 *
	 * getVFI muss nodeName strtoken, die VFIs dazu finden, bis es nicht mehr weiter geht mit deviceID VFI
	 * ab da muss das dateisystem gefragt werden und diesem muss die device ID übergeben werden.
	 * z.B. afsa.getVFI(parentVFI, nextNodeName) - wobei parentVFI das letzte verfügbare verzeichnis ist
	 * parentVFI ist IMMER das root-VFI vom gewollten Device. Weil die letzte dem VFIManager bekannten
	 * VFI das root-VFI vom gewollten Device ist :P the last device known to VFIManager
	 * und nextNodeName ist nur EIN EINZIGER node. undzwar der nächste gewollte. soll eine Datei "File"
	 * unter dem root-VFI des gewollten Device liegen, wird dieses dort gesucht und zurückgegeben.
	 * hat nodeName noch mehr nodes, wird dieser vorgang solange wiederholt, bis es kein nodeName mehr gibt
	 * (erfolgreich, gefunden) oder bis keine Datei unter nextNodeName gefunden wurde (fehler, nicht gefunden)
	 * wobei sofort dieses getVFI ein ungültiges VFI zurückgibt (0) */

	// look if parentVFI has the children we are looking for






	// VFI is not in childrenList, lookup. << THIS is not possible. if not in childrenlist, then it does not exist
	return 0; //ardunixFileSystemAccessor->getVFI(parentVFI, "testfile");





	/*
	VFI* currentParent = parentVFI;
	char* nextNodeName = strtok(nodeName, ArdunixHeader::slashstring);

	while(nextNodeName)
	{
		switch (deviceID)
		{
			case ardunix::filesystem::devicetype::sdcard:
			{
				if (!ardunixFileSystemAccessor)
				{
					Serial.println(F("adxFS not initialized !"));
					return 0; //ardunixFileSystemAccessor = new ArdunixFileSystemAccessor(); - udev has to initialize all devices
				}
				else
				{
					currentParent = ardunixFileSystemAccessor->getVFI(currentParent, nextNodeName);
					if (!currentParent) // filesystem returns invalid VFI (0) if node not found! so return 0 also
						return 0;
				}

				break;
			}
		}

		nextNodeName = strtok(nodeName, NULL); // get next node name
	} // no next node, should be the VFI the users was looking for, return that VFI

	return currentParent;*/
}

List<VFI*>* FileSystemManager::getListOfNonVFIPathOfNode(ubyte1 deviceID, char* path) // deprecated
{
	Serial.println("FSM: getListOfNonVFIPathOfNode: DEPRECATED! DO NOT USE!");
	while(true){}
	/*  DO NOT USE FOR NOW
	 *
	 * was i drunk defining this method?

	//                          13  >   1,3 -          13    >  1     > 0,3     3   (partition 3)
	float partition = ((((float)deviceID) / 10) - ((byte)deviceID) / 10) * 10; // UGLY, but the way easiest way to do it
	//int partition_type_position = 446 + 4 + (partition * 16); // 446 = first partition info position > add 4 to get to partition type (1 byte)

	// TODO: delete this debug line:
	int partition_type_position = 0;

	ubyte1 filesystem_type = rawBlockDeviceAccessor->getByte(deviceID, partition_type_position); // get filesystem type

	switch (filesystem_type)
	{
		//case 6: return fat16FileSystemAccessor->getListOfPath(path);
		case ardunix::filesystem::filesystemtype::adxFS:
		{
			if (!ardunixFileSystemAccessor)
				return 0;
			else
				return ardunixFileSystemAccessor->getListOfPath(path);
		}
      default: return 0;
	}

	*/

   return 0; // to cover up compiler warnings (switch does return 0 by default)
}

List<VFI*>* FileSystemManager::getChildrenListOfVFI(VFI* vfi)
{
	if (!vfi)
		return 0;

	switch(vfi->fileSystemType)
	{
		case ardunix::filesystem::filesystemtype::adxFS:
		{
			if (ardunixFileSystemAccessor)
				return ardunixFileSystemAccessor->getChildrenListOfVFI(vfi);
		}
	}

	return 0;
}

ubyte1 FileSystemManager::getFileByte(VFI* vfiOfFile, ubyte8 &pos)
{	
	if (!vfiOfFile)
		return 0; // error handling?

	switch (vfiOfFile->fileSystemType)
	{
		case ardunix::filesystem::filesystemtype::adxFS:
		{
			if (ardunixFileSystemAccessor)
				return ardunixFileSystemAccessor->getFileByte(vfiOfFile, (ubyte8&)pos); // TODO: ubyte1
		}

		//default: return 0; // do not read from other formats, and NOT from VFI
	}

	return 0; // TODO: set last error (class should receive errors via events?!)
}

void FileSystemManager::setFileByte(VFI* vfiOfFile, ubyte2 &pos, ubyte1 &value)
{
	return; // not implemented yet
}

VFI* FileSystemManager::initializeFilesystem(ubyte1 fileSystemType, VFI* deviceVFI)//ubyte1 deviceID) // initializes the filesystem and returns the vfi for the filesystems root directory
{
	switch(fileSystemType)
	{
		case ardunix::filesystem::filesystemtype::adxFS:
		{
			if (!ardunixFileSystemAccessor)
			{
				ardunixFileSystemAccessor = new ArdunixFileSystemAccessor; // create a new AFSA
				if (ardunixFileSystemAccessor->initPartition(deviceVFI)) // is a vfi necessary?
				{
					VFI* rootVFI = ardunixFileSystemAccessor->getRootVFI(deviceVFI->deviceID);
					rootVFI->nodeName = deviceVFI->nodeName; // we have to replace the nodeName, as the nodename is "/", which is illegal. change it to coresponding deviceID name. e.g. "sdcard"
					return rootVFI;
				}
				else
				{
					delete ardunixFileSystemAccessor;
					ardunixFileSystemAccessor = 0;
					return 0;
				}
			} // do not give back a vfi if already initialized
		}
	}

	return 0;
}
