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

#ifndef _DEVICE_FLAG_TABLE_
#define _DEVICE_FLAG_TABLE_

#include "VDK_API.h"
#include "IDTable.h"
#include "VDK_DeviceFlag.h"


namespace VDK
{
    typedef IDTable<VDK::DeviceFlag,VDK::DeviceFlagID> DeviceFlagTable;

    extern DeviceFlagTable g_DeviceFlagTable;

} // namespace VDK


#endif  // _DEVICE_FLAG_TABLE_ 
