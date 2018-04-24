/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions for thread
 *	 usage and information.
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

#include "TMK.h"

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("HistoryAndStatus")

namespace VDK 
{

	// Return the stack used so far in the instrumented build.
	// It will return 0 in un-instrumented builds.
	//
	unsigned int 
	GetThreadStackUsage(const ThreadID inThreadID)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		TMK_AcquireMasterKernelLock();
	    Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		if (NULL != pThread)
		{
			TMK_ReleaseMasterKernelLock();
			return (pThread->GetStackUsed());
		}
		else
		{
			TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_ >0)
			DispatchThreadError( kUnknownThread, inThreadID );
#endif
			return 0;
		}
	}


#ifdef __ADSPTS__
	unsigned int
	GetThreadStack2Usage(const ThreadID inThreadID)
	{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
    	TMK_AcquireMasterKernelLock();
    	Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );
		if (NULL != pThread)
		{
			TMK_ReleaseMasterKernelLock();
			return (pThread->GetStackUsed2());
		}
		else
		{
			TMK_ReleaseMasterKernelLock();
#if (VDK_INSTRUMENTATION_LEVEL_ >0)
			DispatchThreadError( kUnknownThread, inThreadID );
#endif
			return 0;
		}
	}
#endif


	SystemError
	GetThreadStackDetails( const ThreadID	inThreadID, 
						   unsigned int 	*outStackSize,
						   void 			**outStackAddress )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// NB: Disabling scheduling is the user's responsibility
		
		SystemError errorCode = kUnknownThread;

		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then get its stack details.
		if (NULL != pThread) 
		{
			if(outStackSize != NULL)
			{
				*outStackSize = pThread->StackSize();
			}
			if(outStackAddress != NULL)
			{
				*outStackAddress = static_cast<void *>(pThread->Stack());
			}
			errorCode = kNoError;
		}
		//Otherwise the thread doesn't exist
  		TMK_ReleaseMasterKernelLock();
		
		return errorCode;
	}


#ifdef __ADSPTS__
	SystemError
	GetThreadStack2Details( const ThreadID	inThreadID, 
							unsigned int 	*outStackSize,
							void 			**outStackAddress )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// NB: Disabling scheduling is the user's responsibility
		
		SystemError errorCode = kUnknownThread;

		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then return its stack2 details.
		if (NULL != pThread) 
		{
			if(outStackSize != NULL)
			{
				*outStackSize = pThread->Stack2Size();
			}
			if(outStackAddress != NULL)
			{
				*outStackAddress = static_cast<void *>(pThread->Stack2());
			}
			errorCode = kNoError;
		}
		//Otherwise the thread doesn't exist
  		TMK_ReleaseMasterKernelLock();
		
		return errorCode;
	}
#endif

} // namespace VDK

