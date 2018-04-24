/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public PendEvent API function
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: This API is separate from Set/ClearEventBit because they may
 *  not need to be linked in if the setting/clearing is done from ISR
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_API.h"
#include "BitOps.h"
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "Event.h"
#include "EventBit.h"
#pragma file_attr("OS_Component=Events")
#pragma file_attr("Events")

	////////////////////////////////////////
	// EVENTS
	////////////////////////////////////////
namespace VDK {
    extern int        	g_SchedulerRegionCount;
	extern int           g_remainingTicks;


bool
PendEvent( const EventID inEventID, const Ticks inTimeout )
{
	if NERRCHK(IS_ISR_LEVEL()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	// Check that the timeout is within range
	if (INT_MAX == inTimeout || (Ticks) INT_MIN == inTimeout)
	{
		// We had an out-of-range timeout passed in
		DispatchThreadError(kInvalidTimeout, inTimeout);
		return false;
	}

	// Check to see that the event we're pending on exists.
	if (inEventID >= kNumEvents)
	{
		// We tried to pend on an unknown event
		DispatchThreadError(kUnknownEvent, inEventID);
		return false;
	}
#endif

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (TMK_IsSchedulingSuspended() && kDoNotWait != inTimeout)
	{
		// We are going to perform a Pend, and we may block.
		// This is a possible error state, so throw an error to the current thread
		DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
	}
#endif

	// We can't have DPCs changing the Thread's RunStatus etc. or the event's value
	TMK_AcquireMasterKernelLock();

	Event *pEvent = &g_Events[inEventID];
	bool event_value = pEvent->Value();

	// We only want to block (and call scheduler) if it's appropriate
	if (event_value == true)
	{
		// Log the event
		// Set the high bit to show the event's current value
		HISTORY_(VDK::kEventPended, inEventID | INT_MIN, GetThreadID());
				
		TMK_ReleaseMasterKernelLock();
	}
	else // (event_value == false)
	{
		// Log the event
		HISTORY_(VDK::kEventPended, inEventID, GetThreadID());

		// If inTimeout is 0xFFFFFFFF then don't block, return failure
		if (kDoNotWait == inTimeout)
		{
			TMK_ReleaseMasterKernelLock();
			return false;
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (TMK_IsSchedulingSuspended())
        {
            // We have tried to perform a Pend, and we are going to block.
            // This is an error state, so throw an error to the current thre
			TMK_ReleaseMasterKernelLock();

			DispatchThreadError(kBlockInInvalidRegion, g_SchedulerRegionCount);
			return false;
		}
#endif

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

		// Move the thread to the appropriate queues.
		pThread->BlockedOnIndex() = inEventID;

		if (inTimeout != 0 && inTimeout != kNoTimeoutError)
		{
			HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
			TMK_MakeThreadNotReady(pThread, kEventBlockedWithTimeout);

			// Put the thread into the time queue. Use TMK_ResetTimeout() instead of
			// TMK_SetTimeout() in case the current thread is round-robin.
			//
			g_remainingTicks = TMK_ResetTimeout(pThread, inTimeout & INT_MAX);   // mask out the sign bit
		}
		else
		{
			HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
			TMK_MakeThreadNotReady(pThread, kEventBlocked);
		}

		// Put the thread onto the event's pending list
		TMK_AppendThreadToList(&pEvent->m_PendingThreads, pThread);

		TMK_Block();

		TMK_ReleaseMasterKernelLock();
	        
		// Check the error status.  This is set in Thread::Timeout()
		if (pThread->TimeoutOccurred() == true)
		{
			if ((int)inTimeout > 0)   // test sign bit
				DispatchThreadError(kEventTimeout, inTimeout);
			pThread->TimeoutOccurred() = false;
			
			return false;
	    }
	}

	return true;
}

} //namespace VDK
