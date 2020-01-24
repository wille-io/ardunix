#include "ArdunixFileSystemAccessor.h"

FITBlock::FITBlock()
{
	this->fileName = 0;
	this->nodeID = 0;
	this->parentNodeID = 0;
}

FITBlock::~FITBlock()
{
	delete fileName;
	this->fileName = 0;
	this->nodeID = 0;
	this->parentNodeID = 0;
}



#warning UNRESOLVED PROBLEM !!! the devBlockSize var has to be located in the SDRBDA , but cannot be received here... problem! devBlockSize must be a virtual var in the parent class !!



ArdunixFileSystemAccessor::ArdunixFileSystemAccessor() :
	AbstractFileSystemAccessor(ardunix::filesystem::filesystemtype::adxFS) // getting VFI manager & Raw Block Device Accessor.. & set file system type to 123
{
	devBlockSize = 512; // standard
	fsBlockSize = 16; // TEMP!! read fsBlockSize from SD card !!
	version = 0;


	// TEST !!!
	initialized = true;
	valid = true;
}

ArdunixFileSystemAccessor::~ArdunixFileSystemAccessor()
{
	fileSystemType = 0;
	devBlockSize = 0;
	fsBlockSize = 0;
	version = 0;
}

bool ArdunixFileSystemAccessor::validatePartition(VFI* vfi) // to validate that the given vfi's filesystem is of this type
{
	ubyte1 header_magic1 = rawBlockDeviceAccessor->getByte(vfi->deviceID, 0);
	ubyte1 header_magic2 = rawBlockDeviceAccessor->getByte(vfi->deviceID, 1);
	ubyte1 header_magic3 = rawBlockDeviceAccessor->getByte(vfi->deviceID, 2);
	ubyte1 header_magic4 = rawBlockDeviceAccessor->getByte(vfi->deviceID, 3);
	ubyte1 header_magic5 = rawBlockDeviceAccessor->getByte(vfi->deviceID, 4);
	//ubyte1 header_version = rawBlockDeviceAccessor->getByte(vfi->deviceID, 5);

	/*Serial.print("Inhalt Header: ");

	Serial.print(header_magic1);
	Serial.print(header_magic2);
	Serial.print(header_magic3);
	Serial.print(header_magic4);
	Serial.print(header_magic5);*/

	if (   header_magic1 == 'a' // 0x61 &&
			 && header_magic2 == 'd' // 0x64 &&
			 && header_magic3 == 'x' // 0x78 &&
			 && header_magic4 == 'F' // 0x46 &&
			 && header_magic5 == 'S' // 0x53 &&
			 //&& header_version <= ADXFS_MINVER)
			 )
	{
		return true;
	}
	else
	{
		/*Serial.print((char)header_magic1);
		Serial.print((char)header_magic2);
		Serial.print((char)header_magic3);
		Serial.print((char)header_magic4);
		Serial.println((char)header_magic5);*/
		return false;
	}

	return false;
}

/**
 * @brief ArdunixFileSystemAccessor::initPartition
 * @param vfi - ???
 * @return
 */
bool ArdunixFileSystemAccessor::initPartition(VFI* vfi) // validates and initializes the partition (initializes this classes attributes)
{
	if (!validatePartition(vfi))
	{
		return false;
	}
	else
	{
		// TODO: look for last device error before fast judgement that device works and fs is right

		version = rawBlockDeviceAccessor->getByte(vfi->deviceID, 5); // set fs version for this class

		/* NOT IMPLEMENTED YET ubyte1 blockSizeLow = rawBlockDeviceAccessor->getByte(vfi->deviceID, 6);
		ubyte1 blockSizeHigh = rawBlockDeviceAccessor->getByte(vfi->deviceID, 7);

		converter c;
		c.UBYTE1[0] = blockSizeLow;
		c.UBYTE1[1] = blockSizeHigh;

		fsBlockSize = c.UBYTE2; // 2 bytes, so we need to convert 2 bytes into 1 short
		Serial.print("fsBlockSize = "); Serial.println((long)fsBlockSize);*/

		// TODO: add sub entries (subdirectories) to mounted directory already !!

		return true;
	}

	return false;
}

