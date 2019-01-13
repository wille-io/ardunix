// this is the main header file, which gives varibales and types, etc to many classes in ardunix
// tip: return a variable which is inited with 0, never return 0 for "cosmetic issues" because it is ugly

#ifndef ARDUINO
#define ARDUINO
#endif

#ifndef ARDUNIX_H
#define ARDUNIX_H

#ifdef ARDUINO
#include <Arduino.h>
#include <EEPROM.h>
#include <adxApi.h> // ardunix sdk header
#include "ArdunixHeader.h"
#define BUS_SIZED_PTR int // 16 bit on avr microcontrollers
#endif

namespace ardunix
{
	namespace events
	{
		enum // event types
		{
			etINVALID = 0,
			etRestart,
			etGetRawCharDeviceAccessor,
			etGetRawBlockDeviceAccessor,
			etGetStandardConsoleDeviceType,
			etGetConsole,
			etGetVFIManager,
			etStdOutFlashLine,
			etSOFL = etStdOutFlashLine,
			etStdOutNumberLine,
			etSONL = etStdOutNumberLine,
			etLogError,
			etAddVirtualMachine,
			etAPI,
			etExitVM,
			etFillVFIChildrenList,
			etAPIStdOut, // vmManager must find coherant stdout and send it to it
			etGetVMManager,
			etGetInterpreter,
			etSetInterpreterActiveVMProcess,
			etStdOutStringLine,
			etStdOutFlash
		};
	}

	namespace filesystem
	{
		namespace filesystemtype
		{
			enum
			{
				INVALID = 0,
				fat16 = 6,
				adxFS = 123,
				VFI = 200,
				RAW = 1
			};
		}

		namespace devicetype
		{
			enum
			{
				INVALID = 0,
				VFI = 10,
				eeprom = 20,
				eeprom1 = eeprom+1,
				eeprom2 = eeprom+2,
				flash = 30,
				sdcard = 40,
				sdcard1 = sdcard+1,
				sdcard2 = sdcard+2,
				disp = 50,
				uart = 60,
				test = 70
			};
		}

		namespace filetype
		{
			enum
			{
				INVALID = 0,
				file,
				directory,
				device,
			};
		}
	}
}

// helper variables, which have to be statically and globally defined, becaude many classes uses these varibales
/*static const char  nullchar    = '\0';
static const char  slashchar   = '/';
static const char* slashstring = "/";
static const char* spacestring = " ";
static const char* delimiter   = "/";

static const char* uart_string = "uart";
static const char* eeprom_string = "eeprom";*/

// type definitions for ardunix - to make ardunix consistent
typedef char               sbyte1;
typedef char               sbit8;

typedef unsigned char      ubyte1;
typedef unsigned char      ubit8;



typedef int                sbyte2;
typedef int                sbit16;

typedef unsigned int       ubyte2;
typedef unsigned int       ubit16;



typedef long               sbyte4;
typedef long               sbit32;

typedef unsigned long      ubyte4;
typedef unsigned long      ubit32;



typedef long long          sbyte8;
typedef long long          sbit64;

typedef unsigned long long ubyte8;
typedef unsigned long long ubit64;

/*union converter // this union converts from ubyte2 to ubyte1-array and the other way around
{
		ubyte2 UBYTE2;
		ubyte1 UBYTE1[2];
};*/



#ifndef ARDUINO

#include <string.h>
#include <string>
#include <iostream>
#include <malloc.h>

typedef unsigned char byte;

#define BUS_SIZED_PTR unsigned long long

class _EEPROM
{
	public:
		ubyte1 read(ubyte1 pos)
		{
			return 0;
		}
};

class _Serial
{
	public:
		void begin(int x){}

		void print(int& x)
		{
			std::cerr << x;
		}

		void print(char* x)
		{
			std::cerr << x;
		}

		void println(char* x)
		{
			print(x);
			print("\r\n");
		}

		void println(int x)
		{
			print(x);
			print("\r\n");
		}

		void end(){}

		ubyte1 read()
		{
			char c;
			std::cin >> c;
			if (c == '$')
				return 13; // return
			return c;
		}

		void write(char c)
		{
			std::cerr << c;
		}

		ubyte1 available()
		{
			return 1; // always 1, because read() is blocking
		}
};

//typedef bool boolean;   not necessary under windows operating system
typedef unsigned char byte;
typedef ubyte1 uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef std::string String;
typedef char* __FlashStringHelper;

static _Serial Serial;
static _EEPROM EEPROM;

inline char* F(char* x)
{
	return x;
}

inline char* PSTR(char* x)
{
	return x;
}

inline int strcmp_P(char* x, char* y)
{
	return strcmp(x, y);
}

inline void delay(int x)
{
	Sleep(x);
}

#ifndef ARDUINO
#include "windows_cxx_amd64_to_ardunix_simulator.h"
#endif

#endif // ARDUINO

#endif // ARDUNIX_H
