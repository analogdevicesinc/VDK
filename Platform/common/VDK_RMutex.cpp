/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the semi-private Mutex API functions.
 *
 *   Mutex is a recursive thread-level mutex, with optional (limited) support for
 *   priority inheritance. It is presently intended only for use by the C/C++
 *   runtime libraries, and therefore omits some functionality that might be
 *   expected in a full public API. For example:
 *
 *   - there is no automatic release if thread dies while holding a mutex.
 *   - threads waiting on the mutex are not maintained in priority order.
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
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "ThreadTable.h"
#include "SemaphoreTable.h"
#include "KernelPanic.h"
#include "VDK_History.h"

// #define PRIORITY_INHERITANCE
#ifndef __ADSPTS__
// TS does not like suppressing this warning
#pragma diag(suppress:1994:" multicharacter character literal potential portability problem  ")
#endif

#include "VDK_RMutex.h"

#pragma file_attr("OS_Component=Mutex")
#pragma file_attr("Mutex")

namespace VDK
{
	extern int			g_CriticalRegionCount;
	extern int			g_SchedulerRegionCount;

#ifdef PRIORITY_INHERITANCE
	// Defined at end of module
	static void inheritPriority  (Mutex *pMutex);
	static void deInheritPriority(Mutex *pMutex);
#endif

/****************************************************************
 * VDK::InitMutex
 *
 * Creates a mutex in an area of memory provided by the caller.
 *
 * We do this so that a mutex can be created without doing any
 * heap memory allocation, since the heap itself may require an
 * mutex for mutual exclusion.
 ****************************************************************/
void InitMutex(Mutex *pMutex, unsigned size, bool inLogHistory)
{
	void *pRawMemory = pMutex;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	// Fail if the memory area isn't large enough
	if (size < sizeof(Mutex))
		KernelPanic(kInternalError, kMutexSpaceTooSmall, size);
#endif
	TMK_InitThreadList(&pMutex->pendingThreads);

	// Initialize the "plain" fields. We only need to do 'level' as
	// 'owner' and 'ownerOriginalPriority' are don't-cares when level
	// is zero.
	pMutex->level = 0;

    // we don't want to log history for the RTL which only uses this 
    // function but we want to do it for users who use CreateMutex
	if (inLogHistory)
        pMutex->ID() = MUTEX_SIGNATURE;
	else
        pMutex->ID() = RTL_MUTEX_SIGNATURE;
}


/****************************************************************
 * VDK::DeInitMutex
 *
 * Undo the initialisation of the mutex.
 ****************************************************************/
void DeInitMutex(Mutex *pMutex)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (pMutex->level != 0 || !TMK_IsThreadListEmpty(&pMutex->pendingThreads))
		KernelPanic(kInternalError, kMutexDestructionFailure, (int)pMutex);
#endif
        pMutex->ID() = DEAD_MUTEX_SIGNATURE;
}

/****************************************************************
 * VDK::AcquireMutex
 *
 * Get ownership of a mutex, blocking if necessary.
 *
 * The "recursive" property of the mutex means that the owner can
 * perform nested acquires of the mutex. The 'level' and 'owner'
 * fields keep track of this.
 ****************************************************************/
