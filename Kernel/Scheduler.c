/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file Scheduler.c
 * Contains the scheduler functions.
 *
 * The scheduler ties together the services of the ready queue
 * and the context switch, to provide the higher-level scheduling
 * primitives which are exported to the macrokernel.
 */
#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"

#if defined(__ADSPBLACKFIN__)

#include <sys/platform.h>

#define RESCHEDULE_INT_LEVEL   EVT_IVG14

#elif defined(__ADSPTS20x__)

#include <defts201.h>

#define RESCHEDULE_INT_LEVEL   INT_KERNEL_64

#elif defined(__ADSPTS101__)

#include <defts101.h>

#define RESCHEDULE_INT_LEVEL   INT_TIMER0L_64

#elif defined(__ADSP21000__)

#if defined(__ADSP21065L__)
#include <def21065l.h>
#elif defined (__ADSP21261__)
#include <def21261.h>
#elif defined (__ADSP21262__)
#include <def21262.h>
#elif defined (__ADSP21266__)
#include <def21266.h>
#elif defined (__ADSP21267__)
#include <def21267.h>
#elif defined (__ADSP21363__)
#include <def21363.h>
#elif defined (__ADSP21364__)
#include <def21364.h>
#elif defined (__ADSP21365__)
#include <def21365.h>
#elif defined (__ADSP21366__)
#include <def21366.h>
#elif defined (__ADSP21367__)
#include <def21367.h>
#elif defined (__ADSP21368__)
#include <def21368.h>
#elif defined (__ADSP21369__)
#include <def21369.h>
#elif defined (__ADSP21371__)
#include <def21371.h>
#elif defined (__ADSP21469__)
#include <def21469.h>
#elif defined (__ADSP21479__)
#include <def21479.h>
#elif defined (__ADSP21160__)
#include <def21160.h>
#elif defined (__ADSP21161__)
#include <def21161.h>
#elif defined (__ADSP21061__)
#include <def21061.h>
#elif defined (__ADSP21062__)
#include <def21062.h>
#elif defined (__ADSP21060__)
#include <def21060.h>
#else
#error "Unsupported processor detected"
#endif

#define RESCHEDULE_INT_LEVEL   SFT3I

#else
#error Unknown processor family
#endif

#pragma file_attr("ISR")
#pragma file_attr("OS_Internals")

///////////////////////////////////////////////////////////////////////////////
//   Scheduler Functions
///////////////////////////////////////////////////////////////////////////////


/** Context-switches into the (new) highest-priority runnable thread.
 *
 * This function forms part of the "glue" between the ready queue and the context switch.
 * It is called whenever one or more threads have been added to the ready queue,
 * when the priority of a thread has been changed or when the order of threads
 * within a priority level has changed.
 */
void
TMK_Reschedule(void)
{
	/**@pre Must be called with the master kernel lock held. */
    assert(tmk.masterLock.lockHeld);

    if (0 == tmk.schedulerRegionCount)
	{
		/**@pre If scheduling isn't suspended then the
		 * master kernel lock must be the only lock held.
		 */
		assert(1 == lck_InterruptLocksHeld);

		// Get the new thread ready to run
		TMK_Thread *pOldThread = tmk.pCurrentThread;
		TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
   
		// If the thread that should run isn't the thread that's running...
		if (pOldThread != pNewThread)
		{
			// ...then context-switch to the new thread
			tmk.pCurrentThread = pNewThread;

			// Switch to the new context, saving only the preserved state
			(*tmk.vpfCtxtSwitch[TMK_RUNTIMEHOOK])(pOldThread, pNewThread);
		}
	}
}


/** Context-switches into the (new) highest-priority runnable thread.
 *
 * This function forms part of the "glue" between the ready queue and the context switch.
 * It is called whenever the current thread has been removed from the ready queue.
*/
void
TMK_Block(void)
{
	/**@pre Must be called with the master kernel lock held. */
    assert(tmk.masterLock.lockHeld);
    /**@pre The Master Kernel Lock must be the only lock held. */
	assert(1 == lck_InterruptLocksHeld);
	/**@pre Must not be called with scheduling suspended */
    assert(0 == tmk.schedulerRegionCount);
	/**@pre The current thread must have left the ready state */
	assert(READY != tmk.pCurrentThread->runStatus);

	// Get the new thread ready to run
    TMK_Thread *pOldThread = tmk.pCurrentThread;
	TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
   
    // The new thread should *not* be the same as the thread that's running
    assert(pOldThread != pNewThread);
    
	// Context-switch to the new thread, saving only the preserved state
	tmk.pCurrentThread = pNewThread;
	(*tmk.vpfCtxtSwitch[TMK_RUNTIMEHOOK])(pOldThread, pNewThread);
}


