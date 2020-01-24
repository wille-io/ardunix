#include "Kernel.h"
#include "EventController.h"

void Kernel::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etAddVirtualMachine:
			if (ptr)
			{
				/*if (!vmManager)
					vmManager = new VMManager(); // as kernel must manage vm's*/

#warning for now 0, because there are no vm interpreters (only interpreter class)
				vmManager->addVirtualMachine((VirtualMachine*)ptr, 0); // add virtual machine to virtual machine - list
			}
			return;

		case ardunix::events::etExitVM: // the opcode manager, avr_op_RET is calling this
			if (ptr)
			{
				if (vmManager) // only if there is a manager already
					vmManager->killVirtualMachine((VirtualMachine*)ptr); // TODO: must interpreter get a message that a vm was killed or must interpreter see by itself? (if getVMByID == 0)
			}
			return;

		case ardunix::events::etGetRawCharDeviceAccessor:
			if (!ptr && rawCharDeviceAccessor) // only send if this event does NOT come from here (0 comes from other class)
				eventController.fireEvent(eventType, (void*)rawCharDeviceAccessor);
			return;

		case ardunix::events::etGetRawBlockDeviceAccessor:
			if (!ptr && rawBlockDeviceAccessor)
				eventController.fireEvent(eventType, (void*)rawBlockDeviceAccessor);
			return;

		case ardunix::events::etGetStandardConsoleDeviceType:
			if (!ptr && standardConsoleDeviceType) // only send if this event does NOT come from here (0 comes from other class)
				eventController.fireEvent(eventType, (void*)&standardConsoleDeviceType); // do not send the value as void-pointer - send the pointer of the value instead !!
			return;

		case ardunix::events::etGetConsole:
			if (!ptr && console) // only send if this event does NOT come from here (0 comes from other class)
				eventController.fireEvent(eventType, (void*)console);
			return;

		case ardunix::events::etGetVFIManager:
			if (!ptr && vfiManager)
				eventController.fireEvent(eventType, (void*)vfiManager);
			return;

		case ardunix::events::etRestart:
			console->PrintLN(F("etRestart.."));
			delete this;
#ifdef ARDUINO
			asm volatile ("JMP 0"); // this opcode does not work on machines with windows operating system and similar
#else
			exit(0); // too time consuming to restart an application layered on another os - so just exit
#endif
			return;
	}
}

void Kernel::initAndRegisterConsole()
{
	eventController.registerEvent(ardunix::events::etGetRawCharDeviceAccessor, this);
	eventController.registerEvent(ardunix::events::etGetRawBlockDeviceAccessor, this);
	eventController.registerEvent(ardunix::events::etGetStandardConsoleDeviceType, this);
	eventController.registerEvent(ardunix::events::etRestart, this);
	eventController.registerEvent(ardunix::events::etGetVFIManager, this);
	eventController.registerEvent(ardunix::events::etGetConsole, this);
	eventController.registerEvent(ardunix::events::etAddVirtualMachine, this);
	eventController.registerEvent(ardunix::events::etExitVM, this);

	this->console = new Console();
}

void Kernel::setUARTAsStandardIO() // if there is no output device specified, set UART as the standard (input and) output device
{
	AbstractRawDevice* consoleDevice = rawCharDeviceAccessor->getDeviceByName(ArdunixHeader::uart_string);
	if (consoleDevice) // not a null-pointer? device found?
	{
		ubyte1 consoleDeviceType = consoleDevice->deviceType; // get deviceType from primitive device
		if (consoleDeviceType) // a invalid device ?
		{
			this->standardConsoleDeviceType = consoleDeviceType;
			consoleDevice->init(); // initialize device
			this->initAndRegisterConsole();
			return;
		}
	}
}

