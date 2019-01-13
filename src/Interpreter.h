#ifndef INTERPRETER_H					
#define INTERPRETER_H

#include "ardunix.h"
#include "InternalFunctions.h"
#include "Console.h"
#include "VFIManager.h"
#include "VMManager.h"


// this class can interpret commands etc., that are entered by the end user
class Interpreter : public AbstractEventClass
{
	public:
		InternalFunctions internalFunctions;

		Console* console;
		VFIManager* vfiManager;

#define statement_max 65 // = 64 + 1 backup null / \0
		char    statement[statement_max]; // terminal entered statement to execute
		int     statement_pos;
		VFI*    wdir;
		ubyte1  activeVMProc;
		VMManager* vmManager;


		void handleEvent(ubyte1 eventType, void* ptr);
		~Interpreter();
		Interpreter(VFI* workingDirectory);
		void listAllFiles(VFI* index);
		void prepareStatement(bool echo);
		void execute();
		bool functionKeys();
		void addCharToStatement(char c);
		void parseChar();
};

#endif
