#pragma once

#include "VirtualMachine.h"


class VMManager : AbstractEventClass
{
	public:
		struct VMProcess
		{
			ubyte1 vmID;
			ubyte1 stdIOVMID;
			bool isInterpreter; // ONLY NEEDED AS LONG AS THE INTERPRETER IS STILL >NOT< A VM PROCESS !!
			VirtualMachine* vm;
		};

		VMManager();
		~VMManager();

		void handleEvent(ubyte1 eventType, void* ptr);

		void addVirtualMachine(VirtualMachine* vm, ubyte1 stdIOVMID);
		bool killVirtualMachine(ubyte1 vmID);
		bool killVirtualMachine(VirtualMachine* vm);
		VirtualMachine* getVirtualMachine(ubyte1 vmID);
		ubyte1 getVirtualMachineID(VirtualMachine* vm);
		void tick();

	//private:
		ubyte1 nextProcessID;
		List<VMProcess*>* virtualMachineList; // list which contains all running virtual machines [ I WANT THIS PRIVATE AND friend void loop();]
};