void AcquireMutex(MutexID inMutexID)
{
	Mutex *pMutex = reinterpret_cast<Mutex *>(inMutexID);
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

	if (TMK_IsSchedulingSuspended())
	{
		// We are going to perform a Acquire, and we may block.
		// This is a possible error state, so throw an error to the current thread
		DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
	}
	if ERRCHK(!MTX_TST(pMutex) ) {
        DispatchThreadError(kInvalidMutexID, inMutexID);
        return;
	}
#endif


	TMK_AcquireMasterKernelLock();	 // freeze thread level

#if (VDK_INSTRUMENTATION_LEVEL_>0)
// check that the mutex owner still exists
        if (pMutex->level > 0) 
        {
#pragma suppress_null_check
            Thread* pStoredOwner = static_cast<Thread*> (pMutex->pOwner);
	        Thread* pMutexOwner= g_ThreadTable.GetObjectPtr(pStoredOwner->ID());
	        if (pMutexOwner!= pStoredOwner)
            {
		        // does the mutex owner still exist? 
		        DispatchThreadError( kInvalidMutexOwner, pStoredOwner->ID() );
		        pMutex->level = 0;

            }
        }
#endif

	TMK_Thread *pCurrentThread = TMK_GetCurrentThread();

#if (VDK_INSTRUMENTATION_LEVEL_ == 2)
    if (USER_MTX(pMutex))
        HISTORY_(VDK::kMutexAcquired, inMutexID, GetThreadID());
#endif

	if (0 == pMutex->level)
	{
		// No thread owns the mutex, so we can take ownership of it
		//
		pMutex->pOwner = pCurrentThread;
		pMutex->level = 1;
#ifdef PRIORITY_INHERITANCE
		pMutex->ownerOriginalPriority = pCurrentThread->priority;
#endif
	}
	else if (pCurrentThread == pMutex->pOwner)
	{
		// The current thread owns the mutex, so we just have to increment the level count
		++pMutex->level;
	}
	else
	{

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	    if (TMK_IsSchedulingSuspended())
        {
            TMK_ReleaseMasterKernelLock();
            DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
            TMK_AcquireMasterKernelLock();
        }

#endif
#pragma suppress_null_check
		Thread *pVDKThread = static_cast<Thread*>(pCurrentThread);

		// The mutex is owned by another thread, so we have	to block.
		//
		// Remove the thread from the ReadyQueue.
		HISTORY_(VDK::kThreadStatusChange, pVDKThread->runStatus, pVDKThread->ID()); // log previous status
		TMK_MakeThreadNotReady(pCurrentThread, kMutexBlocked);

		// And put it on the list of threads waiting for the mutex,
		// in priority order.
		TMK_AddThreadToOrderedList(&pMutex->pendingThreads, pCurrentThread);

		// Mark the thread as being blocked on the mutex. Since there
		// is no mutex table (and hence no integer mutex ID) we just
		// store the mutex address in the BlockedOnIndex field.
		pVDKThread->BlockedOnIndex() = reinterpret_cast<int>(pMutex);

#ifdef PRIORITY_INHERITANCE
		inheritPriority(pMutex);
#endif

		TMK_Block();

		// We're here because ownership of the mutex has been
		// handed over to this thread (by the previous owner).
		//
	}

	TMK_ReleaseMasterKernelLock();  // unfreeze thread level
}

/****************************************************************
 * VDK::ReleaseMutex
 *
 * Release one level of mutex ownership. If the nesting level is
 * greater than one then this will not actually relinquish ownership.
 *
 * If the nesting level is one then ownershipo will be released, and
 * the first waiting thread will be unblocked. If this thread is of
 * highter priority than the current thread then there may be a context
 * switch.
 ****************************************************************/
void ReleaseMutex(MutexID inMutexID)
{
	Mutex *pMutex = reinterpret_cast<Mutex *>(inMutexID);
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

	if (!MTX_TST(pMutex) ) {
        DispatchThreadError(kInvalidMutexID, inMutexID);
        return;
    }

	// Verify that the mutex is currently owned...
	if (0 == pMutex->level)
	{
		DispatchThreadError(kMutexNotOwned, 0);
		return;
	}

	// ...and that the owner is the current thread.
	if (TMK_GetCurrentThread() != pMutex->pOwner)
	{
		DispatchThreadError(kNotMutexOwner, static_cast<Thread*>(pMutex->pOwner)->ID());
		return;
	}
#endif

	TMK_AcquireMasterKernelLock();	 // freeze thread level

	// At this point we know (or assume) that the current thread owns
	// the mutex and that the mutex level count is non-zero.

#if (VDK_INSTRUMENTATION_LEVEL_ == 2)
    if (USER_MTX(pMutex))
        HISTORY_(VDK::kMutexReleased, inMutexID, GetThreadID());
#endif

	if (pMutex->level > 1)
	{
		// The current thread is retaining ownership of the mutex,
		// just decrement the level.
		--pMutex->level;
	}
	else // 1 == pMutex->level
	{
		// The current thread is about to release ownership of the
		// mutex, so we check to see if any other thread is waiting on
		// it.
		//
		// Take the first thread off the waiting list,
		TMK_Thread *pNewOwner = TMK_GetNextThreadFromList(&pMutex->pendingThreads);

		if (NULL == pNewOwner)
		{
			// No threads waiting, set the mutex state to un-owned.
			// The 'owner' field is a don't-care when level is zero,
			// so we don't change it (the fact the fact that the ID of
			// the last owner is retained may also be useful for
			// diagnostic purposes).
			//
			pMutex->level = 0;
		}
		else // there is a thread waiting
		{
			// make it the owner of the mutex (the mutex level count is still 1),
			pMutex->pOwner = pNewOwner;

			// and put it in the ready queue.
#pragma suppress_null_check
			Thread *pVdkNewOwner = static_cast<Thread*>(pNewOwner);
			HISTORY_(VDK::kThreadStatusChange, pVdkNewOwner->runStatus, pVdkNewOwner->ID()); // log previous status
			TMK_MakeThreadReady(pNewOwner);

#ifdef PRIORITY_INHERITANCE
			deInheritPriority(pMutex);
			pMutex->ownerOriginalPriority = pNewOwner->priority;
#endif

			TMK_Reschedule();
		}
	}

	TMK_ReleaseMasterKernelLock();  // unfreeze thread level
}

