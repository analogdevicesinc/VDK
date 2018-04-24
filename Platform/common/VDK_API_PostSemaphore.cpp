/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public PostSemaphore API function
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
#include "VDK_History.h"
#include "VDK_Thread.h"
#include "SemaphoreTable.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


namespace VDK
{
	void
	PostSemaphore( const SemaphoreID inSemaphoreID )
	{
#if (VDK_INSTRUMENTATION_LEVEL_ >0)
		if (IS_USER_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
#endif

		// May be called from kernel level (i.e. device activation)
		TMK_AcquireMasterKernelLock();  // freeze the queues (and the semaphore table)

	    Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemaphoreID);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		// Check to see that the semaphore we're posting actually exists.
		if (NULL == pSemaphore)
		{
	        // The semaphore we're trying to post doesn't exist
			TMK_ReleaseMasterKernelLock();   // unfreeze

			// If we're actually in kernel mode (i.e. device
			// activation) then DispatchThreadError() should call the
			// global panic.
			DispatchThreadError( kUnknownSemaphore, inSemaphoreID );
			return;
		}
#endif
		// Log the event
		// We need to possibly set the high bit of the value passed to the HISTORY_ call
		// so that we specify if the semaphore's value has changed
		HISTORY_(VDK::kSemaphorePosted, inSemaphoreID | (pSemaphore->m_Value ? INT_MIN : 0), GetThreadID());

		// If there is a thread waiting on the semaphore, unblock it

		if (TMK_IsThreadListEmpty(&pSemaphore->m_PendingThreads))
		{
			// No threads waiting, increment the semaphore's count
			TMK_LockState state = TMK_AcquireGlobalInterruptLock(&pSemaphore->m_lock);

			if (pSemaphore->m_Value < pSemaphore->m_MaxCount)
				++pSemaphore->m_Value;

			TMK_ReleaseGlobalInterruptLock(&pSemaphore->m_lock, state);
		}
		else
		{
			// We might expect that if we get here (i.e. there are threads in
			// the semaphore's pending queue) that the semaphore's m_Value must
			// necessarily be zero.
			//
			// However, if an interrupt occurs (as it conceivably could) between
			// the reschedule interrupt being masked (above) and here, and if that
			// interrupt has called VDK_ISR_POST_SEMAPHORE on this semaphore,
			// then the value will have been incremented but Semaphore::Invoke()
			// has not yet been able to run and remove the thread from the pending
			// queue.
			//
			// In this event, we will have unblocked the thread here, and if it was
			// the only one in the queue then Invoke() will find itself with nothing
			// to do.
			//
			// None of this has any direct bearing on the operation of this function
			// (at present) but I thought I should mention it while it is clear in
			// what passes for my mind.
			//
#pragma suppress_null_check
			Thread *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pSemaphore->m_PendingThreads));
			
			// Remove from the time queue
			if (pThread->runStatus == kSemaphoreBlockedWithTimeout)
				TMK_CancelTimeout(pThread);  // safe wrt. RescheduleISR

			// Put the thread in the ready queue
			HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
			TMK_MakeThreadReady(pThread);

			TMK_Reschedule();
		}

		// Unfreeze the queues and the semaphore table
		TMK_ReleaseMasterKernelLock();   // a pre-emptive cswitch *could* happen now
	}

} // namespace VDK



