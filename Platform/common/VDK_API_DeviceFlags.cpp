/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Device flag API's
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_API.h"
#include "vdk_History.h"
#include "DeviceFlagTable.h"
#include "VDK_Thread.h"
#include "Heap.h"
#include <stdlib.h>

#pragma file_attr("OS_Component=DeviceFlags")
#pragma file_attr("DeviceFlags")

namespace VDK
{
    extern int  g_CriticalRegionCount;
    extern int  g_SchedulerRegionCount;
	extern TMK_Lock      g_CriticalRegionLock;
	extern TMK_LockState g_CriticalRegionState;
	extern int           g_remainingTicks;

	extern void (*g_PFEventRecalcHandler)(); // defined in main.cpp
    
////////////////////////////////////////////////////////////////////////////////
//
// Pend on a device flag. Should be called after PushCriticalRegion()
// 
////////////////////////////////////////////////////////////////////////////////

bool
PendDeviceFlag(DeviceFlagID inFlag, Ticks inTimeout)
{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
	DeviceFlag *pDeviceFlag = g_DeviceFlagTable.GetObjectPtr(inFlag);
	
	// Check that the timeout is within range
	if ERRCHK(INT_MAX != inTimeout && (Ticks) INT_MIN != inTimeout && UINT_MAX != inTimeout)
	{
		if ERRCHK(pDeviceFlag != NULL)
		{
			if ERRCHK((g_CriticalRegionCount * g_SchedulerRegionCount) == 1)
			{
				// Mark the deviceflag to indicate that at least one thread is in
				// mid-pend. Once the interrupt lock is released a device interrupt
				// may be serviced, and a device activation may post the device flag,
				// so we must ensure that nothing falls between the cracks, leaving the
				// thread blocked.
				//
				// Because it is required to hold the critical region
				// lock in order to increment any deviceflag's
				// postCount, we know (because we're holding the
				// critical region lock) that this deviceflag's
				// postCount has not changed since the last critical
				// region was pushed, even if an interrupt has occured
				// and has been serviced by another processor and the
				// resulting activation has tried to post the
				// deviceflag.
				//
				unsigned postCount = pDeviceFlag->postCount;

				// Pop the (outermost) critical region
				g_CriticalRegionCount = 0;
				TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
				g_SchedulerRegionCount = 0;

				// Since PendDeviceFlag() stands in for PopCriticalRegion() it has to
				// do the same Events processing. This will call TMK_ResumeScheduling()
				//
				(*g_PFEventRecalcHandler)();

				// The deviceflag's postCount may have changed since
				// we released the critical region lock, above, but it
				// won't change again after we've acquired the master
				// kernel lock, here. (CAVEAT: this may assume that PostDeviceFlag()
				// will only ever be called from an activation).
				//
				TMK_AcquireMasterKernelLock();

				// Log the event
				HISTORY_(kDeviceFlagPended, inFlag, GetThreadID());

				// If the deviceflag's postCount has changed since we copied it above then
				// a post has occurred between the release of the global interrupt lock
				// and the acquire of the master kernel lock, 
				//
				while (postCount == pDeviceFlag->postCount)
				{
					// ...otherwise we block. It doen't matter if
					// another post occurs (e.g. on another core after
					// we checked the postCount, above) because the
					// post will spin waiting for the master kernel
					// lock until after we've blocked.
					//
					TMK_Thread *pTmkThread = TMK_GetCurrentThread();
#pragma suppress_null_check
					Thread *pThread = static_cast<Thread*>(pTmkThread);
#pragma suppress_null_check
					TMK_TimeElement *pTimeElement = pThread;

					if (inTimeout != 0 && inTimeout != kNoTimeoutError)
					{
						// Take the thread out of the ready queue.
						HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
						TMK_MakeThreadNotReady(pTmkThread, kDeviceFlagBlockedWithTimeout);

						// Put the thread into the time queue. Use TMK_ResetTimeout() instead of
						// TMK_SetTimeout() in case the current thread is round-robin.
						//
						g_remainingTicks = TMK_ResetTimeout(pTimeElement, inTimeout & INT_MAX);   // mask out the sign bit
					}
					else
					{
						HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
						TMK_MakeThreadNotReady(pTmkThread, kDeviceFlagBlocked);
					}

					// Put the thread into the DeviceFlag's queue
					pThread->BlockedOnIndex() = inFlag;
					TMK_AppendThreadToList(pDeviceFlag, pTmkThread);
				
					TMK_Block();
					
					// If we timed out, throw an error
					if (pThread->TimeoutOccurred())
					{
						TMK_ReleaseMasterKernelLock();

						if ((int)inTimeout > 0)   // test sign bit
							DispatchThreadError(kDeviceTimeout, inTimeout);
						pThread->TimeoutOccurred() = false;

						return false;
					}
				}  // while (postCount == pDeviceFlag->postCount)

				TMK_ReleaseMasterKernelLock();

				return true;
			}
			else
				// We have tried to block in an invalid region
				DispatchThreadError(kBlockInInvalidRegion, g_SchedulerRegionCount);
		}
		else
			// We had an invalid DeviceFlagID passed in
			DispatchThreadError(kInvalidDeviceFlag, inFlag);
	}
	else
		// We had an out-of-range timeout passed in
		DispatchThreadError(kInvalidTimeout, inTimeout);

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// Posts a device flag, all threads blocked on this device flag will be put in
// the ready queue.
// 
////////////////////////////////////////////////////////////////////////////////
void
PostDeviceFlag(DeviceFlagID inFlag)
{
	if NERRCHK(IS_USER_ISR_LEVEL()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

	TMK_SuspendScheduling();

	DeviceFlag *pDeviceFlag = g_DeviceFlagTable.GetObjectPtr(inFlag);

	if ERRCHK(pDeviceFlag != NULL )
	{
		// Log the event
		HISTORY_(kDeviceFlagPosted, inFlag, GetThreadID());

		// We have to acquire the critical region lock before we can
		// increment the deviceflag's postCount. We may already be
		// holding it, i.e. if we're inside a critical region.
		//
		if (0 == g_CriticalRegionCount)
			g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

		++pDeviceFlag->postCount;

		if (0 == g_CriticalRegionCount)
			TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);

		TMK_AcquireMasterKernelLock();

		for(;;) // middle-breaking loop
		{
			// Remove the thread from the waiting list
			TMK_Thread *pBlockedThread = TMK_GetNextThreadFromList(pDeviceFlag);

			if (NULL == pBlockedThread) break;  // loop breaks here

#pragma suppress_null_check
			Thread *pVdkThread = static_cast<Thread*>(pBlockedThread);
#pragma suppress_null_check
			TMK_TimeElement *pTimeElement = pVdkThread;

			if (pBlockedThread->runStatus == kDeviceFlagBlockedWithTimeout)
				TMK_CancelTimeout(pTimeElement);  // safe wrt. RescheduleISR

			// Put the thread in the ready queue
			HISTORY_(VDK::kThreadStatusChange, pVdkThread->runStatus, pVdkThread->ID()); // log previous status
			TMK_MakeThreadReady(pBlockedThread);
		}

		TMK_ReleaseMasterKernelLock();
	}
	else
	{
		// If we're actually at kernel level (e.g. device activation)
		// then DispatchThreadError() should call the global panic.
		DispatchThreadError(kInvalidDeviceFlag, inFlag);
	}

	TMK_ResumeScheduling();
}

} /* namespace VDK */

