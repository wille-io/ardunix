#ifndef INTERNALFUNCTIONS_H			
#define INTERNALFUNCTIONS_H

#include "ardunix.h"
#include "VFI.h"
#include "VFIManager.h"
#include "Console.h"

/* this class contains all functions which can be executed by the interpreter
 * further, this class is there to outsource big functions from the Interpreter
 * to make the Interpreter more readable */
class InternalFunctions
{
	public:
		void changeDirectory(Console* console, VFIManager* vfiManager, VFI*& wdir);
		void echo(Console* console, VFIManager* vfiManager, VFI*wdir);
		void vfinfo(VFIManager* vfiManager, Console* console, VFI* wdir);
		void listAllFiles(Console* console, VFI* index);
		void startVirtualMachine(Console* console, VFIManager* vfiManager, VFI*wdir);
		void help(Console* console);
		void hexDump(Console* console, VFIManager* vfiManager, VFI* wdir);
		void mount(Console* console, VFIManager* vfiManager, VFI* wdir);
		void automount(Console* console, VFIManager* vfiManager, VFI* wdir);
};

#endif
