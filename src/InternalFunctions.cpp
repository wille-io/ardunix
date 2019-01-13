#include "InternalFunctions.h"
#include "EventController.h"
#include "File.h"
#include "VirtualMachine.h"

void InternalFunctions::changeDirectory(Console* console, VFIManager* vfiManager, VFI*& wdir)
{
	char* temp = strtok(NULL, ArdunixHeader::spacestring);

	VFI* index = 0;
	if (temp)
		index = vfiManager->parsePath(temp, wdir);

	if (index) // only change directory if new index is valid and the index is a directory
		if (index->fileType != ardunix::filesystem::filetype::directory)
			console->Print(F("cd: not a directory"));
		else
			wdir = index; // arg
	else
		console->Print(F("cd: directory not found"));
}

void InternalFunctions::mount(Console* console, VFIManager* vfiManager, VFI* wdir)
{
	char* devicePath = strtok(NULL, ArdunixHeader::spacestring); // /dev/sdcard/
	char* mountPath = strtok(NULL, ArdunixHeader::spacestring); // /mnt
	char* fileSystemType = strtok(NULL, ArdunixHeader::spacestring); // 123 (for adxFS)

	if (!devicePath || !mountPath || !fileSystemType)
	{
		console->PrintLN(F("mount: usage: mount devicePath mountPath fileSystemType"));
		return;
	}
	else
	{
		VFI* devicePathVFI = vfiManager->parsePath(devicePath, wdir);
		if (!devicePathVFI) // check if given device path is valid
		{
			console->PrintLN(F("mount: invalid device path"));
			return;
		}

		VFI* mountPathVFI = vfiManager->parsePath(mountPath, wdir);
		if (!mountPathVFI) // check if given target path is valid
		{
			console->PrintLN(F("mount: invalid mount path"));
			return;
		}


		/* mount the device by creating a new VFI which has the information that ardunix has to access this device
		 * by using the given filesystem driver.
		 * > use the deviceID by the devicePath (because udev initialized the device with the correct deviceID)
		 * > TODO: replace "sdcard" with a variable ;)
		 * > use the mounted device, logically, as a directory as a filesystem can't be a file or device
		 * > tell the VFI of which file system it is
		 * > use the mount path to add the vfi to, for example, /mnt. so path would be "/mnt/sdcard" */

		int _fileSystemType = atoi(fileSystemType);

		// initialize filesystem first, before using it


		VFI* rootVFI = vfiManager->fileSystemManager.initializeFilesystem(_fileSystemType, devicePathVFI);

		if (!rootVFI) // if initialization was successful
		{
			console->PrintLN(F("mount: could not initialize the filesystem"));
			return;
		}
		else
		{
			rootVFI->parent = mountPathVFI; // set the mountPathVFI as parent, so that the directory is "/mnt/sdcard/"
			mountPathVFI->addChild(rootVFI); // add the sdcard vfi to the mount path, because everything is fine
			console->Print(F("mounted to "));
			console->PrintLN(mountPathVFI->nodeName);
		}
	}

	// TODO: free memory..
}

void InternalFunctions::echo(Console* console, VFIManager* vfiManager, VFI* wdir)
{
	bool showInHex = false;
	char* filepath = strtok(NULL, ArdunixHeader::spacestring);
	char* hex  = strtok(NULL, ArdunixHeader::spacestring);

	if (hex && !strcmp_P(hex, PSTR("HEX")))
		showInHex = true;

	VFI* file_VFI = 0;
	if (!filepath)
		console->PrintLN(F("echo: no file specified"));
	else
	{
		file_VFI = vfiManager->parsePath(filepath, wdir);

		if (file_VFI)
		{
			File file; // = new File();

			if (file.openFile(file_VFI))
			{
				char c = 0;

				for (int i = 0; i < 64; i++)
				{
					c = file.getByte(i);

					//if (!c)
						//break;

					if (showInHex)
						console->Print((int)c, 10);
					else
						console->Print(c);
				}
			}
			else
				console->PrintLN(F("echo: cannot open file"));
		}
		else
			console->PrintLN(F("echo: file not found"));
	}
}

void InternalFunctions::help(Console* console)
{
	console->Print  (F("ardunix.org "));
	console->Print  (F("ardunix standard interpreter "));
	console->PrintLN(F("--------------------------------------------------------------------------"));
	console->PrintLN();
	console->PrintLN(F("List of commands of the internal standard interpreter: "));
	console->PrintLN(F("   cd      - change directory"));
	console->PrintLN(F("   echo    - echo textfile to the standard output"));
	console->PrintLN(F("   ls      - list all files"));
	console->PrintLN(F("   restart - restart ardunix"));
	console->PrintLN(F("   vm      - start a virtual machine from a ELF file or a device with a"));
	console->PrintLN(F("             ELF file burned on it"));
	console->PrintLN(F("   vfinfo  - print informations about current or specified VFI"));
	console->PrintLN(F("   mount   - mount a device to the given path and the given file system type"));
	console->PrintLN();
	console->PrintLN(F("Because this interpreter adapts the functionality of unix, you can "));
	console->PrintLN(F("refer to every unix support. You can also refer to ardunix.org/wiki"));
	console->PrintLN(F("for help."));
}