/** Terminates execution of the current thread.
 *
 *  @return never
 */
void
TMK_Stop(void)
{
	/**@pre Must be called with the master kernel lock held. */
    assert(tmk.masterLock.lockHeld);
    /**@pre The Master Kernel Lock must be the only lock held. */
	assert(1 == lck_InterruptLocksHeld);
	/**@pre Must not be called with scheduling suspended. */
    assert(0 == tmk.schedulerRegionCount);
    
    TMK_Thread *pOldThread = tmk.pCurrentThread;
    TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
    
    assert(LIMBO == pOldThread->runStatus);
    assert(pOldThread != pNewThread);
    
    // Context-switch to the new thread
    tmk.pCurrentThread = pNewThread;

    // This call never returns
    tmk_RunOnSystemStack(pOldThread, pNewThread, tmk.vpfCtxtSwitch[TMK_KILLTHREADHOOK]);
}


/** Starts running the first thread.
 *
 *  This function forms part of the "glue" between the ready queue and the context switch.
 *  @return never
 */
void
TMK_Run(void)
{
	/**@pre Must be called with scheduling suspended by one level */
	assert(1 == tmk.schedulerRegionCount);
	/**@pre The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
    /**@pre Must be called with no locks held */
	assert(0 == lck_InterruptLocksHeld);
	
	// equiv to TMK_AcquireMasterKernelLock();
	//
    assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
    assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);

    tmk.masterState = LCK_AcquireRescheduleInterruptLock(&tmk.masterLock);

	--tmk.schedulerRegionCount;

    // Get the first thread ready to run
    TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
	
	/**@pre The new thread must not have been run yet, */
    assert(NOTRUN == pNewThread->contextLevel);
	/**@pre The new thread must be in the ready state. */
    assert(READY == pNewThread->runStatus);

    // Context-switch to the new thread
    tmk.pCurrentThread = pNewThread;

    // Switch to the first thread, saving *no* state.
    //
    // The dummy first argument (old thread) is there to allow all (three)
    // of the low-level context-switch functions - and their associated
    // function pointer types - to have the same signature. Since
    // TMK_RunThread() is called only once, during startup, this is
    // an acceptable overhead.
    (*tmk.vpfCtxtSwitch[TMK_FIRSTTIMEHOOK])(NULL, pNewThread);
}


/** Hooks the context switch chains.
 *
 * Installs a function into one of the context-switch hook functions.
 *
 * @param[in] pfHookFn - the function to be installed
 * @param which - indicates which chain to hook
 * @return the previous head of the chain
 */
TMK_PFCSwitch
TMK_HookThreadSwitch(TMK_HookSelector which, TMK_PFCSwitch pfHookFn)
{
	/**@pre Must be called with scheduling suspended. */
    assert(0 != tmk.schedulerRegionCount);
    assert(which < TMK_NUM_HOOK_FNS);

    TMK_PFCSwitch prev = tmk.vpfCtxtSwitch[which];
	tmk.vpfCtxtSwitch[which] = pfHookFn;
    
    return prev;
}


/** Hooks the dispatch chain.
 *
 * Installs a function at the head of the dispatch chain.
 *
 * @param[in] pfHookFn - the function to be installed
 * @return the previous head of the chain
 */
TMK_PFDispatch
TMK_HookDispatch(TMK_PFDispatch pfHookFn)
{
	/**@pre Must be called with scheduling suspended. */
    assert(0 != tmk.schedulerRegionCount);

    TMK_PFDispatch prev = tmk.pfDispatchHook;
	tmk.pfDispatchHook = pfHookFn;
    
    return prev;
}


/** Decrements the region count and resumes scheduling if it is zero.
 *
 * Assumes that the ready queue may have been changed while scheduling
 * was suspended.
 */
