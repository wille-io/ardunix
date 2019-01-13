#include "VFIManager.h"
#include "EventController.h"
#include "RawBlockDeviceAccessor.h"


VFIManager::VFIManager()
{

	this->buildVFI();
}

VFIManager::~VFIManager() // in hope that we can delete every VFI
{
	eventController.fireEvent(ardunix::events::etStdOutFlashLine, ((void*)F("VFIManager: deleting virtual file index")));
	// root is automatically deleted
}

List<VFI*>* VFIManager::getListOfNode(VFI* vfi)
{
	return fileSystemManager.getListOfNonVFIPathOfNode(vfi->deviceID, (char*)getNonVFIPathOfNode(vfi).c_str()); // this will read vfi->deviceID to determine which device to ask - this will read the filesystem of the device(/partition) - this will read the Children Of The Path !
}

ubyte1 VFIManager::getChildrenNumOfVFI(VFI* vfi) // refresh the children list
{
	ubyte1 count = 0;

	if (vfi)
	{
		if (vfi->deviceID == ardunix::filesystem::devicetype::VFI)
			count = vfi->getChildrenList()->count(); // no need to get list of node, because VFI is always filled out
		else
		{
			vfi->setChildrenList(this->getListOfNode(vfi));
			if (vfi->getChildrenList())
				count = vfi->getChildrenList()->count();
		}
	}

	return count;
}

void VFIManager::addChildren(VFI* source, VFI* destination) // insert a children into a parent [THIS IS THE NEW CHILDREN LIST STUFF!!]
{
	if (source && destination) // only do this if both exist
	{
		if (!destination->getChildrenList()) // if there is no childrenList, create it first
			destination->setChildrenList(new List<VFI*>());
		destination->getChildrenList()->appendEntry(source); // add void-pointer of source to children list of destination
	}
}

VFI* VFIManager::newVFI(ubyte1 deviceID, char* nodeName, ubyte1 fileType, ubyte1 fileSystemType, VFI* parent, int lba) // insert a NEW children into a parent
{
	VFI* index = new VFI;
	index->deviceID = deviceID;
	index->nodeName = (char*)nodeName;
	index->fileType = fileType;
	index->fileSystemType = fileSystemType;
	index->lba = lba;
	index->parent = parent;

	this->addChildren(index, parent); // add the new children into the parent

	return index; // return for testing purpose
}

String VFIManager::getNonVFIPathOfNode(VFI* index)
{
	String path = "";
	String temp = "";

	if (!index)
		return path;

	while (index->parent && !index->deviceID)//ardunix::filesystem::devicetype::vfi)) // as long as there is a parent and is not of type VFI
	{
		temp = ArdunixHeader::slashstring;
		temp += index->nodeName;
		path = temp + path;

		index = index->parent; // next
	}

	return path;
}

String VFIManager::getPathOfNode(VFI* index)
{
	String path = "";
	String temp = "";

	if (!index)
		return path;

	while (index->parent != 0) // solange er einen parent hat
	{
		temp = ArdunixHeader::slashstring;
		temp += index->nodeName;
		path = temp + path;

		index = index->parent; // nextclass
	}

	return path;
}

