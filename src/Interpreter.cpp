#include "Interpreter.h"
#include "EventController.h"
#include "File.h"
#include "VirtualMachine.h"
#include "VMManager.h"


/* set interpreter focus on active vm:
 *
 * wenn vm gestartet: activeVMPID = neueVMPID
 * vm braucht PID !!!
 * ...
 * beim step: wenn in vm liste vm mit PID x existiert UND STATUS ist "RUNNING" und nicht "PAUSIEREN", dann alle eingaben und ausgaben der VM gewidmet
 * bei STRG+C > activeVMPID = 0 + Prozess beenden + "#/" zeigen
 * bei STRG+Z > activeVMPID BLEIBT!! + Prozess pausieren (vm braucht STATUS !!!) + "#/" zeigen
 * bei "fg" > Prozess STATUS = RUNNING
 * bei "bg" > activeVMPID = 0 [ALSO ANDERS ALS BEI UNIX!]
 * um Prozess zurückzuholen, "attach vmPID"
 * bei "attach" > zweiter parameter die vmPID die attacht werden soll
 * bei "lsps" ("list processes", NICHT "ps aux".. finde ich doof) > liste aller prozesse, ihre status (plural?)(, ggf. attachments??? muss aber interpreterabhaengig sein)
 *
 */

void Interpreter::handleEvent(ubyte1 eventType, void* ptr)
{
	switch (eventType)
	{
		case ardunix::events::etGetInterpreter:
		{
			if (!ptr) // send this class
			{
				eventController.fireEvent(ardunix::events::etGetInterpreter, (void*)this);
			}
			return;
		}
		case ardunix::events::etGetConsole:
			if (ptr && !this->console)
			{
				this->console = (Console*)ptr;
				prepareStatement(true);
			}
			return;
		case ardunix::events::etGetVFIManager:
			if (ptr)
				this->vfiManager = (VFIManager*)ptr;
			return;
		case ardunix::events::etGetVMManager:
			if (ptr)
			{
				this->vmManager = (VMManager*)ptr;
			}
			return;
		case ardunix::events::etSetInterpreterActiveVMProcess:
		{
			if (ptr) // what if n active process will be replaced? nobody will notice !
			{
				if (this->activeVMProc) // there is already a activeVMProc
				{
					if (this->console)
						this->console->PrintLN(F("Attempt to add a VM process to interpreter which already has an active VM process! failed."));
				}
				else
				{
					VMManager::VMProcess* vmProcess = (VMManager::VMProcess*)ptr;
					this->activeVMProc = vmProcess->vmID; // set active vm process id
				}
			}
		}
	}
}

Interpreter::~Interpreter()
{
	if (this->console)
		this->console->PrintLN(F("interpreter: bye..!"));
}

Interpreter::Interpreter(VFI* workingDirectory)
{
	this->statement_pos = 0;
	this->console = 0;
	this->vfiManager = 0;
	this->vmManager = 0;
	this->activeVMProc = 0; // 0 = invalid

	if (workingDirectory)
		this->wdir = workingDirectory;
	else
	{
		if (this->console)
			this->console->PrintLN(F("No block devices found"));
		return;
	}

	// getter
	eventController.registerEvent(ardunix::events::etGetVFIManager, this);
	eventController.fireEvent(ardunix::events::etGetVFIManager, 0); // null-pointer - because this class needs a RCDA
	eventController.registerEvent(ardunix::events::etGetConsole, this);
	eventController.fireEvent(ardunix::events::etGetConsole, 0); // null-pointer - because this class needs a RCDA
	eventController.registerEvent(ardunix::events::etGetVMManager, this);
	eventController.fireEvent(ardunix::events::etGetVMManager);

	//events
	eventController.registerEvent(ardunix::events::etSetInterpreterActiveVMProcess, this);

	// only start interpreter if a console is there - so no prepareStatement() here
}

