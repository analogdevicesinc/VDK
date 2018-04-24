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
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "VDK_History.h"
#include "ThreadTable.h"
#include "IdleThread.h"
#include "Heap.h"
#include "KernelPanic.h"
#include "BitOps.h"
#if (VDK_INSTRUMENTATION_LEVEL_==2)
#include "VDK_StackMeasure.h"
#endif
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("SignalDestruction")
#pragma file_attr("ThreadDestruction")
#pragma file_attr("FuncName=DestroyThread")
#pragma file_attr("FuncName=FreeDestroyedThreads")

namespace VDK
{
	TMK_PFCSwitch g_pfKillThreadChain=NULL ;

    extern int        g_CriticalRegionCount;
    extern int        g_SchedulerRegionCount;

	extern unsigned g_RoundRobinInitPriority;
	extern int      g_remainingTicks;

	void 
	CleanupThread(Thread *pThread)
	{
		++g_SchedulerRegionCount;

		pThread -> ~Thread();
	        HISTORY_(VDK::kThreadResourcesFreed, pThread->ID(), GetThreadID());
		_heap_free(g_Heaps[pThread->TemplatePtr()->threadStructHeap].m_Index, pThread);

		--g_SchedulerRegionCount;
	}

	// This function always runs on the system stack, immediately prior to switching
	// to the new thread.
#pragma param_never_null pOldThread pNewThread
	void KillThreadHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread)
	{
#pragma suppress_null_check
		Thread *pDestroyedThread = static_cast<Thread*>(pOldThread);

		// Do the VDK-specific cleanup
		CleanupThread(pDestroyedThread);

		// Chain to the next function in the line, pass NULL
		// as oldThread argument since the old thread doesn't exist
		// any more.
		//
		(*g_pfKillThreadChain)(NULL, pNewThread);
	}

	void
	DestroyThread( const ThreadID inThreadID, bool destroyNow )
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
		// Freeze the thread table and the Thread's runstatus
		TMK_AcquireMasterKernelLock();

	    Thread *pThread = g_ThreadTable.GetObjectPtr( inThreadID );
		bool destroyingSelf = (TMK_GetCurrentThread() == pThread);
	
		// If the thread being destroyed *is* the running thread, then
		// we *must* reschedule. So if the current thread is
		// destroying itself within an unscheduled region then we
		// can't continue.
		if (destroyingSelf && TMK_IsSchedulingSuspended())
			KernelPanic(kThreadError, kThreadDestroyedInInvalidRegion, inThreadID);

	    // Check to see if we got the thread
	    if (NULL != pThread)
	    {
#if (VDK_INSTRUMENTATION_LEVEL_==2)
	        // Log the thread's destruction
	        HISTORY_(VDK::kThreadDestroyed, pThread->ID(), GetThreadID());
#endif
#pragma suppress_null_check
			TMK_Thread *pTmkThread = pThread;
#pragma suppress_null_check
			TMK_TimeElement *pTimeElement = pThread;

	        // Remove the thread from the readyqueue or any resources it's pending on
	        switch(pThread->runStatus)
	        {
			case kReady:
				// Remove the thread from the ready queue.
			    HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status

				if (IsBitSet(g_RoundRobinInitPriority, pThread->priority))
					g_remainingTicks = TMK_CancelTimeout(pThread);

				TMK_MakeThreadNotReady(pTmkThread, kUnknown);
				break;
			case kSleeping:
				g_remainingTicks = TMK_CancelTimeout(pTimeElement); // safe wrt. RescheduleISR
				pThread->runStatus = kUnknown; // don't know the current state of the thread anymore (HACK!)
				break;
			case kSemaphoreBlockedWithTimeout:
			case kEventBlockedWithTimeout:
			case kDeviceFlagBlockedWithTimeout:
			case kMessageBlockedWithTimeout:
				g_remainingTicks = TMK_CancelTimeout(pTimeElement); // safe wrt. RescheduleISR
			case kSemaphoreBlocked:
			case kEventBlocked:
			case kDeviceFlagBlocked:
		    case kMessageBlocked:
		    case kMutexBlocked:
				TMK_RemoveThreadFromList(pTmkThread);
				pThread->runStatus = kUnknown; // don't know the current state of the thread anymore (HACK!)
				break;
			default:
				KernelPanic(kInternalError,kInvalidThreadStatus,pThread->ID());
				break;
	        }

			// Remove the thread from the thread table
			g_ThreadTable.RemoveObject(pThread->ID());

#if (VDK_INSTRUMENTATION_LEVEL_==2)
			//
			// Log maximum stack used by this thread as an event.
			//
			unsigned int StackUsed = CheckMemory(pThread->m_StackP, pThread->m_StackSize,STACK_FILL_VALUE);
			HISTORY_(VDK::kMaxStackUsed, StackUsed, pThread->ID());
#ifdef __ADSPTS__
			unsigned int Stack2Used = CheckMemory(pThread->m_StackP2, pThread->m_StackSize,STACK_FILL_VALUE);
			HISTORY_(VDK::kMaxStackUsed, Stack2Used, pThread->ID());
#endif
#endif
            // If we are destroying a thread that IS NOT ourselves,
            // then we actually want to delete the thread....  If we HAD actually
            // called delete right here on the currently running thread we would
            // delete the stack and call schedule....
            if (destroyNow)
			{
				if (destroyingSelf)
					TMK_Stop();	// Won't return
				else // not destroying the current thread
					CleanupThread(pThread);
            }
			else	// deferred cleanup
			{
		        // Add the thread to the list of Threads that the idle thread must delete
		        g_IdleThreadPtr->AddToKillList(pThread);

				// We want to call the scheduler at the end of this, if necessary
				if (destroyingSelf)
					TMK_Reschedule();
			}
		}

		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	    if (NULL == pThread)  // thread not found in table
			DispatchThreadError(kUnknownThread, inThreadID);
#endif
	}


	void
	FreeDestroyedThreads()
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

		while (g_IdleThreadPtr->KillThreadsInPurgatory())
		{
			// returns false when kill list is empty...
		}
	}






} // Namespace VDK

