#ifndef VFIMANAGER_H	
#define VFIMANAGER_H

#include "ardunix.h"
#include "FileSystemManager.h"
#include "VFI.h"

class VFIManager // virtual file index manager
{
	public:
		VFIManager();
		~VFIManager(); // in hope that we can delete every VFI

		void addChildren(VFI* source, VFI* destination); // insert a children into a parent [THIS IS THE NEW CHILDREN LIST STUFF!!]
		VFI* newVFI(ubyte1 dev_id, char* node_name, ubyte1 fileType, ubyte1 fileSystemType, VFI* parent, int lba = 0); // insert a NEW children into a parent
		VFI* parsePath(char* path, VFI* wdir); // returns the index of the given path (can be a file or directory) PLEASE do not feed with bullshit or else...!
		List<VFI*>* getListOfNode(VFI* vfi);
		ubyte1 getChildrenNumOfVFI(VFI* vfi); // refresh the children list
		String getNonVFIPathOfNode(VFI* index);
		String getPathOfNode(VFI* index);
		void buildVFI();
		VFI* initializeDevice(byte deviceID);

		VFI root;
		VFI* dev;
		VFI* mnt;

	//private: TODO: make fsm private
		FileSystemManager fileSystemManager;
};

#endif
