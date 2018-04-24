/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is the VDK Internals header
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Description: The internals generator utility generates a single include file
 *  to provide all declarations for the VDK libraries at user build time.  To
 *  include a declaration in VDK_Internals.h, add the header files to the list
 *  below and use the following comments to limit user visibility:
 *		// BEGIN_INTERNALS_STRIP
 *		// END_INTERNALS_STRIP
 * 	These comments should begin in the first column.  The make/install process
 *  will generate the desired output.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "Semaphore.h"
#include "Event.h"
#include "EventBit.h"
#include "VDK_IntVector.h"
#include "VDK_MemPool.h"
#include "MemoryPoolTable.h"
#include "ThreadTable.h"
#include "VDK_DeviceFlag.h"
#include "DeviceFlagTable.h"
#include "SemaphoreTable.h"
#include "VDK_Abs_Device.h"
#include "IODeviceTable.h"
#include "VDK_Driver.h"
#include "IDE_typedefs.h"
#include "Message.h"
#include "Heap.h"