void Interpreter::prepareStatement(bool echo)
{
	if (!this->vfiManager || !this->console)
	{
#ifdef DEBUG
		Serial.print(F("Something went badly wrong!"));
#endif
		return;
	}

	this->console->PrintLN();
	for (ubyte1 i = 0; i < (statement_max-1); i++) // clear statement
		this->statement[i] = ArdunixHeader::nullchar;
	this->statement[0] = ArdunixHeader::nullchar; // set statement begin
	this->statement_pos = 0; // reset statement array position

	if (echo)
	{
		this->console->Print((char*)this->vfiManager->getPathOfNode(this->wdir).c_str());
		this->console->Print(F("/# "));
	}
}

void Interpreter::execute()
{
	char* new_statement = strtok(this->statement, ArdunixHeader::spacestring); // get really first argument even if there is only one

	if (!new_statement)
	{
		this->prepareStatement(true);
		return;
	}

	if (strlen(new_statement) < 1)
		return;

	bool prepareStatementAfterExecution = true;

	/* shell integrated commands - and it is not necessary to pass a statement to InternalFunctions,
	 * because the methods get their statements by calling strtok (which can be used globally) */
	if (!strcmp_P(new_statement, PSTR("vm")))
	{
		if (this->console && this->vfiManager)
		{
			internalFunctions.startVirtualMachine(this->console, this->vfiManager, this->wdir);
			prepareStatementAfterExecution = false;
		}
		else
		{
#ifdef DEBUG
			Serial.println(F("Ouch.."));
#endif
		}
	}
	else
	// funny how I implemented this the worst way possible back in 2012:
		if (!strcmp_P(new_statement, PSTR("ls"))) // list all files
		{
			if (this->console)
				internalFunctions.listAllFiles(this->console, this->wdir);
		}
		else
			if (!strcmp_P(new_statement, PSTR("automount")))
			{
				if (this->vfiManager && this->console)
				{
					char* automountStr = "empty /dev/sdcard /mnt 123";
					strtok(automountStr, ArdunixHeader::spacestring);
					internalFunctions.mount(this->console, this->vfiManager, this->wdir);
				}
			}
			else
				if (!strcmp_P(new_statement, PSTR("mount")))
				{
					if (this->vfiManager && this->console)
						internalFunctions.mount(this->console, this->vfiManager, this->wdir);
				}
				else
					if (!strcmp_P(new_statement, PSTR("vfinfo")))
					{
						if (this->vfiManager && this->console)
							internalFunctions.vfinfo(this->vfiManager, this->console, this->wdir);
					}
					else
						if (!strcmp_P(new_statement, PSTR("help")))
						{
							if (this->console)
								internalFunctions.help(this->console);
						}
						else
							if (!strcmp_P(new_statement, PSTR("echo")))
							{
								if (this->console && this->vfiManager)
									this->internalFunctions.echo(this->console, this->vfiManager, this->wdir);
							}
							else
								if (!strcmp_P(new_statement, PSTR("cd"))) // change working-directory
								{
									if (this->console && this->vfiManager)
										this->internalFunctions.changeDirectory(this->console, this->vfiManager, this->wdir);
								}
								else
									if (!strcmp_P(new_statement, PSTR("restart"))) // print next input character
									{
										eventController.fireEvent(ardunix::events::etRestart, 0); // null-pointer - because destination class does not need parameters
									}
									else
										if (!strcmp_P(new_statement, PSTR("hexdump"))) // hey dump of EEPROM
										{
											if (this->console && this->vfiManager && this->wdir)
												this->internalFunctions.hexDump(this->console, this->vfiManager, this->wdir);
										}
										else // command not found? then it must be on the file system
										{
											VFI* file_VFI = this->vfiManager->parsePath(new_statement, wdir);
											if (!file_VFI)
											{
												this->console->Print(F("No such app found: "));
												this->console->PrintLN(this->statement);
												this->console->Print(7); // BELL
											}
											else
												if (file_VFI->fileType != ardunix::filesystem::filetype::file)
													this->console->Print(F("This is not an executable file!"));
												else
												{
													if (this->console && this->vfiManager)
													{
														internalFunctions.startVirtualMachine(this->console, this->vfiManager, this->wdir);
														prepareStatementAfterExecution = false;
													}
													else
													{
#ifdef DEBUG
														Serial.println(F("Ouch.."));
#endif
													}
												}
										}

	prepareStatement(prepareStatementAfterExecution);
}

