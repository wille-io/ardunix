#include "VMManager.h"


VMManager::VMManager()
{
	this->nextProcessID = 1; // 0 = invalid
	this->virtualMachineList = 0;

	eventController.registerEvent(ardunix::events::etGetVMManager, this);
}

VMManager::~VMManager()
{

}

void VMManager::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetVMManager:
		{
			if (!ptr) // send this class
			{
				eventController.fireEvent(ardunix::events::etGetVMManager, (void*)this);
			}
			return;
		}
	}
}

void VMManager::tick()
{
	VMProcess* vmProcess = 0;
	// step every virtual machine in the virtual machine list
	if (virtualMachineList)
	{
		vmProcess = virtualMachineList->getFirstEntry();
	}

	while(vmProcess && vmProcess->vm)
	{
		vmProcess->vm->cpustep();
		vmProcess = virtualMachineList->getNextEntry();
	}
}

void VMManager::addVirtualMachine(VirtualMachine *vm, ubyte1 stdIOVMID) // stdIOVMID deprecated !!
{
	if (!this->virtualMachineList) // if there is no list, create it
	{
		this->virtualMachineList = new List<VMProcess*>();
	}

	VMProcess* vmProcess = 0;
	vmProcess = new VMProcess;
	vmProcess->vm = vm;
	vmProcess->vmID = 1; // nextProcessID++;
	vmProcess->stdIOVMID = stdIOVMID; // which interpreter process started this? 0 if no interpreter (redirects to the only (non vm)-interpreter)
	vmProcess->isInterpreter = false; // probably not..

	this->virtualMachineList->appendEntry(vmProcess); // add virtual machine to virtual machine - list

	eventController.fireEvent(ardunix::events::etSetInterpreterActiveVMProcess, (void*)vmProcess);
}

VirtualMachine* VMManager::getVirtualMachine(ubyte1 vmID)
{
	if (!this->virtualMachineList) // IMPOSSIBLE, because there MUST be an VM, because nobody else can call this event though
	{
		return 0;
	}

	VMProcess* vmProcess = virtualMachineList->getFirstEntry();

	while (vmProcess)
	{
		if (vmProcess->vmID == vmID) // this is the VM we are looking for
		{
			if (vmProcess->vm)
			{
				return vmProcess->vm;
			}
		}

		vmProcess = virtualMachineList->getNextEntry();
	}

	return 0;
}

ubyte1 VMManager::getVirtualMachineID(VirtualMachine* vm)
{
	Serial.println(F("DO NOT USE getVirtualMachineID!"));
	while(true){}
	return 0;
}

// TODO: kVM 2x and not shared !
bool VMManager::killVirtualMachine(ubyte1 vmID)
{
	if (!this->virtualMachineList) // IMPOSSIBLE, because there MUST be an VM, because nobody else can call this event though
		return false;

	VMProcess* vmProcess = virtualMachineList->getFirstEntry();

	while (vmProcess)
	{
		if (vmProcess->vmID == vmID) // this is the VM we are looking for
		{
			killVirtualMachine(vmProcess->vm);
		}

		vmProcess = virtualMachineList->getNextEntry();
	}

	return false;
}

bool VMManager::killVirtualMachine(VirtualMachine *vm)
{
	if (!this->virtualMachineList) // IMPOSSIBLE, because there MUST be an VM, because nobody else can call this event though
		return false;

	VMProcess* vmProcess = virtualMachineList->getFirstEntry();

	while (vmProcess)
	{
		if (vmProcess->vm == vm) // this is the VM we are looking for
		{
			virtualMachineList->deleteEntry(vmProcess); // delete vm (TODO: this will NOT free memory !!!)
			if (vmProcess->vm)
				delete vmProcess->vm; // del vm
			delete vmProcess; // del proc
			eventController.fireEvent(ardunix::events::etSOFL, (void*)F("\r\nVM exited!"));
			return true;
		}

		vmProcess = virtualMachineList->getNextEntry();
	}

	return false;
}
