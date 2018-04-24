/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public Critial region and unscheduled region 
 *					API functions.
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
#pragma file_attr("OS_Component=Regions")
#pragma file_attr("Regions")

namespace VDK
{
    int           g_CriticalRegionCount = 1;      // We don't want to start in a critical region
    int           g_SchedulerRegionCount = 1;     // We *do* want to start in an unscheduled region
	TMK_Lock      g_CriticalRegionLock = TMK_INIT_GLOBAL_INTERRUPT_LOCK(TMK_MIN_LOCK_PRECEDENCE);
	TMK_LockState g_CriticalRegionState = 0;
	void        (*g_PFEventRecalcHandler)() = TMK_ResumeScheduling;

	////////////////////////////////////////
	// Critical Region functions
	////////////////////////////////////////

	void 
	PushCriticalRegion( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_USER_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif
		if (0 == g_SchedulerRegionCount)
			TMK_SuspendScheduling();

		// It's only safe to modify the scheduler region count once we know that
		// scheduling is suspended.
		++g_SchedulerRegionCount;

		if (0 == g_CriticalRegionCount)
			g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

		// It's only safe to modify the critical region count once we know that
		// interrupts are disabled.
        ++g_CriticalRegionCount;
	}

	void
	PopCriticalRegion( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_USER_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		if (g_CriticalRegionCount == 0)
	    {
	        DispatchThreadError( kDbgPopUnderflow, kFromPopCriticalRegion );
	        return;
	    }
#endif
        if (--g_CriticalRegionCount <= 0)
		{
			g_CriticalRegionCount = 0; // probably unnecessary
            TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
		}

        if (--g_SchedulerRegionCount <= 0)
		{
			g_SchedulerRegionCount = 0; // probably unnecessary
			(*g_PFEventRecalcHandler)();
		}
	}


	void
	PopNestedCriticalRegions( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_USER_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		if(g_CriticalRegionCount == 0)
	    {
	        DispatchThreadError( kDbgPopUnderflow, kFromPopNestedCriticalRegions );
	        return;
		}
#endif
        // If popping all critical regions would pop all unscheduled regions as well, 
        // call the scheduler.
		// pop only nested critical regions from the scheduler region count
		g_SchedulerRegionCount -= g_CriticalRegionCount;
		g_CriticalRegionCount = 0;
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);

        if (0 == g_SchedulerRegionCount)
			(*g_PFEventRecalcHandler)();
	}

	////////////////////////////////////////
	// Scheduler functions
	////////////////////////////////////////

	void
	PushUnscheduledRegion( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif
		if (0 == g_SchedulerRegionCount)
			TMK_SuspendScheduling();

		++g_SchedulerRegionCount;
	}

	void
	PopUnscheduledRegion( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	    // In error-checking build, if we have no Scheduled regions, or the scheduled
	    // regions we have were caused by calls to PushCriticalRegion,
	    // we want to throw an error
		if ((g_SchedulerRegionCount == 0) || (g_SchedulerRegionCount == g_CriticalRegionCount))
		{
			DispatchThreadError( kDbgPopUnderflow, kFromPopUnscheduledRegion );
			return;
		}
#endif
        if (--g_SchedulerRegionCount <= 0)
		{
			g_SchedulerRegionCount = 0;  // probably unnecessary
			(*g_PFEventRecalcHandler)();
		}
	}

	void
	PopNestedUnscheduledRegions( void )
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		if ((g_SchedulerRegionCount == 0) || (g_SchedulerRegionCount == g_CriticalRegionCount))
		{
			DispatchThreadError( kDbgPopUnderflow, kFromPopNestedUnscheduledRegions );
			return;
		}
#endif
		g_SchedulerRegionCount = g_CriticalRegionCount;

	    if (0 == g_SchedulerRegionCount)
			(*g_PFEventRecalcHandler)();
	}

} // Namespace VDK

