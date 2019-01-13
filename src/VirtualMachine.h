#ifndef VIRTUALMACHINE_H					
#define VIRTUALMACHINE_H

#include "CPU.h"
#include "OpcodeManager.h"
#include "File.h"
#include "ELFile.h"

/* this class manages a program which executed and running and has the ability
 * to read an executable file and execute its program code */
class VirtualMachine
{
	public:
		VirtualMachine();

		bool init(File* file, char* arga, int args);//, bool isBinary);
		void cpustep();
		ubyte2 readWord(int pos);
		ubyte2 getOpcode();
		void executeOpcode(ubyte2 opcode);
		CPU* cpu;
		void addCharToInBuffer(char c); // char from console

	private:		
		OpcodeManager* opcodeManager;
		int stream_pos;
		File* file;

		char inBuf; // char to read from vm (1 byte buffer size)
};

#define EXECUTE_OPCODE(opcode, demonic)\
  case opcode: \
  { \
    opcodeManager->opcodeICALL(); \
    return; \
  }

#endif
