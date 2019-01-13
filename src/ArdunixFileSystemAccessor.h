#pragma once

#include "EventController.h"
#include "AbstractFileSystemAccessor.h"
#include "VFI.h"


#define ADXFS_MINVER 1 // which versions can be read

#define FIT_BLOCK_POS 2 // the block in which the file information table starts
#define FDT_BLOCK_POS 260 //FIT_BLOCK_POS + 256 // = 260

class FITBlock
{
	public:
		FITBlock();
		~FITBlock();

		// block 0
		ubyte1 nodeID;
		ubyte1 parentNodeID;

		// block 1
		char*  fileName;
};

// this class represents the ardunix filesystem which is on a partition on serval devices
class ArdunixFileSystemAccessor : public AbstractFileSystemAccessor
{
	public:
		ArdunixFileSystemAccessor(); // getting VFI manager & Raw Block Device Accessor.. & set file system type to 6
		~ArdunixFileSystemAccessor();
		//VFI* getVFI(VFI* currentParent, char *nextNodeName);
		List<VFI*>* getChildrenListOfVFI(VFI* vfi);
		ubyte1 getFileByte(VFI* vfi, ubyte8& position);
		bool setFileByte(VFI* vfi, int &position, ubyte1 &value);
		VFI* getRootVFI(ubyte1 deviceID);
		bool initPartition(VFI* vfi);

	private: // TODO: other data types pl0x
		ubyte1 version;
		ubyte2 devBlockSize; // standard = 256
		ubyte8 fsBlockSize; // standard = 16
		bool validatePartition(VFI *vfi);
};