#ifdef PRIORITY_INHERITANCE

/*
 * Priority inheritance involves boosting the priority of the owner to
 * that of the highest-priority waiting thread, to avoid the
 * possibility of priority inversion (i.e. a high-priority thread
 * waiting on a mutex owned by a pre-empted low-priority thread).
 *
 * Priority inheritance only comes into play when there is a change in
 * the list of threads waiting on the mutex. This happens when:
 * 
 * - a thread blocks on the mutex
 * - the mutex is released and ownership passes to a waiting thread
 * - the priority of a waiting thread changes, either due to a SetPriority()
 *   call or to a priority inheritance event on a different mutex.
 *
 * When a thread is added to the list we must ensure that the
 * effective priority of the mutex's current owner is equal to or
 * greater than that of the thread at the head of the waiting
 * list. (The waiting list is held in priority order, so the first
 * thread in the list will be the highest priority - all being well.)
 * This is made more complicated by the fact that the owner thread
 * may not be in the running state.
 */
static void 
inheritPriority(Mutex *pMutex)
{
	assert(NULL != pMutex->pOwner);
#pragma suppress_null_check
	Thread *pOwner = static_cast<Thread*>(pMutex->pOwner);
	TMK_Thread *pWaiter = TMK_QueryNextThreadInList(&pMutex->pendingThreads);
	
	if (pOwner->priority < pWaiter->priority)	
	{
		HISTORY_(VDK::kThreadPriorityChanged, pWaiter->priority, pOwner->ID());

		switch(pOwner->runStatus)
		{
		case kReady:
			TMK_MakeThreadNotReady(pOwner, kUnknown);
			pOwner->priority = pWaiter->priority;
			TMK_MakeThreadReady(pOwner);
			break;
		case kSemaphoreBlocked:
		case kSemaphoreBlockedWithTimeout:
			TMK_RemoveThreadFromList(pOwner);
			pOwner->priority = pWaiter->priority;
			g_SemaphoreTable.GetObjectPtr(static_cast<SemaphoreID> (pOwner->BlockedOnIndex()))->AddPending(pOwner);
			break;
		case kMutexBlocked:
			// We ought really to recurse here if the owner is blocked on a different mutex, so as
			// to propagate the inheritance of priority. Another time, perhaps.
			TMK_RemoveThreadFromList(pOwner);
			pOwner->priority = pWaiter->priority;
			{
				Mutex *pBlockingMutex = (reinterpret_cast<Mutex*> (pOwner->BlockedOnIndex()));
				TMK_AddThreadToOrderedList(&pBlockingMutex->pendingThreads, pOwner);
				inheritPriority(pBlockingMutex);
			}
			break;
		case kEventBlocked:
		case kEventBlockedWithTimeout:
		case kDeviceFlagBlocked:
		case kDeviceFlagBlockedWithTimeout:
	                            // Events & Device Flags don't need to be in priority order!!!
		case kUnknown:
		default:
			pOwner->priority = pWaiter->priority;
			break;
		}
	}
}

/*
 * When the highest priority thread is removed from the head of the
 * list (because the previous owner has released the mutex) we know
 * that its priority is greater than or equal to that of any other
 * thread in the list. Therefore all we have to do is make sure that
 * the previous owner is restored to its original priority.
 */
static void
deInheritPriority(Mutex *pMutex)
{
	// Restore the owner's original priority level, if necessary
	TMK_Thread *pCurrentThread = TMK_GetCurrentThread();

	if (pMutex->ownerOriginalPriority != pCurrentThread->priority)
	{
		// Note that if the thread has had its priority changed (via
		// SetPriority()) while it was holding the mutex then that change
		// will be undone here. A more thorough implementation of
		// priority inheritance would need to check for this scenario
		// in SetPriority().
		//
		HISTORY_(VDK::kThreadPriorityChanged, pMutex->ownerOriginalPriority, static_cast<Thread*>(pCurrentThread)->ID());

		TMK_MakeThreadNotReady(pCurrentThread, kUnknown);
		pCurrentThread->priority = pMutex->ownerOriginalPriority;
		TMK_MakeThreadReady(pCurrentThread);
	}
}
#endif

}; // namespace VDK
