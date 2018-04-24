/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A single semaphore with a list of pending threads & timeouts
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Semaphores provide a mechanism that prevents multiple clients 
 *  from accessing the same system resource inappropriately..
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#include "Semaphore.h"
#include "VDK_Thread.h"
#include <new>
#include "TMK.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


/** ----------------------------------------------------------------------------
 * FUNC   : VDK::Semaphore::Semaphore()
 *
 * DESC   : Constructor for a semaphore class.
 *
 * PARAMS : enum_value, initial_value, initial_delay, period
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Semaphore::Semaphore( VDK::SemaphoreID inEnumValue, unsigned int inInitialValue, 
								unsigned int inMaxCount,
								VDK::Ticks inInitialDelay, VDK::Ticks inPeriod)
								: m_MaxCount(inMaxCount), m_Value(inInitialValue), 
								  m_Period(inPeriod)
{
	// The semaphore functions may log history while holding the semaphore lock,
	// so the semaphore lock must be of lower precedence than the history buffer
	// lock (which currently has precedence TMK_MAX_LOCK_PRECEDENCE == 255).
	//
	TMK_InitGlobalInterruptLock(&m_lock, TMK_MAX_LOCK_PRECEDENCE - 1);
	TMK_InitTimeElement(this, Timeout);
	TMK_InitDpcElement(this,Invoke);
	TMK_InitThreadList(&m_PendingThreads);

	m_InitialDelay = inInitialDelay;
	ID() = inEnumValue;
}

////////////////////////////////////////
// Called when the the Semaphore is removed from the DPC Queue.
// Called from RescheduleISR, so the reschedule interrupt
// is masked.
////////////////////////////////////////
#pragma param_never_null pDpcQE
void
VDK::Semaphore::Invoke(TMK_DpcElement *pDpcQE)
{
#pragma suppress_null_check
	Semaphore *pSelf = static_cast<Semaphore*>(pDpcQE);

	TMK_AcquireMasterKernelLockFromKernelLevel();

	// We come in here expecting that m_Value will be >0 and that
	// m_PendingThreads will not be empty because, after all, that's
	// why VDK_ISR_POST_SEMAPHORE queued the semaphore for DPC in the
	// first place.
	//
	// However, it is possible (just) for the interrupt which led to
	// the VDK_ISR_POST_SEMAPHORE to have occurred inside
	// PostSemaphore, after the reschedule interrupt has been masked.
	// In this event, if there was only one thread in the semaphore's
	// pending queue then PostSemaphore() will have removed it by the
	// time the reschedule interrupt is unmasked and Semaphore::Invoke
	// gets to run. In which case we'll fall out of the loop below at
	// the first break and the Invoke() will become a no-op.
	//
	while (pSelf->m_Value)
	{
		if (TMK_IsThreadListEmpty(&pSelf->m_PendingThreads))
		{
			//
			// We're leaving the loop early as there are no more
			// pending threads to wake up. We apply the max count
			// limit here as it is *not* applied by VDK_ISR_POST_SEMAPHORE
			// if there are threads pending (in case m_MaxCount is less
			// than the number of pending threads).
			//
			// Doesn't need to be synchronised with interrupt level (?)
			// 
			if (pSelf->m_Value > pSelf->m_MaxCount)
				pSelf->m_Value = pSelf->m_MaxCount;
			break;
		}

		TMK_LockState state = TMK_AcquireGlobalInterruptLock(&pSelf->m_lock);

		// If there is a thread waiting on the semaphore, acquire it.
		// We know that the list isn't empty, so pTmkThread will not be NULL.
		TMK_Thread *pTmkThread = TMK_GetNextThreadFromList(&pSelf->m_PendingThreads);
		assert(NULL != pTmkThread);
#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(pTmkThread);

		--pSelf->m_Value;

		TMK_ReleaseGlobalInterruptLock(&pSelf->m_lock, state);

		// Remove the thread from the TimeQueue (if necessary)
		if (pThread->runStatus == kSemaphoreBlockedWithTimeout)
			TMK_CancelTimeout(pThread);  // safe wrt. RescheduleISR
	
		// Put the thread in the ready queue
		HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
		TMK_MakeThreadReady(pThread);
	}

	TMK_ReleaseMasterKernelLockFromKernelLevel();
}


////////////////////////////////////////
// This function is called with the reschedule interrupt masked
////////////////////////////////////////
void
VDK::Semaphore::AddPending(VDK::Thread *inThreadP)
{
#pragma suppress_null_check
    TMK_Thread *pTmkThread = inThreadP;
	TMK_AddThreadToOrderedList(&m_PendingThreads, pTmkThread);
}

////////////////////////////////////////
// Called when the Periodic queue times out the Semaphore.
// Called from RescheduleISR, so the reschedule interrupt
// is masked.
////////////////////////////////////////
#pragma param_never_null pTLE
void
VDK::Semaphore::Timeout(TMK_TimeElement *pTLE)
{
#pragma suppress_null_check
	Semaphore *pSelf = static_cast<Semaphore*>(pTLE);

	TMK_AcquireMasterKernelLockFromKernelLevel();

	// Log the event
	// We need to possibly set the high bit of the value passed to the HISTORY_ call
	// so that we specify if the semaphore's value has changed
	HISTORY_(VDK::kSemaphorePosted, pSelf->ID() | (pSelf->m_Value? INT_MIN:0), GetThreadID());

	// If there is a thread waiting on the semaphore, unblock it
	if (TMK_IsThreadListEmpty(&pSelf->m_PendingThreads))
	{
		// No threads waiting, increment the semaphore's count
		TMK_LockState state = TMK_AcquireGlobalInterruptLock(&pSelf->m_lock);

		if (pSelf->m_Value < pSelf->m_MaxCount)
			++pSelf->m_Value;
		
		TMK_ReleaseGlobalInterruptLock(&pSelf->m_lock, state);
	}
	else
	{
		// We know that the list isn't empty, so pThread will not be NULL.
		TMK_Thread *pTmkThread = TMK_GetNextThreadFromList(&pSelf->m_PendingThreads);
#pragma suppress_null_check
		Thread *pVdkThread = static_cast<Thread*>(pTmkThread);
			
		// Remove the thread from the TimeQueue (if necessary)
		if (pVdkThread->runStatus == kSemaphoreBlockedWithTimeout)
			TMK_CancelTimeout(pVdkThread);  // safe wrt. RescheduleISR
	
		// Put the thread in the ready queue
		HISTORY_(VDK::kThreadStatusChange, pVdkThread->runStatus, pVdkThread->ID()); // log previous status
		TMK_MakeThreadReady(pTmkThread);
	}

    // Put the semaphore back in the timequeue
	TMK_SetTimeout(pSelf, pSelf->m_Period);

	TMK_ReleaseMasterKernelLockFromKernelLevel();
}


