#include <adxApi.h>
#include "API.h"
#include "EventController.h"

#define DEBUG

API::API()
{
	this->console = 0;

	// getter
	eventController.registerEvent(ardunix::events::etGetConsole, this);
	eventController.fireEvent(ardunix::events::etGetConsole);
	eventController.registerEvent(ardunix::events::etGetVMManager, this);
	eventController.fireEvent(ardunix::events::etGetVMManager);

	// TEMP
	/*eventController.registerEvent(ardunix::events::etGetInterpreter, this); // temp, to communicate with interpreter class (as long as it is not a vm)
	eventController.fireEvent(ardunix::events::etGetInterpreter);*/

	// events
	eventController.registerEvent(ardunix::events::etAPI, this);
}

void API::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetConsole:
			if (ptr && !this->console)
				this->console = (Console*)ptr;
			return;
		case ardunix::events::etGetVMManager:
			if (ptr && !this->vmManager)
				this->vmManager = (VMManager*)ptr;
			return;
		/*case ardunix::events::etGetInterpreter: // TEMP !!!!
		{
			if (ptr && !this->interpreter)
			{
				this->interpreter = (Interpreter*)ptr;
			}
			return;
		}*/
		case ardunix::events::etAPI:
		{
			if (ptr)
			{
				this->vmAPI((VirtualMachine*)ptr);
			}
			return;
		}
		default:
		{
			if (this->console)
			{
				console->PrintLN(F("API received invalid event"));
			}
			return;
		}
	}
}

//#error api must be ubyte2, as the address of the string ist > 255 (exactly 256, which is really really nasty :D)

void API::vmAPI(VirtualMachine* vm)
{
	switch(vm->cpu->api_cmd)
	{
		case ardunix::events::api::PRINT_STRING: // Print String
		{
			if (!vm->cpu->api_adr)
			{
				if (this->console)
					this->console->PrintLN(F("API call without valid value! abort."));
				return;
			}

			ubyte2 realAddress = vm->cpu->api_adr;

#ifdef DEBUG
			Serial.print(F("api_adr = ")); Serial.println(realAddress);

			Serial.println(F("Searching for 'J' in sram -.-"));

			ubyte1* x = (ubyte1*)&vm->cpu->sram;
			for (int i = 0; i < 0x08FF; i++)
			{
				if (x[i] == 'J')
				{
					Serial.print(F("Found 'J' @ "));
					Serial.print((int)i);
				}
			}
#endif

			char* stringPos = (char*)&vm->cpu->sram[realAddress]; // get pointer by value from sram from value from api[2] ... hahah
			if (this->console)
				this->console->PrintLN(stringPos);

			return;
		}

		case ardunix::events::api::STDIN_AVAIL: // StdIn Available
		{
			if (this->console)
			{
				vm->cpu->api_adr = this->console->bytesAvailable();
			}
			else
			{
#ifdef DEBUG
				Serial.println("No console for StdIn!");
#endif
			}

			return;
		}

		case ardunix::events::api::STDIN_GET: // StdIn Get Char
		{
			if (this->console)
			{
				vm->cpu->api_adr = (ubyte2)this->console->getByte();
			}
			else
			{
#ifdef DEBUG
				Serial.println("No console for StdIn!");
#endif
			}
		}

		default: // invalid
		{
			if (this->console)
			{
				this->console->PrintLN(F("API: Invalid API call #"));
				this->console->PrintLN(vm->cpu->api_cmd);
			}
			return;
		}
	}

	return;
}

void API::Print(char* print) // prints the text in 'print' until 'length'
{
	if (this->console)
		console->PrintLN(print); // print the string
	//else
		//?????
	//delete print; // done, clean-up    NEVER EVER DO THIS, ARDUNIX HAS NOT THE SCOPE ON THE VIRTUAL stuff
}
