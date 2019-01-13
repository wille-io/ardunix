#pragma once

#include "AbstractEventClass.h"
#include "Console.h"
#include "VirtualMachine.h"
#include "VMManager.h"
#include "Interpreter.h"


// this class represents the bridge between a virtual machine and the operating system
class API : public AbstractEventClass
{
	public:
		enum apiType // which command for a api
		{
			apiInvalid = 0x0,
			apiPrintln = 0x1
		};

		API();

		void handleEvent(ubyte1 eventType, void* ptr);

		void Print(char* print);
		void vmAPI(VirtualMachine* vm);

		Interpreter* interpreter;
		Console* console; // soon to be deprecated
		VMManager* vmManager;
};
