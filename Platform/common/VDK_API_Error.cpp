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
 *  Comments: These f'ns allow the user to test for error in-line with alg code.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "VDK_API.h"
#include "VDK_Thread.h"
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("ErrorHandling")

namespace VDK
{

	void
	SetThreadError(VDK::SystemError inErr, int inVal)
	{
		if NERRCHK(IS_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

		pThread->LastErrorType() = inErr;
		pThread->LastErrorValue() = inVal;
	}


	void
	ClearThreadError( void )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

		pThread->LastErrorType() = VDK::kNoError;
		pThread->LastErrorValue() = 0;
	}


	VDK::SystemError  
	GetLastThreadError( void )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());
	    return pThread->LastErrorType();
	}


	int
	GetLastThreadErrorValue( void )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());
	    return pThread->LastErrorValue();
	}


} // namespace VDK

