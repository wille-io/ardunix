#ifndef KERNEL_H				
#define KERNEL_H

#include "ardunix.h"
#include "Interpreter.h"
#include "Console.h"
#include "VFIManager.h"
#include "RawCharDeviceAccessor.h"
#include "RawBlockDeviceAccessor.h"
#include "VMManager.h"
#include "API.h"


// this class represents the main component which manages all children classes in ardunix
class Kernel : AbstractEventClass
{
	public:
		Kernel();
		~Kernel();

		void tick();
		void handleEvent(ubyte1 eventType, void* ptr);
		void initAndRegisterConsole();
		void setUARTAsStandardIO();
		void panic(char* msg); // classic :)

		Interpreter* interpreter;
		VMManager*   vmManager;

	private:
		Console*     console;
		VFIManager*  vfiManager;
		API*         api;		

		RawCharDeviceAccessor*  rawCharDeviceAccessor;
		RawBlockDeviceAccessor* rawBlockDeviceAccessor;

		ubyte1 standardConsoleDeviceType;
};


#endif