Kernel::~Kernel()
{
	console->PrintLN(F("Shutting down ardunix - Please wait ..."));

	delete this->rawBlockDeviceAccessor;
	//delete this->vfiManager;  reimplement!! would crash arduinx after something is mounted, as recursive deletes delete already deleted pointers. ignore deleting vfiManager at first
	delete this->interpreter;

	//panic("error #4D53");

	delete this->console;
	delete this->rawCharDeviceAccessor;
	delete this->api;	
}

void Kernel::tick()
{
	if (this->interpreter)
	{
		//Serial.write("P");
		this->interpreter->parseChar(); // step the interpreter (read one more character, if there is one in the input buffer)
	}

	if (this->vmManager)
	{
		this->vmManager->tick();
	}

	// i would say that the kernel reads the console - if there is one char, then parseChar @ interpreter and @ vmManager (just testing if this is a logical conclusion)
	// no, this does not work. imagine 20 vm's getting the char at once.. so the interpreter gives the char only to the active vm !
	/*if (this->console)
	{
		if (this->console->bytesAvailable())
		{
			char c = this->console->getByte();

			if (this->interpreter) // yes, there is only one, hardcoded interpreter. running a interpreter as a vm is dumb. cpu, ram, and why should you start more than one on a micro computer?!
			{
				this->interpreter->parseChar(c);
			}

			if (this->vmManager)
			{
				this->vmManager->parseChar(c);
			}
		}
	}*/
}

Kernel::Kernel()
{
	this->vmManager = 0;
	this->interpreter = 0;
	this->console = 0;
	this->vfiManager = 0;
	this->standardConsoleDeviceType = 0;
	this->api = 0;

#warning DELETE:
	this->rawCharDeviceAccessor  = new RawCharDeviceAccessor();

	// >>> GETTING STANDARD CONSOLE OUTPUT CHAR DEVICE & INITIALIZE Console CLASS
	//
	// read, which output to use to output informations a.s.o - no RawDeviceAccessor necessary for this action
	this->standardConsoleDeviceType = 0; // set invalid

	if (EEPROM.read(0) == 'c') // first address has a 'c' as char (c means console)
	{
		ubyte1             consoleDeviceType = 0;
		AbstractRawDevice* consoleDevice     = 0;

		char* consoleDeviceName = (char*)malloc(33); // maximum length of device name is 32 + \0
		memset(consoleDeviceName, 0, 33); // set every ubyte1 to '\0', so there is no need to add a '\0' at the end of the string

		for (ubyte2 i = 2; i < 32; i++) // start reading from adr 2 ("..c=....")
		{
			char c = EEPROM.read(i);
			if (c == ';') // end of string
			{
				consoleDevice = this->rawCharDeviceAccessor->getDeviceByName(consoleDeviceName);
				delete consoleDeviceName; // this function has the scope of this string, it is responsible to delete it

				if (consoleDevice) // not a null-pointer?
				{
					consoleDeviceType = consoleDevice->deviceType; // get deviceType from primitive device
					if (consoleDeviceType) // not a invalid device ?
					{
						this->standardConsoleDeviceType = consoleDeviceType;
						consoleDevice->init(); // initialize device
						this->initAndRegisterConsole();
						break; // init kernel further now
					}
					else
						this->setUARTAsStandardIO(); // device type is not implemented in ardunix
				}
				else
					this->setUARTAsStandardIO(); // device is not implemented in ardunix
			}
			else
				consoleDeviceName[i-2] = c; // add character to string
		}
	}
	else
		this->setUARTAsStandardIO(); // no 'c' in eeprom

	if (!console)
		return;

	console->PrintLN(F("          \rardunix.org open-source operating system\r\n"));

	rawBlockDeviceAccessor = new RawBlockDeviceAccessor(); // initialize all block devices now
	vfiManager  = new VFIManager();
	vmManager   = new VMManager(); // must be created, so interpreter can use it....
	interpreter = new Interpreter(&vfiManager->root); // root as workingDirectory
	api = new API();
}

void Kernel::panic(char* msg)
{
	console->Print(F("Kernel panic: "));
	console->PrintLN(msg);

	while(true){}
}