VFI* VFIManager::parsePath(char* path, VFI* wdir) // returns the index of the given path (can be a file or directory) PLEASE do not feed with bullshit or else...!
{
	char* temp = 0;
	char* new_path;
	VFI* index = wdir; // use working directory, but not if it begins with '/'
   bool found = false;

	temp = strtok(path, ArdunixHeader::delimiter);
   if (temp)
		new_path = &path[strlen(temp)]; // set new path by going forward in the string

	if (!temp && path[0] != ArdunixHeader::slashchar) // if requested path is NOT / (root)
		return 0; // if temp has NO result then return invalid index
	else
		if (!temp && path[0] == ArdunixHeader::slashchar)
			return &this->root;

	if (path[0] == ArdunixHeader::slashchar) // if the path starts with a '/', then set index to root first
		index = &this->root;

	while(temp) // Dokumentation: wir nutzen kein != oder != NULL oder so, der Uebersicht und leserlichkeit halber
	{
		found = false;

		if (!strcmp_P(temp, PSTR("..")) && index->parent) // if a node is described by "..", then take the index's parent, if there is one
		{
			index = index->parent;
			found = true;
		}
		else
		{
			if (index->deviceID == ardunix::filesystem::devicetype::VFI)
			{
				VFI* currentVFI = 0;

				if (index)
					if (index->getChildrenList())
						currentVFI = index->getChildrenList()->getFirstEntry();

				while (currentVFI) // search in parent VFI for the given name // AENDERUNG: ALTES SYSTEM (for) ERSETZT MIT NEUEM SYSTEM MIT while !!
				{
					if (!strcmp(currentVFI->nodeName, temp))  // compare VFI node names
					{
						index = currentVFI; // we found the desired path > return it!;
						found = true;
					}

					currentVFI = index->getChildrenList()->getNextEntry();
				}
			}
			else // read from an external file system, because this file is not inside the VFI file system anymore
			{
				// WE HAVE TO EXIT THE WHILE CLAUSE HERE, lookupChildren IS ALREADY RECURSIVE !! SO RETURN !

				VFI* new_index = index->lookupChildren(temp); // old: (char*)getNonVFIPathOfNode(index).c_str()); // get VFI by device id and by the NON-VFI PATH of INDEX !
				if (new_index)
				{
					found = true;
					new_index->parent = index; // set parent. because of memory limitation we set the parent by ourselves
					return new_index; // the new index, return it, we are done, getVFI is recursive
				}
				else
				{
					return 0; // not found, return invalid index
				}
			}
		}

		if (!found) // if nothing was found (not by '..' or either the for-loop), then return invalid index (because one part of the path was not found)
			return 0; // return invalid index

		temp = strtok(NULL, ArdunixHeader::delimiter);
      if (temp)
		  new_path = &path[strlen(temp)]; // set new path by going forward in the string
	}

	if (found)
		return index;
	else
		return 0; // return invalid index if not found
}

VFI* VFIManager::initializeDevice(byte deviceID) // acts a little like udev
{
	char* deviceName = 0;

	switch (deviceID)
	{
		// TODO: use static names in adx header

		case ardunix::filesystem::devicetype::eeprom:
		{
			deviceName = (char*)"eeprom";
			break;
		}

		case ardunix::filesystem::devicetype::eeprom1:
		{
			deviceName = (char*)"eeprom1";
			break;
		}

		case ardunix::filesystem::devicetype::eeprom2:
		{
			deviceName = (char*)"eeprom2";
			break;
		}

		case ardunix::filesystem::devicetype::sdcard:
		{
			deviceName = (char*)"sdcard";
			break;
		}
	}

	if (!deviceName) // device not found
		return 0;



	return newVFI(deviceID, deviceName, ardunix::filesystem::filetype::device,
					  ardunix::filesystem::filesystemtype::VFI, dev);
}

void VFIManager::buildVFI() // TODO: udev !!
{
	// set root /
	this->root.deviceID = ardunix::filesystem::devicetype::VFI;
	this->root.nodeName = "/";//(char*)ArdunixHeader::slashstring;
	this->root.parent = 0; // will always be 0
	this->root.fileType = ardunix::filesystem::filetype::directory;

	// TODO: decouple	all VFIs, add something like "mkdir" to the vfi system
	// TODO: "mnt" and "dev" as static string in header OR as flash string ???
	mnt = newVFI(ardunix::filesystem::devicetype::VFI, (char*)"mnt", ardunix::filesystem::filetype::directory,
					 ardunix::filesystem::filesystemtype::VFI, &this->root);


	// set dev
	dev = newVFI(ardunix::filesystem::devicetype::VFI, (char*)"dev", ardunix::filesystem::filetype::directory,
					 ardunix::filesystem::filesystemtype::VFI, &this->root);
	/*	VFI* eep = newVFI(ardunix::filesystem::devicetype::eeprom, (char*)"eeprom", ardunix::filesystem::filetype::device, dev);
		VFI* eep1 = newVFI(ardunix::filesystem::devicetype::eeprom1, (char*)"eeprom1", ardunix::filesystem::filetype::device, dev, 0); // 64 ubyte1 space for config data
		VFI* eep2 = newVFI(ardunix::filesystem::devicetype::eeprom2, (char*)"eeprom2", ardunix::filesystem::filetype::device, dev, 64); // (512-64) ubyte1 space for user data (assemblers, etc)
		VFI* sdcard = newVFI(ardunix::filesystem::devicetype::sdcard, (char*)"sdcard", ardunix::filesystem::filetype::device, dev, 0);
	*/

	for (int i = 0; i < 100; i++) // TODO: do not use a fixed value
	{
		initializeDevice(i);
	}
}
