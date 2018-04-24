/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _IO_DEVICE_TABLE_
#define _IO_DEVICE_TABLE_

#include "IDTable.h"
#include "VDK_ABS_DEVICE.h"

namespace VDK
{
    typedef IDTable<VDK::IOAbstractBase,VDK::IOID> IODeviceTable;

    extern IODeviceTable g_IOdevTable;

} /* namespace VDK */

#endif /* _IO_DEVICE_TABLE_ */
