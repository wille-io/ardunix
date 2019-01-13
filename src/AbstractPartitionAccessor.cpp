#include "AbstractPartitionAccessor.h"
#include "FAT16FileSystemAccessor.h"
#include "ArdunixFileSystemAccessor.h"

AbstractPartitionAccessor::AbstractPartitionAccessor(ubyte1 fileSystemType) : AbstractFileSystemAccessor(fileSystemType)
{
	this->fileSystemAccessor = 0;

	switch (fileSystemType)
	{
		//case 6:  this->fileSystemAccessor = new FAT16FileSystemAccessor; this->fileSystemType = 6; return;
		case 123: this->fileSystemAccessor = new ArdunixFileSystemAccessor; this->fileSystemType = 123; return;
		default: this->fileSystemAccessor = 0; this->fileSystemType = 0; return;
	}
}

AbstractPartitionAccessor::~AbstractPartitionAccessor()
{
	//delete this->fileSystemAccessor;
	this->fileSystemAccessor = 0;
}

List<VFI*>* AbstractPartitionAccessor::getListOfVFI(VFI* vfi)
{
	if (fileSystemType)
		return fileSystemAccessor->getChildrenListOfVFI(vfi);
	else
		return 0;
}

ubyte1 AbstractPartitionAccessor::getFileByte(ubyte8& position)
{
	return 0;
}

bool AbstractPartitionAccessor::setFileByte(int& position, byte& value)
{
	return 0;
}
