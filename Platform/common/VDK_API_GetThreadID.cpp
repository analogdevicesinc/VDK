/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API function GetThreadID
 *
 *   Last modified $Date$
 *       Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "VDK_API.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

namespace VDK {
    ThreadID
    GetThreadID()
    {
	if (TMK_AtKernelLevel())  {
		return (ThreadID) VDK_KERNEL_LEVEL_;
	}
	else {
#pragma suppress_null_check
	Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());
	    // Return the current thread's ID...
	    return pThread->ID();
	}
    }
}
