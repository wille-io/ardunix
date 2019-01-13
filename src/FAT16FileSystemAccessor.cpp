#ifdef FAT16
#include "FAT16FileSystemAccessor.h"

FAT16FileSystemAccessor::FAT16FileSystemAccessor() : AbstractFileSystemAccessor(6) // getting VFI manager & Raw Block Device Accessor.. & set file system type to 6
{

}

FAT16FileSystemAccessor::~FAT16FileSystemAccessor()
{
	fileSystemType = 0;
}

VFI* FAT16FileSystemAccessor::getVFI(char* nodeName)
{
	return 0; // not implemented yet............
}

List<VFI*>* FAT16FileSystemAccessor::getListOfPath(char* path)
{
	if (!fileSystemType)
		return 0;

	char* temp = strtok((char*)ArdunixHeader::delimiter, path);

	while (temp)
	{
		// go to path ......

		temp = strtok((char*)ArdunixHeader::delimiter, 0);
	}

	// successful? get path children

	return 0; // because there is no code yet..
}

ubyte1 FAT16FileSystemAccessor::getFileByte(ubyte8& position)
{
	return 0;
}

bool FAT16FileSystemAccessor::setFileByte(int& position, byte& value)
{
	return 0;
}
#endif
