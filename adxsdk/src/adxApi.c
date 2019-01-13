#include "../include/adxApi.h"


void API_printString(const char* string)
{
	union converter c;

	ubyte1* api_ref = 0; // to point outside the array
	api_ref[_api_cmd] = PRINT_STRING; // api_cmd = Print String
	c.UBYTE2 = (ubyte2)string; // to fill api_ref with low & high byte
	api_ref[_api_ptr_lo] = c.UBYTE1[0]; // api_ptr_lo
	api_ref[_api_ptr_hi] = c.UBYTE1[1]; // api_ptr_hi
	asm("sei \n\t"); // tell vm we want a api call (hacky, we replaced sei with out api routine, as we don't need cli&sei)
}

ubyte2 API_bytesAvailable()
{
	ubyte1* api_ref = 0;
	api_ref[_api_cmd] = STDIN_AVAIL;
	asm("sei \n\t"); // api call, wait until answer is in api_ptr

	union converter c;
	c.UBYTE1[0] = api_ref[_api_ptr_lo];
	c.UBYTE1[1] = api_ref[_api_ptr_hi];

	return c.UBYTE2;
}

char API_getChar()
{
	ubyte1* api_ref = 0;
	api_ref[_api_cmd] = STDIN_GET;
	asm("sei \n\t"); // api call, wait until answer is in api_ptr

	return api_ref[_api_ptr_lo];
}
