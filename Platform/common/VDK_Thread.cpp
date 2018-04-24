/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Abstract base class for a schedulable thread of execution
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Thread is the base class for all threads of execution in the RTOS.
 *  The thread class contain all of the data structure required for pending on
 *  semaphores or events (with or without timeout), waiting in the time queue,
 *  waiting to run in the ready queue or executing.  The class provides two
 *  entry points: Run() and Error().  Run() is a pure virtual function.  Error()
 *  has a default implentation, but may be overridden by a user implementation.
 *  Threads are always accessed by pointers within the kernel, but always only
 *  by ThreadIDs from the public interface to avoid dangling references.  
 *
 *  Threads have two sets of doubly linked list pointers through multiple 
 *  inheritance of ListElement.  Through ThreadListElement, Thread inherits 
 *  pointers that it uses for insertion into the ready queue or for pending on
 *  a semaphore, event or fifo.  Through TimeListElement, Thread inherits a 
 *  second set of pointers that it uses for insertion onto the time queue for 
 *  sleep (pure delay) or for timeout in conjunction with pending.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <stdlib.h>

#include "TMK.h"

#include "VDK_Thread.h"
#include "DebugInfo.h"
#include "VDK_API.h"
#include "VDK_Macros.h"
#include "VDK_Abs_Device.h"
#include "IODeviceTable.h"
#include "Heap.h"
#include "KernelPanic.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

namespace VDK
{
	Ticks g_RoundRobinTimeRemaining[CHAR_BIT * sizeof(int) - 1] = {0};
	extern Ticks    g_RoundRobinInitPeriod[];

#if defined (__ADSPTS__) && (VDK_INSTRUMENTATION_LEVEL_==2)
	void* CycCountVarPtr[1];
#endif

////////////////////////////////////////
// Called when a thread has timed out on the time queue
//
//  A Thread can timeout on 3 conditions: (1) When it is actually sleeping,
//  (2) when it is pending on something and the timeout exprires, and (3) when
//  it is running and its round-robin timeslice expires.
//  When a thread is actually sleeping we just move it back into the ready queue
//  to wake it, and when it has a timeout it sets the m_timeoutOccured flag
//  and moves from the thread list to the ready queue. If the timeslice has expired then
//  a forced yield is performed and the thread remains in the ready queue.
//
////////////////////////////////////////
#pragma param_never_null pTLE
void
Thread::Timeout(TMK_TimeElement *pTLE)
{
#pragma suppress_null_check
	Thread *pSelf = static_cast<Thread*>(pTLE);

	TMK_AcquireMasterKernelLockFromKernelLevel();

    // If a thread was waiting for a resource, and
    // it has timed out, then call the error handler
    // otherwise move the thread to the ready queue
    switch (pSelf->runStatus)
	{
	case kReady:
		// This must be a round-robin timeout. The only ready threads which should be
		// on the time queue are those which are currently running (could be more than
		// one in an SMP system).

		// Refresh the time remaining value for this priority, i.e. start a new
		// timeslice.
		g_RoundRobinTimeRemaining[pSelf->priority] = g_RoundRobinInitPeriod[pSelf->priority];

		// Move the thread to the end of the list for its priority level
		if (!TMK_YieldThread(pSelf))
		{
			// Restart the timeout, as this is the only thread at that priority and hence
			// will continue to run.
			//
			TMK_SetTimeout(pTLE, g_RoundRobinTimeRemaining[pSelf->priority]);
		}
		break;

	default:
		// This must be a timeout on a blocked API call

        // Log the timeout
        HISTORY_(kThreadTimedOut, pSelf->BlockedOnIndex(), pSelf->ID());

        // Remove the thread from the Event/DeviceFlag/Semaphore/Fifo we were waiting for
		TMK_RemoveThreadFromList(pSelf);

        // We want to pass an error back to the API
        pSelf->m_TimeoutOccurred = true;

		// Fall though

	case kSleeping:
		HISTORY_(kThreadStatusChange, pSelf->runStatus, pSelf->ID()); // log previous status
		TMK_MakeThreadReady(pSelf);
	}

	TMK_ReleaseMasterKernelLockFromKernelLevel();
}


int
Thread::ErrorHandler()
{
	// by default, we kill the thread and free its memory in the IDLE thread
    KernelPanic(kThreadError, LastErrorType(), LastErrorValue());
    return 0;
}

} // namespace VDK

