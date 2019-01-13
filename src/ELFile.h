#ifndef ELFILE_H
#define ELFILE_H

#include "ardunix.h"
#include "File.h"
#include "EventController.h"

// this class represents an access to an elf file, the file is read after the offset which is given by the elf file
class ELFile : public File
{
	public:
		ELFile(File* file);
		bool openFile();

		/*ubyte1 getByte(int pos);
		void setByte(int pos, ubyte1 value);*/

		ubyte2 getWord(int pos);

		ubyte1 offset;
};

#endif // ELFILE_H
