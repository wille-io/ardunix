#ifndef RAWDEVICEACCESSOR_H
#define RAWDEVICEACCESSOR_H

#include "RawBlockDeviceAccessor.h"
#include "RawCharDeviceAccessor.h"

/* unimportant class, RBDA and RCDA inherit from this class,
 * they have much in common (devices) but in general this class
 * can be removed */

class RawDeviceAccessor
{
	public:
		RawDeviceAccessor();

		/*ubyte1 getByte();
		void setByte(); NOT NECESSARY?? */
};

#endif // RAWDEVICEACCESSOR_H
