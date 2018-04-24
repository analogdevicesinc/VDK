/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "VDK_API.h"
#include "IODeviceTable.h"
#include "VDK_Abs_Device.h"
#include "vdk_macros.h"
#include <limits.h>
#include "KernelPanic.h"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")
#pragma file_attr("Startup")
namespace  VDK
{
	extern unsigned int kNumBootIOObjects;
	extern BootIOObjectInfo g_BootIOObjects[];


// Creates boot devices 
//
bool InitBootIOObjects(void)
{
	for(int i = 0; i < kNumBootIOObjects; i++)
	{
		// Call the API that creates the device driver object
		//
		IOID expectedIOID = CreateIOObject(g_BootIOObjects[i].m_ioTemplateID,
										   &g_BootIOObjects[i].m_instanceNum);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (expectedIOID != i )
		{
			// Log History event  
			// TODO: common boot error instead of per type ??
			// HISTORY_(VDK::kBootIOObjectFailure,i,0) 
			//
			KernelPanic(kBootError, kIOCreateFailure,i);
			if (expectedIOID != UINT_MAX)
				DestroyIOObject(expectedIOID);
		}
#endif
	}
	return true;
}

} /* namespcace VDK */


