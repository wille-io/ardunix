#ifdef FAT16
#ifndef FAT16FILESYSTEMACCESSOR_H	
#define FAT16FILESYSTEMACCESSOR_H

#include "ardunix.h"
#include "AbstractFileSystemAccessor.h"
#include "VFI.h"
#include "List.h"

// this class represents the fat16 file system which is on a partition on serval devices
class FAT16FileSystemAccessor : public AbstractFileSystemAccessor
{
	public:
      FAT16FileSystemAccessor();// : AbstractFileSystemAccessor(6); // getting VFI manager & Raw Block Device Accessor.. & set file system type to 6
		~FAT16FileSystemAccessor();
      VFI* getVFI(char *nodeName);
		List<VFI*>*getListOfPath(char* path);
      ubyte1 getFileByte(int &position);
      bool setFileByte(int &position, ubyte1 &value);
};

#endif
#endif