// DEPRECATED ?? should call getVFI with invalid currentPath and nextNodeName as "/"
VFI* ArdunixFileSystemAccessor::getRootVFI(ubyte1 deviceID) // for initializers
{
	if (!initialized || !valid)
	{
		return 0;
	}
	else
	{
		/* the root vfi's nodename is always '/'
		 * so create a vfi with nodename slashstring
		 *
		 * read all FITs and see which FITs have 0 (root) as parent and add them to the rootVFI
		 * but begin at sektor 3. as we must ignore root (which starts at sektor 2) */

		VFI* rootVFI = new VFI;
		rootVFI->deviceID = deviceID;
		rootVFI->fileSize = 0;
		rootVFI->fileSystemType = ardunix::filesystem::filesystemtype::adxFS;
		rootVFI->fileType = ardunix::filesystem::filetype::directory;
		rootVFI->iNode = 0; // 0 = root
		rootVFI->lba = 0; // don't care
		rootVFI->nodeName = (char*)ArdunixHeader::slashstring; // THIS STRING HAS TO BE REPLACED BY THE CALLER !! e.g. "sdcard"
		rootVFI->parent = 0; // no parent for now, the caller of getRootVFI has to set the parent !!!


		// ADD CHILDREN [CAUTION! " < 255" <= okay!    " <= 255" <= loop]
		for (ubyte1 fitBlock = 1; fitBlock < 255; fitBlock++) // ubyte1 enough, as there are only 256 FITs and ubyte1 can count from 0 - 255
		{
			ubyte8 sdSector = fitBlock + 2; // get sector on sd card
			ubyte8 pos = sdSector * devBlockSize; // convert sector to position
			ubyte1 fitParent = rawBlockDeviceAccessor->getByte(deviceID, pos); // byte 0 of current fit

			if (fitParent == 0) // only get FITs with parent 0 (root) [uhm.. y?]
			{
				ubyte1 filenameSize = 0; // if filenameSize == 0 then this is an empty FIT block

				for (ubyte1 i = 0; i <= 15; i++) // get filename size (max size = 15)
				{
					ubyte1 filenameChar = rawBlockDeviceAccessor->getByte(deviceID, pos + 1 + i); // byte i of current fit in section "filename"

					if (!filenameChar)
					{
						//Serial.print("filenameChar not set!");
						//Serial.println("pos = "); Serial.println((long)pos);
						break;
					}

					filenameSize = i+1; // as size does never start with 0 if more than 0 chars are available ;)
					//Serial.print("filenamesize = ");
					//Serial.println((int)filenameSize);
				}

				if (!filenameSize)
					continue;

				ubyte1* filename = (ubyte1*)malloc(filenameSize+1); // no memset, I trust this method. +1 for terminating string

				for (ubyte1 i = 0; i < filenameSize; i++) // get filename
				{
					ubyte1 c = rawBlockDeviceAccessor->getByte(deviceID, pos + 1 + i); // byte i of current fit in section "filename"
					//Serial.print((char)c); Serial.print("("); Serial.print((int)c); Serial.print(") ");
					filename[i] = c;
				}

				filename[filenameSize] = 0; // terminate string !!


				// create children now
				VFI* children = new VFI;
				children->deviceID = deviceID;
				children->fileSize = 0; // temp
				children->fileSystemType = ardunix::filesystem::filesystemtype::adxFS;
				children->fileType = ardunix::filesystem::filetype::file; // I forgot to teach adxFS what files and directories are... temp!!
				children->iNode = fitBlock; // as the iNode is the fitBlock
				//Serial.print("fit block = "); Serial.println((long)fitBlock);
				children->lba = 0; // don't care
				children->nodeName = (char*)filename;
				children->parent = rootVFI;

				rootVFI->addChild(children);
			}
			/*else
				Serial.println("fitParent != 1");*/
		}

		return rootVFI;
	}
}

/*VFI* ArdunixFileSystemAccessor::getVFI(VFI* currentParent, char* nextNodeName) // reconsider this method.. how to complete incomplete VFIs? (missing children, etc.)
{

}*/

List<VFI*>* ArdunixFileSystemAccessor::getChildrenListOfVFI(VFI* vfi) // get children of vfi
{
	// TEST !!!
	List<VFI*>* childrenList = new List<VFI*>();

	VFI* testVFI = new VFI;
	testVFI->deviceID = vfi->deviceID; // take device ID from currentParent, as any child can only be on THAT device on which the parent is from
	testVFI->fileSize = 0;
	testVFI->fileSystemType = ardunix::filesystem::filesystemtype::adxFS;
	testVFI->fileType = ardunix::filesystem::filetype::file;
	testVFI->iNode = 1;
	//testVFI->lba = 123; // TEST !
	testVFI->nodeName = "getChildrenListOfVFI_TEST";
	testVFI->parent = vfi;

	testVFI->getChildrenList()->appendEntry(testVFI);

	return childrenList;
}

ubyte1 ArdunixFileSystemAccessor::getFileByte(VFI* vfi, ubyte8& position)
{
	// TODO: comment this better

	ubyte8 inode = 0; //

	if (vfi) // valid vfi given? then take inode
		inode = vfi->iNode; // get inode

	ubyte8 ftd_block = (inode * 256) + FDT_BLOCK_POS; // get first ftd block
	//Serial.print("  fsBlockSize = "); Serial.println((long)fsBlockSize);
	ubyte8 skipBlocks = position / fsBlockSize; // get the block we really want by respecting the position and the filesystem block size    Byte 56 wanted > 56 / 16 = 3,5 > Block 3
	ftd_block = ftd_block + skipBlocks; // get the real block we want to read by respecting the position
	ubyte8 rest = position % fsBlockSize; // get the rest(/offset) in block
	ubyte8 bytes = (ftd_block - 1) * devBlockSize + rest; // get the real position in bytes

	/*Serial.print("  Block = "); Serial.print((long)ftd_block);
	Serial.print("  Rest = "); Serial.print((long)rest);
	Serial.print("  in Bytes: "); Serial.println((long)bytes);*/

	return rawBlockDeviceAccessor->getByte(vfi->deviceID, bytes);
}

bool ArdunixFileSystemAccessor::setFileByte(VFI* vfi, int& position, byte& value)
{
	/* now comes the biggest advantage of this filesystem
	 * normally we have to read all [devBlockSize] bytes (256 bytes by default)
	 * but adxFS has, by default, a 16 byte fsBlockSize. so there must be only 16 bytes read
	 * and then 16 bytes written (even if this method only writes 1 byte. if you do not know why
	 * please refer to a sd card tutorial which explains blocks).
	 * the missing 240 bytes can be filled with zeroes or with whatever you like, because
	 * adxFS ignores everything after the first [fsDevBlockSize] bytes
	 * additional: NOPE, this is not the job of the block device driver because then
	 * there must be a cache with the size of [devBlockSize] bytes, because fsBlockSize is
	 * unknown to the relevant device driver */

	return false;
}
