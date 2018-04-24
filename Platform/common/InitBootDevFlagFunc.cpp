/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"
#include "vdk_macros.h"
#include "VDK_DeviceFlag.h"
#include "DeviceFlagTable.h"
#include "VDK_History.h"
#include <limits.h>
#include "KernelPanic.h"
#pragma file_attr("OS_Component=DeviceFlags")
#pragma file_attr("DeviceFlags")
#pragma file_attr("Startup")

using namespace VDK;
// Creates boot device flags
//
namespace VDK
{
	extern unsigned int kNumBootDeviceFlags;

	bool InitBootDeviceFlags(void)
	{
		for(int i = 0; i < kNumBootDeviceFlags; i++)
		{
			// Call the API that creates the device flag
			//
			DeviceFlagID expectedDeviceFlagID = CreateDeviceFlag();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
			// We should check the value given by ID table manager
			// 
			if (expectedDeviceFlagID != i )
			{
				KernelPanic(kBootError,kDeviceFlagCreationFailure,i);
				// Log History event  
				// TODO: common boot error instead of per type ??
				// HISTORY_(VDK::kBootDeviceFlagCreateFailure,i,0) 
				//
				if (expectedDeviceFlagID != UINT_MAX)
					DestroyDeviceFlag(expectedDeviceFlagID);
			}
#endif
		}
		return true;
	}
} /* namespace vdk */
