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
#include "VDK_Thread.h"

#include "VDK_Abs_Device.h"
#include "vdk_driver.h"
#include "IODeviceTable.h"
#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")


namespace VDK
{
    void
    CloseDevice(DeviceDescriptor    inDD)
    {
#pragma suppress_null_check
	Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

	// Get the device control block
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	VDK::DeviceControlBlock  *pDCB = pThread->GetDCB(inDD);
	IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(pDCB->WhichDevice());

	// Make sure the device control block is valid
	if ERRCHK(NULL != pDCB  && NULL != pIODevice)
	{
	    VDK::DispatchUnion t;

	    // Call the device dispatch routine
	    t.OpenClose_t.dataH = pDCB->DD_DataPointer();
	    pIODevice->DispatchFunction(kIO_Close,t);

	    // The DeviceControlBlock isn't being used anymore
	    pThread->FreeDCB(inDD);
	}
	else
	    // we were passed a bad dcb
	    DispatchThreadError(kBadDeviceDescriptor, inDD);
    }
}


