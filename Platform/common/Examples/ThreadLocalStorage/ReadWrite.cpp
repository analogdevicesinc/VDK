#include "VDK.h"

/******************************************************************************
 *  Read Write Function 
 *
 *	Retrieves the current thread's device descriptor from its local storage
 *	area and then calls some device driver functions.
 */

void ReadWrite(int ddSlot)
{ 
    // Get the corresponding Device Descriptor for the current thread
	// (Cannot cast directly from void* to VDK::DeviceDescriptor, so we 
	// cast to int first and then to VDK::DeviceDescriptor)
    VDK::DeviceDescriptor dd = (VDK::DeviceDescriptor)
									((int)VDK::GetThreadSlotValue(ddSlot));
									
	// Read from the Device
	VDK::SyncRead(dd, 0, 0, 0);

	// Write to the Device
	VDK::SyncWrite(dd, 0, 0, 0);
}

