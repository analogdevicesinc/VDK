/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Device flag creation API
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_API.h"
#include "vdk_History.h"
#include "DeviceFlagTable.h"
#include "VDK_Thread.h"
#include "Heap.h"
#include <stdlib.h>

#pragma file_attr("OS_Component=DeviceFlags")
#pragma file_attr("DeviceFlags")
#pragma file_attr("DevFlagCreation")
#pragma file_attr("SignalCreation")
#pragma file_attr("FuncName=CreateDeviceFlag")



namespace VDK {

////////////////////////////////////////////////////////////////////////////////
//
// Creates a device flag, and returns the ID. Threads could create device flags
// dynamically.
//
// If the number of active device flags are more than the maximum number of device
// flags or creation is failed then the call returns UINT_MAX.
//
////////////////////////////////////////////////////////////////////////////////

DeviceFlagID CreateDeviceFlag(void)
{
	DeviceFlagID devFlagID = static_cast <DeviceFlagID> (UINT_MAX);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	TMK_SuspendScheduling();
	devFlagID = g_DeviceFlagTable.GetNextObjectID();

	if(devFlagID != UINT_MAX)
	{
		DeviceFlag *newDevFlagObj = (DeviceFlag *) _heap_malloc(g_Heaps[g_DeviceFlagHeap].m_Index, sizeof(DeviceFlag));
		new (newDevFlagObj) DeviceFlag(devFlagID);

		if(newDevFlagObj)
		{
			g_DeviceFlagTable.AddObject(newDevFlagObj);
			HISTORY_(VDK::kDeviceFlagCreated, devFlagID, GetThreadID());
		}
		else
			devFlagID = static_cast <DeviceFlagID> (UINT_MAX);
	}

	TMK_ResumeScheduling();

	//
	// unable to create a device flag
	//
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if(devFlagID == UINT_MAX)
		DispatchThreadError(kDeviceFlagCreationFailure,devFlagID);
#endif

	return(devFlagID);
}


}
