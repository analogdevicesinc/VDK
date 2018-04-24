/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Device flag Destruction
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
#pragma file_attr("SignalDestruction")
#pragma file_attr("DevFlagDestruction")
#pragma file_attr("FuncName=DestroyDeviceFlag")

namespace VDK
{
////////////////////////////////////////////////////////////////////////////////
//
// Destroys the device flag object and deletes the memory associated with it.
//
// In the instrumented builds this call will throw the error kInvalidDeviceFlag
// for invalid device flags.
// 
////////////////////////////////////////////////////////////////////////////////

void DestroyDeviceFlag(DeviceFlagID inDeviceFlagID)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	TMK_SuspendScheduling();

	DeviceFlag *pDevFlag = g_DeviceFlagTable.GetObjectPtr(inDeviceFlagID);

	if (NULL != pDevFlag)
	{
		g_DeviceFlagTable.RemoveObject(inDeviceFlagID);
		pDevFlag->~DeviceFlag();
		_heap_free(g_Heaps[g_DeviceFlagHeap].m_Index,pDevFlag);
		HISTORY_(VDK::kDeviceFlagDestroyed, inDeviceFlagID, GetThreadID());
	}

	TMK_ResumeScheduling();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (NULL == pDevFlag)
		DispatchThreadError(kInvalidDeviceFlag, inDeviceFlagID);
#endif
}

}

