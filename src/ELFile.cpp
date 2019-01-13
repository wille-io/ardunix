#include "ELFile.h"

ELFile::ELFile(File* file)
{
	this->offset = 0;

	// copy file class
	this->file = file->file;
	this->rawBlockDeviceAccessor = file->rawBlockDeviceAccessor;
}

bool ELFile::openFile()
{
	// check 0x7F(ELF)
	if (getByte(0) != 0x7F || getByte(1) != 'E' || getByte(2) != 'L' || getByte(3) != 'F')
	{
		eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("elfile: invalid magic number"));
		return false;
	}

	// check for 32 bit
	if (getByte(4) != 1)
	{
		eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("elfile: no 32 bit"));
		return false;
	}

	// check endianess
	if (getByte(5) != 1)
	{
		eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("elfile: wrong endianess"));
		return false;
	}

	// check ELF version
	if (getByte(6) != 1)
	{
		eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("elfile: wrong ELF version"));
		return false;
	}

	// check file type (executable, shared... etc.)
	if (getByte(0x10) != 2)
	{
		eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("elfile: not an as executable marked file"));
		return false;
	}


	ubyte1 header_pos = getByte(0x1C);

	this->offset = getByte(header_pos + 4); // here is where the program starts
	//Serial.print(F("elfile: offset at: "));
	//Serial.println(this->offset);

	return true;
}

/*ubyte1 ELFile::getByte(int pos)
{
	Serial.print("step3.3 ");

	return this->getByte(pos + this->offset);
}*/

ubyte2 ELFile::getWord(int pos)
{
	converter con;
	con.UBYTE1[0] = this->getByte((pos) + this->offset);
	con.UBYTE1[1] = this->getByte((pos) + this->offset + 1);

	return con.UBYTE2;
}

/*void ELFile::setByte(int pos, ubyte1 value)
{
	this->setByte(pos + this->offset, value);
}*/

