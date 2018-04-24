/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the thread error access APIs
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: DispatchThreadError which is always linked in
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "VDK_API.h"
#include "VDK_Thread.h"
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("ErrorHandling")

extern "C" bool vdkMainMarker;


namespace VDK
{

	int
	DispatchThreadError(VDK::SystemError inErrorNum, const int inErrorValue)
	{
		if (TMK_AtKernelLevel())
		{
			if ( true == vdkMainMarker)
				KernelPanic(kInternalError, inErrorNum, inErrorValue);
			else
				KernelPanic(kBootError, inErrorNum, inErrorValue);
		}

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

        if (pThread->ShouldDispatchError() == true)
        {
    		pThread->LastErrorType() = inErrorNum;
	    	pThread->LastErrorValue() = inErrorValue;

		return pThread->ErrorHandler();
        }

        return 0;
	}

} // namespace VDK

