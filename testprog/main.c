#include <adxApi.h>


int main()
{
	asm("cli \n\t");
	asm("cli \n\t");
	asm("cli \n\t");
	API_printString("test");
	asm("cli \n\t");
	asm("cli \n\t");
	asm("cli \n\t");

	return 0;
}
