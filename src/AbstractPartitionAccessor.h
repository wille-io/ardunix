#ifndef ABSTRACTPARTITIONACCESSOR_H	
#define ABSTRACTPARTITIONACCESSOR_H

#include "ardunix.h"
#include "AbstractFileSystemAccessor.h"

// this class represents a partition on serval devices
class AbstractPartitionAccessor : public AbstractFileSystemAccessor // partition accessor for one partition. one partition has one file system
{
	public:
      AbstractPartitionAccessor(ubyte1 fileSystemType);
		~AbstractPartitionAccessor();
		List<VFI*>* getListOfVFI(VFI* vfi);
		ubyte1  getFileByte(ubyte8& position);
		bool  setFileByte(int& position, byte& value);

	private:
		AbstractFileSystemAccessor* fileSystemAccessor; // fileSystem determined by fileSystemType. One partition has only one file system
};

#endif
