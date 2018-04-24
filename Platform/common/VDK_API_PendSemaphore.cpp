/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public PendSemaphore API functions
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
#include "VDK_History.h"
#include "SemaphoreTable.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


namespace VDK
{
	extern int           g_remainingTicks;
	extern int           g_SchedulerRegionCount;

	bool
	PendSemaphore( const SemaphoreID inSemaphoreID, const Ticks inTimeout )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// Check that the timeout is within range
		if (INT_MAX == inTimeout || (Ticks) INT_MIN == inTimeout)
		{
			// We had an out-of-range timeout passed in
			DispatchThreadError(kInvalidTimeout, inTimeout);
			return false;
		}

		if (TMK_IsSchedulingSuspended() && kDoNotWait != inTimeout)
		{
			// We are going to perform a Pend, and we may block.
			// This is a possible error state, so throw an error to the current thread
			DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
		}
#endif
		int newStatus = (0 == inTimeout)
			? kSemaphoreBlocked : kSemaphoreBlockedWithTimeout;

		// Get the semaphore pointer from the object table
		TMK_AcquireMasterKernelLock();
	    Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemaphoreID);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		// Check to see that the semaphore we're pending on exists.
		if (NULL == pSemaphore)
		{
			// We tried to pend on an unknown semaphore
			TMK_ReleaseMasterKernelLock();
			DispatchThreadError( kUnknownSemaphore, inSemaphoreID );
			return false;
		}
#endif
		// We don't want the Semaphore value to change while we're deciding
		// what to do. The saved imask will have the reschedule interrupt's
		// bit *unset*, because we masked it above.
		TMK_LockState state = TMK_AcquireGlobalInterruptLock(&pSemaphore->m_lock);

		// We only want to throw the error below if we are going to block
		// and we're in an unscheduled region. Do the valid cases first.
		if (pSemaphore->m_Value > 0)           // not going to block
		{
			// decrement the semaphore's value
			--pSemaphore->m_Value;
  
			// No rescheduling to be done
			TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&pSemaphore->m_lock, state);

			// Log the event, now that ints are enabled again
			HISTORY_(VDK::kSemaphorePended, inSemaphoreID | INT_MIN, GetThreadID());
		}
		else
		{
			// If inTimeout is 0xFFFFFFFF then don't block, return failure
			if (kDoNotWait == inTimeout)
			{
				TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&pSemaphore->m_lock, state);
				HISTORY_(VDK::kSemaphorePended, inSemaphoreID, GetThreadID());

				return false;
			}
				
#if (VDK_INSTRUMENTATION_LEVEL_>0)
			if (TMK_IsSchedulingSuspended())  // in an unscheduled region
			{
				// We have tried to perform a Pend, and we are going to block.
				// This is an error state, so throw an error to the current thread.
				TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&pSemaphore->m_lock, state);
				DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );

				return false;
			}
#endif
			// We're going to block
#pragma suppress_null_check
			Thread *pCurrentThread = static_cast<Thread*>(TMK_GetCurrentThread());

			// Remove the thread from the ReadyQueue
			HISTORY_(VDK::kThreadStatusChange, pCurrentThread->runStatus, pCurrentThread->ID()); // log previous status
			TMK_MakeThreadNotReady(pCurrentThread, newStatus);

			// There *must* be at least one thread in the semaphore's
			// pending queue before we leave the critical region, as
			// VDK_ISR_POST_SEMAPHORE won't queue the semaphore for
			// DPC if its pending queue is empty.
			//
			// Therefore, if the pending queue is currently empty then
			// we must insert the current thread now (quick as no
			// sorting needed), otherwise we can do the time-consuming
			// (and non-deterministic) work of inserting into the
			// priority-ordered queue - and the TimeQueue - from
			// outside the critical region. In the meantime we are
			// safe in the knowledge that nothing will be removed from
			// the semaphore's pending queue until we re-enable the
			// reschedule interrupt.
			//
			if (TMK_IsThreadListEmpty(&pSemaphore->m_PendingThreads))
			{
				// There are no threads currently pending on this semaphore,
				// so we can just insert this one into the (empty) list
				// without worrying about priority ordering.
				//
				TMK_AppendThreadToList(&pSemaphore->m_PendingThreads, pCurrentThread);
				TMK_ReleaseGlobalInterruptLock(&pSemaphore->m_lock, state); // reschedule is still masked
			}
			else
			{			
				// We have threads present, the insertion is in priority order - NOT FIFO!
				// This is the special case when there are already threads waiting on
				// the ThreadList. We can release the interrupt lock *before* doing the
				// (non-deterministic) ordered list insertion.
				//
				TMK_ReleaseGlobalInterruptLock(&pSemaphore->m_lock, state); // reschedule is still masked
				TMK_AddThreadToOrderedList(&pSemaphore->m_PendingThreads, pCurrentThread);
			}

			// The global interrupt lock has now been released, but
			// the master kernel lock is still held. Now we do the
			// things that didn't need to be inside the interrupts-off
			// region.
			HISTORY_(VDK::kSemaphorePended, inSemaphoreID, GetThreadID());
			pCurrentThread->BlockedOnIndex() = inSemaphoreID;

			// Register for timeout, if required. Use TMK_ResetTimeout() instead of
			// TMK_SetTimeout() in case the current thread is round-robin.
			//
			if (kSemaphoreBlockedWithTimeout == newStatus)
				g_remainingTicks = TMK_ResetTimeout(pCurrentThread, inTimeout & INT_MAX);   // mask out the sign bit

			// Block
			TMK_Block();

			// We've been unblocked, either by a semaphore post or by timeout
			//
			TMK_ReleaseMasterKernelLock();

			// Check the error status.  This is set in Thread::Timeout()
			if (pCurrentThread->TimeoutOccurred())
			{
				if ((int)inTimeout > 0)   // test sign bit
					DispatchThreadError(kSemaphoreTimeout, inTimeout);
				pCurrentThread->TimeoutOccurred() = false;

				return false;
			}
		}

		return true;
	}

	unsigned int
	GetSemaphoreValue( const SemaphoreID inSemID )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif
		unsigned int result = UINT_MAX;

		// Check to see that the semaphore we're looking at exists.
		TMK_AcquireMasterKernelLock();

	    Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemID);
		if (NULL != pSemaphore)
	    	  result = pSemaphore->m_Value;
		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (NULL == pSemaphore)
			DispatchThreadError( kUnknownSemaphore, 0 );
#endif

	    return result;
	}


} // namespace VDK