bool Interpreter::functionKeys() // filter to not add characters (end users input) which can not be displayed
{
	delay(10); // important, otherwise there is no char available in serial buffer
	if (this->console->bytesAvailable() < 1)
		return false; // no, not part of a function key

	char key = this->console->getByte();

	switch(key)
	{
		case 'A':
			return true; // previousCommand
		case 'B':
			return true; // nextCommand
		case 'C':
			if (this->statement_pos > (statement_max-1)) return false;
			this->console->Print(91);
			this->console->Print(key);
			return true; // arrow right
		case 'D':
			if (this->statement_pos < 1) return false;
			this->console->Print(91);
			this->console->Print(key);
			return true; // arrow left

		default:
			return false; // no, not part of a function key
	}
}

void Interpreter::addCharToStatement(char c)
{
	if ((c < 32 || c > 127) && c != ArdunixHeader::nullchar) // discard non ascii input expect for backspace from backSpace which is '\0' in this case
	{
		this->console->Print(7); // BELL
		return;
	}

	if (this->statement[statement_max-2] != ArdunixHeader::nullchar && c == ArdunixHeader::nullchar) // if buffer full & backspace requested > set last char
		this->statement[statement_max-2] = ArdunixHeader::nullchar;
	else
		if (this->statement[0] == ArdunixHeader::nullchar
			 && c == ArdunixHeader::nullchar) // if buffer is already empty, do not backspace (but only if key pressed is backspace, otherwise you can never enter a command)
		{
			this->console->Print(7);
			return;
		}

	for (ubyte1 i = 0; i < (statement_max-1); i++)
	{
		if (this->statement[i] == ArdunixHeader::nullchar) // end of string found > add char & return true
		{
			if (c == ArdunixHeader::nullchar) // if backspace
			{
				this->statement[--this->statement_pos] = ArdunixHeader::nullchar;
				this->console->Print(8); // BACKSPACE
				this->console->Print(32); // SPACE
				this->console->Print(8); // BACKSPACE
			}
			else
			{
				this->statement[this->statement_pos] = c;
				this->statement[++this->statement_pos] = ArdunixHeader::nullchar;
			}

			this->console->Print(c);
			return;
		}
	}

	this->console->Print(7); // BELL
	return; // no case catched > not successful
}

void Interpreter::parseChar()
{
	if (!this->console)
		return;

	// check if a vm was exited
	if (this->activeVMProc)
	{
		VirtualMachine* vm = vmManager->getVirtualMachine(this->activeVMProc);
		if (!vm) // vm does not exist anymore
		{
			this->activeVMProc = 0; // vm was killed or exited or whatever
			prepareStatement(true);
		}
	}	

	if (this->console->bytesAvailable() < 1)
		return; // nothing to do, loop!

	char c  = this->console->getByte();
	char _c = c;

	switch (c) // special cases
	{
		case 8:
		case 127: // in some cases its 127 (which is 255 in unsigned) wtf
			_c = ArdunixHeader::nullchar;
			break;		
		case 91:
			if (this->functionKeys()) return; // do not write back if ascii 91 is part of an function key
			break;
		case 3: // STRG+C
			console->Print(F("\nSTRG+C"));
			vmManager->killVirtualMachine(this->activeVMProc); // kill vm
			this->activeVMProc = 0; // no vm attached anymore
			this->prepareStatement(true);
			// STRG+C SIGNAL
			return;
	}

	// send char to active process (if any)
	if (this->activeVMProc)
	{
		VirtualMachine* vm = vmManager->getVirtualMachine(this->activeVMProc);
		if (vm) // only if this vm exists
		{
			vm->addCharToInBuffer(_c);
		}
		else // check again, for saftey (?)
		{
			this->activeVMProc = 0; // vm was killed or exited or whatever
			prepareStatement(true);
		}
	}

	if (!this->activeVMProc) // check again, don't do an else here !
	{
		switch(c)
		{
			case 13: // only execute RETURN if no active VM Proc
			{
				this->console->PrintLN();
				this->execute();
				return; // CR-RETURN
			}
		}

		this->addCharToStatement(_c); // add to statement if no vm running
	}
}
