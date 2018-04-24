/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Device flag API's
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef _VDK_DEVICE_FLG_
#define _VDK_DEVICE_FLG_

#include "TMK.h"

#include "VDK_API.h"
#include "IDTable.h"

namespace VDK
{
extern HeapID g_DeviceFlagHeap;
class DeviceFlag;
//BEGIN_INTERNALS_STRIP
class DeviceFlag : public IDElementTemplate<DeviceFlagID>, public TMK_ThreadList
{
	friend bool PendDeviceFlag(DeviceFlagID inFlag, Ticks inTimeout);
	friend void PostDeviceFlag(DeviceFlagID inFlag);

 public:
	 DeviceFlag(DeviceFlagID id) 
	 {
		 TMK_InitThreadList(this);
		 ID() = id; 
		 postCount = 0;
	 }

 private:
	 volatile unsigned postCount;
};
//END_INTERNALS_STRIP
} /* namespace VDK */
#endif /* _VDK_DEVICE_FLG_ */

