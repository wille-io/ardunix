#pragma once

#include "adx.h"


union converter
{
	unsigned char  UBYTE1[2];
	unsigned short UBYTE2;
};

#ifdef __cplusplus
namespace ardunix
{
  namespace events
  {
    namespace api
    {
#endif
      enum API_events
      {
        INVALID = 0,
        PRINT_STRING = 1,
        STDIN_AVAIL = 2,
        STDIN_GET = 3
      };
#ifdef __cplusplus
    }
  }
}
#endif


#define _api_cmd 2303 // sram addr, laps over, goes into api (hacky, but works to avoid asm / inline asm)
#define _api_ptr_lo 2304
#define _api_ptr_hi 2305

void API_printString(const char* string);
ubyte2 API_bytesAvailable();
char API_getChar();
