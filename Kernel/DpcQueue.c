/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file DpcQueue.c
 * Contains the deferred procedure call queue and functions.
 *
 * The DPC queue is the sole mechanism of pre-emptive signalling
 * from interrupt level to kernel level (and hence to thread level).
 */
#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"
#pragma file_attr("ISR")
#pragma file_attr("OS_Internals")

///////////////////////////////////////////////////////////////////////////////
/// @internal
/// @brief The global Deferred Procedure Call queue
/// @hideinitializer
///////////////////////////////////////////////////////////////////////////////

DPCQueue tmk_dpcQueue =
{
    LCK_INIT_GLOBAL_INTERRUPT_LOCK(TMK_MAX_LOCK_PRECEDENCE), // lock
    &tmk_dpcQueue.pHead,                                     // ppTail
    NULL                                                     // head
};

///////////////////////////////////////////////////////////////////////////////
//   DPC Queue Functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the fields of a DPC struct
 *
 * @param[out] pDpc - the DpcElement to initialize
 * @param[in] pfInvoke - the DPC function pointer
 */
void
TMK_InitDpcElement(
    TMK_DpcElement *pDpc,
    void          (*pfInvoke)(struct TMK_DpcElement *)
)
{
    pDpc->pNext    = NULL;
    pDpc->pfInvoke = pfInvoke;
}


/** @internal
 * @brief Services the DPCs from kernel level.
 *
 * Removes the each DPC object from the head of the Dpc queue (atomically)
 * and calls its invoke function.
 */
void
tmk_DispatchDPCs(void)
{
	// It is one of the DPC Queue's axioms that *if* there is a dpc on
	// the queue, *then either* the reschedule interrupt should be
	// latched *or* we should be at kernel level. The reverse,
	// however, does not necessarily apply, hence it is valid - and
	// possible - for the reschedule interrupt to be latched (or for us
	// to be at kernel level) yet for the dpc queue to be empty.  This
	// can happen, for example, if an ISR occurs and queues a DPC
	// while we are in the loop below. The DPC will be processed
	// by the loop but the reschedule interrupt will remain latched
	// (unless we are able to clear it as the note below suggests).
	//
    for(;;) // middle-breaking loop
    {
    	LCK_LockState state =
			LCK_AcquireGlobalInterruptLock(&tmk_dpcQueue.lock);
    
    	TMK_DpcElement *pDpc = tmk_dpcQueue.pHead;
    
    	if (NULL == pDpc) // if the queue is empty
    	{
    	    // Then turn interrupts back on and leave the loop
        	TMK_ReleaseGlobalInterruptLock(&tmk_dpcQueue.lock, state);
        	break;
    	}

		// The queue isn't empty, so pull the next item off the list
		tmk_dpcQueue.pHead = pDpc->pNext;
		pDpc->pNext = NULL;

		// Check if it's the last in the list
		if (pDpc == tmk_dpcQueue.pHead) // end-stop detected
		{
			// list is now empty
			tmk_dpcQueue.pHead  = NULL;
			tmk_dpcQueue.ppTail = &tmk_dpcQueue.pHead;

			// NOTE: we could maybe clear the reschedule interrupt here,
			// in case another DPC queue insertion has happened while
			// we were in the reschedule ISR. If the DPC queue is
			// empty then there is no reason for the interrupt to be raised.
			// Clearing latched interrupts will only be practical on
			// some platforms.
		}
		
		// Turn interrupts back on
    	LCK_ReleaseGlobalInterruptLock(&tmk_dpcQueue.lock, state);
    	
    	// And call the DPC's invoke function
	    (*pDpc->pfInvoke)(pDpc);
    }
}