void InternalFunctions::vfinfo(VFIManager* vfiManager, Console* console, VFI* wdir)
{
	char* file = strtok(NULL, ArdunixHeader::spacestring);

	VFI* file_VFI = 0;
	if (file) // if there is a parameter..
		file_VFI = vfiManager->parsePath(file, wdir);
	else
		file_VFI = wdir; // no parameter, so take current directory

	if (file_VFI)
	{
		console->PrintLN(F("VFI informations:"));
		console->PrintLN();
		console->Print(F("device id       : ")); console->PrintLN((int)file_VFI->deviceID);
		console->Print(F("node id         : ")); console->PrintLN((int)file_VFI->iNode);
		console->Print(F("node name       : ")); console->PrintLN(file_VFI->nodeName);
		console->Print(F("file size       : ")); console->PrintLN(file_VFI->fileSize);
		console->Print(F("file type       : ")); console->PrintLN((int)file_VFI->fileType);
		console->Print(F("file system     : ")); console->PrintLN((int)file_VFI->fileSystemType);

		console->Print(F("parent VFI path : "));
		if (file_VFI->parent)
			 console->PrintLN(file_VFI->parent->nodeName);
		else
			console->PrintLN(F("no parent!"));

		console->Print(F("sub entries     : "));
		if (file_VFI->getChildrenList())
			console->PrintLN(file_VFI->getChildrenList()->count());
		else
			console->PrintLN(F("no subentries!"));
	}
	else
		console->PrintLN(F("vfinfo: file not found"));
}

void InternalFunctions::startVirtualMachine(Console* console, VFIManager* vfiManager, VFI* wdir)
{
	//bool isBinary = false;
	char* filepath = strtok(NULL, ArdunixHeader::spacestring);
	/*if (!strcmp_P(file, PSTR("-b"))) // check if file is either BINARY or by default avr ELF (-b for binary)
	{
		file = strtok(NULL, ArdunixHeader::spacestring);
		isBinary = true;
	}*/

	if (!filepath)
		console->PrintLN(F("vm: no file specified"));
	else
	{
		VFI* file_VFI = vfiManager->parsePath(filepath, wdir);
		if (!file_VFI)
		{
			console->Print(F("No such app found: "));
			console->PrintLN(filepath);
			console->Print(7); // BELL
		}
		else
			if (file_VFI->fileType != ardunix::filesystem::filetype::file && // TODO: || ?!
				 file_VFI->fileType != ardunix::filesystem::filetype::device) // vm can be started from raw device!!!!
				console->Print(F("This is not an executable file!"));
			else
			{
				File* file = new File();
				if (!file->openFile(file_VFI))
					delete file;
				else
				{
					VirtualMachine* vm = new VirtualMachine;

					if (!vm->init(file, (char*)0, 0))//, isBinary))
					{
						eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("vm: Error starting virtual machine!"));
						delete vm;
						delete file;												
					}
					else
					{
						//eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("virtual machine .."));
						eventController.fireEvent(ardunix::events::etAddVirtualMachine, vm); // add new virtual machine to kernel, which steps the cpu !!
					}
				}
			}
	}
}

void InternalFunctions::listAllFiles(Console* console, VFI* index)
{
	if (!index->getChildrenList() || index->getChildrenList()->count() < 1) // first check if there is a childrenList! then if count is higher than 1
	{
		console->PrintLN(F("ls: there are no files to display!"));
		return;
	}
	else
	{
		//console->Print(F("index->childrenList = "));
		//console->PrintLN((BUS_SIZED_PTR)(void*)index->childrenList);
	}

	console->Print(F("list of all files in '"));
	console->Print(index->nodeName);
	console->Print(F("':\r\n\r\nFiletype  Filename\r\n"));

	VFI* temp = 0;

	if (index)
		if (index->getChildrenList())
			temp = index->getChildrenList()->getFirstEntry();

	while (temp)
	{
		switch (temp->fileType)
		{
			case ardunix::filesystem::filetype::directory:
				console->Print(F("dir "));
				break; // this break does only break the SWITCH not the while
			case ardunix::filesystem::filetype::file:
				console->Print(F("file"));
				break;
			case ardunix::filesystem::filetype::device:
				console->Print(F("dev "));
				break;
			default:
				console->Print(F("?"));
				break;
		}

		console->Print(F("      "));
		console->PrintLN(temp->nodeName);

		temp = index->getChildrenList()->getNextEntry();
	}
}

void InternalFunctions::hexDump(Console* console, VFIManager* vfiManager, VFI* wdir)
{
	char* file = strtok(NULL, ArdunixHeader::spacestring);

	VFI* file_VFI = 0;
	if (file) // if there is a parameter..
		file_VFI = vfiManager->parsePath(file, wdir);
	else
		file_VFI = wdir; // no parameter, so take current directory

	if (file_VFI)
	{
		int start = 116;
		for (int i = start; i < start + 100; i++)
		{
			console->PrintNumber(i - start);
			console->Print(" ");
			console->PrintLN(EEPROM.read(i+64), 16);
		}
	}
	else
		console->PrintLN(F("hexdump: file not found"));
}
