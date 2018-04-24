/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public Yield and Sleep API functions
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
#include "VDK_Macros.h"
#include "VDK_Thread.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


namespace VDK
{
    extern  int g_SchedulerRegionCount;
    extern unsigned g_RoundRobinInitPriority;
	extern int           g_remainingTicks;
	extern Ticks g_RoundRobinTimeRemaining[CHAR_BIT * sizeof(int) - 1]; // defined in VDK_Thread.cpp
	extern Ticks g_RoundRobinInitPeriod[];  // defined in VDK.cpp

	void
	Yield( void )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#if VDK_INSTRUMENTATION_LEVEL_ > 0
		if (TMK_IsSchedulingSuspended())
	    {
	        // We have tried to perform a Yield, and we are going to block
	        // This is an error state, so throw an error to the current thread
	        DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
			return;
	    }
#endif

		TMK_Thread *pTMKThread = TMK_GetCurrentThread();

		TMK_AcquireMasterKernelLock();

		// If the thread is at a round-robin priority level...
		//
		if (IsBitSet(g_RoundRobinInitPriority, pTMKThread->priority))
		{
			// ...then refresh the time remaining value for this
			// priority and start a new timeslice.

			// Convert to ptr to VDK::Thread so we can reset its TimeElement
#pragma suppress_null_check
			Thread *pVDKThread = static_cast<Thread*>(pTMKThread);

			g_RoundRobinTimeRemaining[pTMKThread->priority] = g_RoundRobinInitPeriod[pTMKThread->priority];
			TMK_ResetTimeout(pVDKThread, g_RoundRobinTimeRemaining[pTMKThread->priority]);
		}

		if (TMK_YieldThread(pTMKThread))
			TMK_Reschedule();

		TMK_ReleaseMasterKernelLock();
	}
	 
	void
	Sleep( const Ticks inSleepTicks )
	{ 
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// You can't sleep for zero ticks, nor for INT_MAX ticks
		if ( inSleepTicks > 0 && inSleepTicks < INT_MAX) // should also use ERRCHK?
		{
	        if ERRCHK(!TMK_IsSchedulingSuspended())
	        {
#pragma suppress_null_check
				Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

				TMK_AcquireMasterKernelLock();

	        	// Take the thread out of the ready queue
				HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
				TMK_MakeThreadNotReady(pThread, kSleeping); // ??

	        	// Set the duration it is going to sleep for. Use TMK_ResetTimeout() instead of
				// TMK_SetTimeout() in case the current thread is round-robin.
				//
				g_remainingTicks = TMK_ResetTimeout(pThread, inSleepTicks); // ??

				TMK_Block();

				TMK_ReleaseMasterKernelLock();
	        }
	        else
			{
	            // We have tried to perform a Sleep, and we are going to block
	            // This is an error state, so throw an error to the current thread
	            DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
			}
	    }
#if VDK_INSTRUMENTATION_LEVEL_ > 0
		// for non-error checking libraries we just want to fall
		// through and not add the item to the sleep queue
	    else
	        // We tried sleeping for an invalid time
			DispatchThreadError( kInvalidDelay, inSleepTicks );
#endif
	}


} // namespace VDK


