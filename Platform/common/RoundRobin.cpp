/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


#include <stdlib.h>

#include "TMK.h"

#include "VDK_Thread.h"
#include "VDK_API.h"
#include "KernelPanic.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("RoundRobin")


namespace VDK
{
extern unsigned g_RoundRobinInitPriority;
extern Ticks g_RoundRobinTimeRemaining[CHAR_BIT * sizeof(int) - 1]; // defined in VDK__Thread.cpp
extern Ticks g_RoundRobinInitPeriod[];  // defined in VDP.cpp

extern int g_remainingTicks;

TMK_PFCSwitch pfRuntimeRRChain;
TMK_PFCSwitch pfInterruptRRChain;

static TMK_PFCSwitch pfFirstTimeRRChain;
static TMK_PFCSwitch pfKillThreadRRChain;

extern void RoundRobinRuntimeBypass(TMK_Thread *pOldThread, TMK_Thread *pNewThread);
extern void RoundRobinInterruptBypass(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

#pragma param_never_null pOldThread pNewThread
void RoundRobinRuntimeHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread)
{
#pragma suppress_null_check
	Thread *pOldVdkThread = static_cast<Thread*>(pOldThread);
#pragma suppress_null_check
	Thread *pNewVdkThread = static_cast<Thread*>(pNewThread);

	if (IsBitSet(g_RoundRobinInitPriority,pOldThread->priority))
	{
	    // Only cancel the outgoing thread's timeout (which it may or
	    // may not have anyway) if we're not going to accidentally
	    // cancel an API timeout or sleep().
	    //
		int remainingTicks;

	    switch(pOldThread->runStatus)
	    {
	    case kSemaphoreBlockedWithTimeout:
	    case kEventBlockedWithTimeout:
	    case kDeviceFlagBlockedWithTimeout:
	    case kSleeping:
	    case kMessageBlockedWithTimeout:
			// For these cases we do nothing, as the outgoing thread's
			// round-robin timeout (if it had one) will have been cancelled
			// by the TMK_ResetTimeout() call that was used to establish
			// the API timeout. The value returned by TMK_ResetTimeout()
			// will have been placed in g_remainingTicks. It is safe to
			// use a global for this as it is protected by the master
			// kernel lock.
			//
		case kUnknown:
			// Similarly, the outgoing thread's timeout will have been
			// cancelled in VDK::DestroyThread().
			//
			remainingTicks = g_remainingTicks;
		break;
			
	    default:
			// For all other cases we cancel the timeout
			remainingTicks = TMK_CancelTimeout(pOldVdkThread);
			break;
		}

		// if the timeout had expired on the outgoing thread then start a new timeslice
		// for its priority level.
		//
		if (remainingTicks <= 0)
			remainingTicks = g_RoundRobinInitPeriod[pOldThread->priority];

		g_RoundRobinTimeRemaining[pOldThread->priority] = remainingTicks;
	}

	// Start the timeout for the new thread's timeslice
	if (IsBitSet(g_RoundRobinInitPriority, pNewThread->priority))
		TMK_SetTimeout(pNewVdkThread, g_RoundRobinTimeRemaining[pNewThread->priority]);

	// Chain to the next function in the line
	(*pfRuntimeRRChain)(pOldThread, pNewThread);
}

static void RoundRobinInterruptHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread)
{
#pragma suppress_null_check
	Thread *pOldVdkThread = static_cast<Thread*>(pOldThread);
#pragma suppress_null_check
	Thread *pNewVdkThread = static_cast<Thread*>(pNewThread);

#if VDK_INSTRUMENTATION_LEVEL_ >1
	if (pOldVdkThread->StackGuardWordChanged())
	    KernelPanic(kStackCheckFailure, kThreadStackOverflow, pOldVdkThread->ID());
#endif

	// The outgoing thread has been pre-empted, we don't know whether or not it
	// had a timeout so we cancel anyway
	if (IsBitSet(g_RoundRobinInitPriority, pOldThread->priority))
	{
		int remainingTicks = TMK_CancelTimeout(pOldVdkThread);

		// if the timeout had expired on the outgoing thread then start a new timeslice
		// for its priority level.
		//
		if (remainingTicks <= 0)
			remainingTicks = g_RoundRobinInitPeriod[pOldThread->priority];

		g_RoundRobinTimeRemaining[pOldThread->priority] = remainingTicks;
	}

	// Start the timeout for the new thread's timeslice
	if (IsBitSet(g_RoundRobinInitPriority, pNewThread->priority))
		TMK_SetTimeout(pNewVdkThread, g_RoundRobinTimeRemaining[pNewThread->priority]);

	// Chain to the next function in the line
	(*pfInterruptRRChain)(pOldThread, pNewThread);
}

#pragma param_never_null pNewThread
static void RoundRobinFirstTimeHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread)
{
	// pOldThread is always NULL
#pragma suppress_null_check
	Thread *pNewVdkThread = static_cast<Thread*>(pNewThread);

	// Start the timeout for the thread's timeslice
	if (IsBitSet(g_RoundRobinInitPriority, pNewThread->priority))
		TMK_SetTimeout(pNewVdkThread, g_RoundRobinTimeRemaining[pNewThread->priority]);

	// Chain to the next function in the line
	(*pfFirstTimeRRChain)(pOldThread, pNewThread);
}

#pragma param_never_null pOldThread pNewThread
static void RoundRobinKillThreadHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread)
{
	//
	// We must be careful about what we do in here as we're
	// running on the system stack.
	//
#pragma suppress_null_check
	Thread *pNewVdkThread = static_cast<Thread*>(pNewThread);

	// The old thread's timeout will have been cancelled by VDK::DestroyThread()

	// Start the timeout for the new thread's timeslice
	if (IsBitSet(g_RoundRobinInitPriority, pNewThread->priority))
		TMK_SetTimeout(pNewVdkThread, g_RoundRobinTimeRemaining[pNewThread->priority]);

	// Chain to the next function in the line
	(*pfKillThreadRRChain)(pOldThread, pNewThread);
}


bool InitRoundRobin(void)
{
	// Only add the round-robin hook functions to the chains if there are RR priority levels
	// defined for the application.
	//
	if (0 != g_RoundRobinInitPriority)
	{
		pfFirstTimeRRChain  = TMK_HookThreadSwitch (TMK_FIRSTTIMEHOOK,  RoundRobinFirstTimeHookFn);
#if 0 // BYPASSING NEEDS TO BE TESTED AND ENABLED - BW
		pfRuntimeRRChain    = TMK_HookThreadSwitch (TMK_RUNTIMEHOOK,    RoundRobinRuntimeBypass);
		pfInterruptRRChain  = TMK_HookThreadSwitch (TMK_INTERRUPTHOOK,  RoundRobinInterruptBypass);
#else
		pfRuntimeRRChain    = TMK_HookThreadSwitch (TMK_RUNTIMEHOOK,    RoundRobinRuntimeHookFn);
		pfInterruptRRChain  = TMK_HookThreadSwitch (TMK_INTERRUPTHOOK,  RoundRobinInterruptHookFn);
#endif
		pfKillThreadRRChain = TMK_HookThreadSwitch (TMK_KILLTHREADHOOK, RoundRobinKillThreadHookFn);

	    // Setup the initial values of time remaining.
	    for (int i = 1; i < sizeof(g_RoundRobinTimeRemaining) / sizeof(VDK::Ticks); ++i)
	        g_RoundRobinTimeRemaining[i] = g_RoundRobinInitPeriod[i];
	}

	return true;
}

} // namespace VDK
