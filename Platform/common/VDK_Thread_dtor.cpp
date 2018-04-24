/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *  $RCSfile$
 *
 *  Description: Thread destructor
 *
 *  Last modified $Date$
 *  Last modified by $Author$
 *  $Revision$
 *  $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#include <stdlib.h>

#include "TMK.h"

#include "VDK_Thread.h"
#include "DebugInfo.h"
#include "VDK_API.h"
#include "VDK_Macros.h"
#include "VDK_Abs_Device.h"
#include "IODeviceTable.h"
#include "Heap.h"
#include "KernelPanic.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("SignalDestruction")
#pragma file_attr("ThreadDestruction")

namespace VDK
{

	// Avoid driver stuff when there are no drivers. The below
	// function pointer is initialized in OpenDevice API. When device
	// drivers are not used it will be NULL.
	//
    IOAbstractBase* (*GetIOObjectPtr) (IOID inDevID) = NULL;


Thread::~Thread()
{
    FreeThreadMemory(*m_TemplatePtr); // calls TMK_DeInitThread()

    //
    // Closing all the open DCBs can cause a possible bug.
    //
#pragma suppress_null_check
    Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());
    
    bool old_value = pThread->ShouldDispatchError();
    pThread->ShouldDispatchError() = false;

    // Close all the open DCBs
    for (DeviceDescriptor dcb_num = static_cast <DeviceDescriptor> (0); dcb_num < kNumDCBs; dcb_num = static_cast <DeviceDescriptor> (dcb_num + 1) )
    {
        DispatchUnion t;

        DeviceControlBlock  *pDCB = GetDCB(dcb_num);

        // Make sure the device control block is valid
        if (NULL != pDCB && NULL != GetIOObjectPtr)
        {
            // Call the device dispatch routine
            //
            t.OpenClose_t.dataH = pDCB->DD_DataPointer();
            IOAbstractBase *pIODevice = GetIOObjectPtr(pDCB->WhichDevice());

            if (NULL != pIODevice)
                pIODevice->DispatchFunction(kIO_Close,t);

            // The DeviceControlBlock isn't being used anymore
            FreeDCB(dcb_num);
        }
    }

    //
    // Cleanup any thread-local storage
    //
    for (int slot = 0;
         slot < kMaxNumDataSlots;
         ++slot)
    {
        if (IsBitClr(c_UnusedSlots, slot)   &&     // if the slot is allocated
            NULL != c_CleanupFns[slot] &&     // and has a cleanup function
            NULL != m_SlotValue[slot])        // and there is data...
        {
            // ...then cleanup the data
            (*c_CleanupFns[slot]) (m_SlotValue[slot]);
            m_SlotValue[slot] = NULL;         // optional
        }
    }

    pThread->ShouldDispatchError() = old_value;
}

}
