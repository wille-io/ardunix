#ifndef CONSOLE_H		
#define CONSOLE_H

#include "ardunix.h"
#include "AbstractEventClass.h"
#include "RawCharDeviceAccessor.h"

// this class represents the bridge between ardunix and the enduser - console input by keyboard and output to display
class Console : AbstractEventClass
{
	public:
		RawCharDeviceAccessor* rawCharDeviceAccessor;
		byte* standardConsoleDeviceType;

		~Console();
		Console();

		ubyte1 getByte();
		ubyte1 bytesAvailable();
		void handleEvent(ubyte1 eventType, void* ptr);
      void Print(char c);
		void PrintLN(void);
		void Print(sbyte2 n, ubyte1 base);
		void PrintNumber(ubyte2 n, ubyte1 base = 10);
		void PrintLN(int n, ubyte1 base=10);
		void Print(const __FlashStringHelper*ifsh);
		void PrintLN(const __FlashStringHelper*ifsh);
      void Print(char *s);
      void PrintLN(char *s);
};

#endif
