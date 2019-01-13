#include "VFI.h"
#include "EventController.h" // don't make this class an abstacrt event class as this would cost a load of ram

void VFI::reset() // to initialize all variables with 0 - because every computer starts with initialized cpu
{
	deviceID = 0;
	nodeName = 0;
	fileSize = 0;
	fileType = 0;
	lba = 0;
	parent = 0;
	childrenList = 0;
	fileSystemType = 0;
	iNode = 0;
}

VFI::~VFI()
{
	//delete this->nodeName; // never do this ! most nodenames do NOT bolong to the VFI
	delete this->childrenList; // deletes all children (and their under-children) automatically
	this->reset(); // finally initialize every variable with 0 (safety issues)
}

VFI::VFI()
{
	this->reset();
}

void VFI::addChild(VFI* child)
{
	if (!childrenList)
	{
		childrenList = new List<VFI*>();
	}

	childrenList->appendEntry(child);
}

VFI* VFI::lookupChildren(char* nodename)
{
	if (childrenList) // only lookup if there are children, or bad crash
	{
		VFI* lookupVFI = childrenList->getFirstEntry();

		while (lookupVFI)
		{
			//Serial.print("lookupChildren: lookup nodename = '"); Serial.print(nodename); Serial.print("'  current vfi nodename = '"); Serial.print(lookupVFI->nodeName); Serial.println("'");
			int s = strcmp(lookupVFI->nodeName, nodename);
			//Serial.print("strcmp = "); Serial.println((int)s);
			if (!s)
			{
				//Serial.println("FOUND!");
				return lookupVFI;
			}
			/*else
				Serial.println("not found -.- !");*/

			lookupVFI = childrenList->getNextEntry();
		}
	}

	// node not found
	return 0;
}

List<VFI*>* VFI::getChildrenList()
{
	if (!childrenList && fileType == ardunix::filesystem::filetype::directory) // get childrenlist if list = 0 and is a directory
	{
		eventController.fireEvent(ardunix::events::etFillVFIChildrenList, (void*)this); // fill this VFI's children list
	}

	return childrenList;
}

void VFI::setChildrenList(List<VFI*>* childrenList)
{
	this->childrenList = childrenList;
}
