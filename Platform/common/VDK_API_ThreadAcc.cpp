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

	Priority
	GetPriority( const ThreadID inThreadID )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// NB: Disabling scheduling is the user's responsibility

	    // Find the thread
		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

	    // If we have found the thread, return its priority
	    // otherwise, call the calling thread's error function
		if (NULL != pThread) 
		{
			TMK_ReleaseMasterKernelLock();
	    	return static_cast<Priority>(pThread->priority);
		}
		
		TMK_ReleaseMasterKernelLock();

    #if (VDK_INSTRUMENTATION_LEVEL_ >0)
	    DispatchThreadError( kUnknownThread, inThreadID );
    #endif
	    return static_cast<Priority> (UINT_MAX);
	}


	ThreadStatus
	GetThreadStatus( const ThreadID inThreadID )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// NB: Disabling scheduling is the user's responsibility
		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then return its runstatus
		// otherwise, call the error function of the calling thread
		if (NULL != pThread) 
		{
			TMK_ReleaseMasterKernelLock();
			return static_cast<ThreadStatus>(pThread->runStatus);
		}
  		TMK_ReleaseMasterKernelLock();
		return kUnknown;
	}


	ThreadType
	GetThreadType( const ThreadID inThreadID )
	{
		KernelPanic(kDeprecatedAPI, (SystemError) 0,0);
		return static_cast<ThreadType>(0);
	}


	void**
	GetThreadHandle( void )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

		return pThread->DataPtr();
	}

	int
	GetAllThreads( ThreadID outThreadIDArray[], int inArraySize )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		
		// A null array pointer can be used, but only if the 
		// stated array size is 0
		if NERRCHK( (inArraySize != 0) && (outThreadIDArray == NULL) )
			DispatchThreadError( kInvalidPointer, 0 );

		// Disable scheduling while we access a global table
		TMK_AcquireMasterKernelLock();
		int count = g_ThreadTable.GetAllObjectIDs(outThreadIDArray, inArraySize);
		TMK_ReleaseMasterKernelLock();

		return count;
	}


	SystemError
	GetThreadBlockingID( const ThreadID inThreadID, int *outBlockingID )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		
		SystemError errorCode = kUnknownThread;

		if ERRCHK( outBlockingID != NULL )
		{
			// Find the thread
  			TMK_AcquireMasterKernelLock();
			Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

			// If we have found the thread, then get its BlockingID.
			if (NULL != pThread) 
			{
				if(outBlockingID)
				{
					if ( (pThread->runStatus == kMessageBlocked) || (pThread->runStatus == kMessageBlockedWithTimeout) )
					{	
							*outBlockingID = pThread->MessageQueue()->MsgChannelMask();

					}
					else {
						*outBlockingID = pThread->BlockedOnIndex();
					}
				}
				errorCode = kNoError;
			}
			//Otherwise the thread doesn't exist
  			TMK_ReleaseMasterKernelLock();
		}
		else
		{
			// The pointer is not valid, so set the errorCode and return
			errorCode = kInvalidPointer;
		}

			// We have been passed a NULL pointer, set the errorCode and return
		return errorCode;
	}


	SystemError
	GetThreadTemplateName( const ThreadID inThreadID, char **outName)				   
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());		
		
		SystemError errorCode = kUnknownThread;

		// Check that the pointer is valid
		if ERRCHK( outName != NULL )
		{
			// Find the thread
  			TMK_AcquireMasterKernelLock();
			Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

			// If we have found the thread, then get its template name.
			if (NULL != pThread) 
			{
				*outName = pThread->TemplatePtr()->name;
				errorCode = kNoError;
			}
			//Otherwise the thread doesn't exist
  			TMK_ReleaseMasterKernelLock();
		}
		else
		{
			// We have been passed a NULL pointer, set the errorCode and return
			errorCode = kInvalidPointer;
		}
		return errorCode;

	}

} // namespace VDK