void
TMK_ResumeScheduling(void)
{
	/**@pre The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
    assert(!tmk.masterLock.lockHeld);
	/**@pre Must be called with scheduling suspended. */
    assert(0 != tmk.schedulerRegionCount);

    if (0 == --tmk.schedulerRegionCount)
	{
		/**@pre There must be no locks held, unless the scheduler
		 * region count is non-zero.
		 */
		assert(0 == lck_InterruptLocksHeld);

		// equiv. to TMK_AcquireMasterKernelLock()
		//
		assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
		assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
    
		tmk.masterState = LCK_AcquireRescheduleInterruptLock(&tmk.masterLock);
    
		// Get the new thread ready to run
		TMK_Thread *pOldThread = tmk.pCurrentThread;
		TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
   
		// If the thread that should run isn't the thread that's running...
		if (pOldThread != pNewThread)
		{
			// ...then context-switch to the new thread
			tmk.pCurrentThread = pNewThread;

			// Switch to the new context, saving only the preserved state
			(*tmk.vpfCtxtSwitch[TMK_RUNTIMEHOOK])(pOldThread, pNewThread);
		}

		// equiv. to TMK_ReleaseMasterKernelLock();
		//
		LCK_ReleaseRescheduleInterruptLock(&tmk.masterLock, tmk.masterState);
	}
}


/** @internal
 *  @brief The pre-emptive scheduler
 *
 *  Called from within the reschedule ISR once the C runtime
 *  has been set up. This function calls the dpc queue to
 *  dispatch all currently-queued dpcs and then reschedules
 *  based on the new state of the ready queue.
 */
void
tmk_Dispatcher(void)
{
    /**@pre The reschedule interrupt must not be masked */
//	assert_bf(__builtin_sysreg_read(IMASK) & EVT_IVG14);
	/**@pre The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
	/**@pre There should be no other locks held. */
	assert(0 == lck_InterruptLocksHeld);
	/**@pre The interrupted thread should have no context
	 * currently saved (or may have interrupt context saved on
	 * SHARC or TigerSHARC). */
#if 0
	assert_bf(NOCONTEXT == tmk.pCurrentThread->contextLevel);
	assert_ts(NOCONTEXT == tmk.pCurrentThread->contextLevel ||
			  INTERRUPTED == tmk.pCurrentThread->contextLevel);
	assert_sh(NOCONTEXT == tmk.pCurrentThread->contextLevel ||
			  INTERRUPTED == tmk.pCurrentThread->contextLevel);
#endif
    /*@internal
	 * For the duration of DPC processing we:
	 * - set the current thread ptr to NULL in case the dpc code
	 *   tries to access the current thread.
	 * - increment the scheduler region count in case the dpc code
	 *   tries to do a reschedule.
	 */
    TMK_Thread *pInterruptedThread = tmk.pCurrentThread;
    tmk.pCurrentThread = NULL;
    ++tmk.schedulerRegionCount;
    
    /*@internal
	 * We don't hold the kernel lock while dispatching DPCs.
	 * In SMP this would allow two or more cores to be
	 * dispatching DPCs at the same time (which should work
	 * fine). It also makes it ok for the DPCs themselves to
	 * call functions which acquire and release the kernel lock.
	 */
    tmk_DispatchDPCs();
    
	// Restore the current thread pointer (may be changed again below)
    tmk.pCurrentThread = pInterruptedThread;
    
    // Round-robin will be handled via DPC
    
    if (0 == --tmk.schedulerRegionCount)   // i.e. was zero at time of ISR
    {
        // equiv. to TMK_AcquireMasterKernelLockFromKernelLevel();
		//
		assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
		assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
    
		TMK_AcquireInterruptLockFromInterruptLevel(&tmk.masterLock);

        // Get the new thread ready to run
        TMK_Thread *pNewThread = tmk_GetHighestPriorityThread();
       
        // If the thread that should run isn't the thread that's running...
        if (pNewThread != pInterruptedThread)
        {
            // ...then context-switch to the new thread
            tmk.pCurrentThread = pNewThread;
    
            // Switch to the new context, this call will never return...
            (*tmk.vpfCtxtSwitch[TMK_INTERRUPTHOOK])(pInterruptedThread, pNewThread);
            // ...so we'll never get here
            assert(false);
        }
        
        // equiv. to TMK_ReleaseMasterKernelLockFromKernelLevel();
		//
		TMK_ReleaseInterruptLockFromInterruptLevel(&tmk.masterLock);
    }
}
