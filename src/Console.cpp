#include "ardunix.h" // for definition ARDUNIX
#include "Console.h"
#include "EventController.h"

#warning Soon to be replaced by stdIO

Console::Console()
{
	this->rawCharDeviceAccessor = 0;
	this->standardConsoleDeviceType = 0; // set POINTER to 0

	// values from other classes for this class
	// getter
	eventController.registerEvent(ardunix::events::etGetRawCharDeviceAccessor, this);
	eventController.fireEvent(ardunix::events::etGetRawCharDeviceAccessor, 0); // null-pointer - because this class needs a RCDA
	eventController.registerEvent(ardunix::events::etGetStandardConsoleDeviceType, this);
	eventController.fireEvent(ardunix::events::etGetStandardConsoleDeviceType, 0); // null-pointer - because this class needs a SCDT

	// events
	eventController.registerEvent(ardunix::events::etStdOutFlashLine, this);
	eventController.registerEvent(ardunix::events::etStdOutFlash, this);
	eventController.registerEvent(ardunix::events::etStdOutStringLine, this);
	eventController.registerEvent(ardunix::events::etStdOutNumberLine, this);

	// values to other classes from this class
	eventController.registerEvent(ardunix::events::etGetConsole, this);
}

Console::~Console()
{
	if (this->standardConsoleDeviceType)
		*this->standardConsoleDeviceType = 0; // set ptr's value to 0
}

ubyte1 Console::getByte()
{
	return rawCharDeviceAccessor->receiveByte(*standardConsoleDeviceType);
}

ubyte1 Console::bytesAvailable()
{
	return rawCharDeviceAccessor->bytesAvailable(*standardConsoleDeviceType);
}

void Console::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetRawCharDeviceAccessor:
			if (ptr)
				this->rawCharDeviceAccessor = (RawCharDeviceAccessor*)ptr;
			return;
		case ardunix::events::etGetStandardConsoleDeviceType:
			if (ptr)
				this->standardConsoleDeviceType = (byte*)ptr;
			return;
		case ardunix::events::etGetConsole:
			if (!ptr) // '!ptr' because we send a pointer and not getting one
				eventController.fireEvent(eventType, (void*)this);
			return;
		case ardunix::events::etStdOutFlashLine:
			if (ptr)
				PrintLN((__FlashStringHelper*) ptr);
			return;
		case ardunix::events::etStdOutFlash:
		{
			if (ptr)
			{
				Print((__FlashStringHelper*) ptr);
			}
			return;
		}
		case ardunix::events::etStdOutStringLine:
		{
			if (ptr)
			{
				char* string = (char*)ptr;
				PrintLN(string);
			}
			return;
		}
		case ardunix::events::etStdOutNumberLine:
			if (ptr)
			{
				sbyte2* temp = (sbyte2*)ptr;
				PrintLN(*temp, 10);
			}
			return;
	}
}

void Console::Print(char c)
{
	if (!this->rawCharDeviceAccessor || !this->standardConsoleDeviceType)
		return; // do not print something

	if ((BUS_SIZED_PTR)(void*)rawCharDeviceAccessor && (BUS_SIZED_PTR)(void*)standardConsoleDeviceType)
		rawCharDeviceAccessor->transmitByte(*standardConsoleDeviceType, c);
}

void Console::PrintLN(void)
{
	Print(F("\r\n"));
}

void Console::Print(sbyte2 n, ubyte1 base)
{
	if (!this->rawCharDeviceAccessor || !this->standardConsoleDeviceType)
		return; // do not print something

	if (base == 0) {
		//write(n);
	} else if (base == 10) {
		if (n < 0) {
			Print('-');
			n = -n;
			PrintNumber(n, 10);
		}
		PrintNumber(n, 10);
	} else {
		PrintNumber(n, base);
	}
}

void Console::PrintNumber(ubyte2 n, ubyte1 base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0'; // memset?

	// prevent crash if called with base == 1
	if (base < 2)
		base = 10;

	do
	{
		unsigned long m = n;
		n /= base;
		char c = m - base * n;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}
	while(n);

	Print(str);
}

void Console::PrintLN(int n, ubyte1 base)
{
	Print(n, base);
	PrintLN();
}

void Console::Print(const __FlashStringHelper * ifsh)
{
	if (!this->rawCharDeviceAccessor || !this->standardConsoleDeviceType)
		return; // do not print something

#ifdef ARDUINO
	char * __attribute__((progmem)) p = (char * ) ifsh;
	//size_t n = 0;
	while (1)
	{
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;
		Print(c);
	}
#else
	std::cerr << (ifsh);
#endif
}

void Console::PrintLN(const __FlashStringHelper * ifsh)
{
	Print(ifsh);
	PrintLN();
}

void Console::Print(char* s)
{
	if (!this->rawCharDeviceAccessor || !this->standardConsoleDeviceType)
		return; // do not print something

	for (ubyte2 i = 0; i < strlen(s); i++)
	{
		Print(s[i]);
	}
}

void Console::PrintLN(char* s)
{
	Print(s);
	PrintLN();
}
