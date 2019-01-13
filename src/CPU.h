#ifndef CPU_H				
#define CPU_H

#include "ardunix.h"

// this class represents the emulated avr microcontroller processor
class	CPU
{
	public:
		// VIRTUAL MACHINE REGISTER

		void incPC()
		{
			this->PC += 2;
		}

		ubyte2 PC;
		ubyte1 gpwr [32];
		ubyte1 sreg;
		ubyte1 sram [0x08FF]; //  fucking stackpointer points to 0x08FF (which is 2303)         96]; // FOR IO TEST !  IO start @ 32 bytes, IO length 64 bytes
		// a solution to avoid assembler - first byte to determine api type (cmd) and the second byte which points to the sram location where the coherant value is stored
		// operation: note that the api-array comes directly after the sram-array - so sram[96] & sram[97] are on api[0] and api[1] !
		//ubyte1 api[3]; // due to an error i made in STD_Z, api has to be 3, check for 2 and 3 if val > 0
		ubyte1 api_cmd; // 2.303
		ubyte2 api_adr; // 2.304 - 2.305

#warning MUST REPLACE STACK AND STPTR INTO SRAM
		//ubyte1 stack[16]; // insert stack into sram(like in every cpu) later !
		//ubyte1 stptr;     // stack pointer

		// RAMPX, RAMPY, RAMPZ, RAMPD and EIND: 8-bit segment registers that are prepended to 16-bit addresses in order to form 24-bit addresses; only available in parts with large address spaces.
		ubyte1 rampx;
		ubyte1 rampy;
		ubyte1 rampz;

		ubyte1 rampd;
		ubyte1 eind;
};

#endif
