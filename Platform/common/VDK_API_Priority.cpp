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
#include "ThreadTable.h"
#include "SemaphoreTable.h"
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


namespace VDK
{
	extern Ticks    g_RoundRobinInitPeriod[];
	extern unsigned g_RoundRobinInitPriority;

	void
	SetPriority( const ThreadID inThreadID, const Priority inPriority )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kInvalidThread;
		int errorValue = inThreadID;

		// If the thread ID isn't the idle thread
		if ERRCHK(0 != inThreadID)
		{
			// If the priority is valid
			if ERRCHK((inPriority >= kMinPriority) && (inPriority <= kMaxPriority))
			{
				TMK_AcquireMasterKernelLock();

				// Find the thread
				Thread *pThread = g_ThreadTable.GetObjectPtr( inThreadID );

				// If we have found the thread, then see about resetting its priority
				if ERRCHK(pThread != 0)
				{
					errorCode = kNoError;

					// don't do anything if the priority hasn't changed 
					if (pThread->priority != inPriority)	
					{
						Thread *pCurrentThread = NULL;

						HISTORY_(VDK::kThreadPriorityChanged, inPriority, pThread->ID());

						switch(pThread->runStatus)
						{
	                    case kReady:
							TMK_MakeThreadNotReady(pThread, kUnknown); // put thread in limbo
#pragma suppress_null_check
							pCurrentThread = static_cast<Thread*>(TMK_GetCurrentThread());

							if (pThread == pCurrentThread)
							{
								// If we are changing the priority of the running thread then we
								// must take account of round-robin issues.
								//
								// If the old priority is an RR level then cancel the timeout
								if (IsBitSet(g_RoundRobinInitPriority, pThread->priority))
									TMK_CancelTimeout(pThread);

								pThread->priority = inPriority;

								// If the new priority is an RR level then re-establish the timeout
								if (IsBitSet(g_RoundRobinInitPriority, pThread->priority))
									TMK_SetTimeout(pThread, g_RoundRobinInitPeriod[pThread->priority]);
							}
							else
							{
								pThread->priority = inPriority;
							}

							TMK_MakeThreadReady(pThread);

							// It is only if we've changed the priority of a ready thread that we need
							// to re-evaluate scheduling, changing the priority of a blocked thread
							// will never require a context switch.
							//
							TMK_Reschedule();
	                        break;
	                    case kSemaphoreBlocked:
	                    case kSemaphoreBlockedWithTimeout:
							TMK_RemoveThreadFromList(pThread);
							pThread->priority = inPriority;
							g_SemaphoreTable.GetObjectPtr(static_cast<SemaphoreID> (pThread->BlockedOnIndex()))->AddPending(pThread);
	                        break;
	                    case kEventBlocked:
	                    case kEventBlockedWithTimeout:
	                    case kDeviceFlagBlocked:
	                    case kDeviceFlagBlockedWithTimeout:
	                        // Events & Device Flags don't need to be in priority order
							// (since all the threads get unblocked at once), so there's
							// nothing for us to do here.
	                    case kUnknown:
	                    default:
							pThread->priority = inPriority;
	                        break;
						}
					}
				}
				else
				{
					// we haven't found the thread
					errorCode  = kUnknownThread;
					errorValue = inThreadID;
				}

				TMK_ReleaseMasterKernelLock();
			}
			else
			{
				errorCode  = kInvalidPriority;
				errorValue = inPriority;
			}
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError( errorCode, errorValue );
#endif
	}

	void
	ResetPriority( const ThreadID inThreadID )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (0 == inThreadID)
		{
			DispatchThreadError(kInvalidThread, inThreadID);
			return;
		}
#endif

	    // Find the thread
		TMK_SuspendScheduling();
		Thread *pThread = g_ThreadTable.GetObjectPtr( inThreadID );

	    // If we found the thread, then set its priority
	    // otherwise, call the error function of the calling thread
		if( NULL != pThread )
	    	SetPriority( inThreadID, pThread->DefaultPriority() );

		TMK_ResumeScheduling();
	    
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if( NULL == pThread )
			DispatchThreadError( kUnknownThread, inThreadID );
#endif
	}

} // namespace VDK


