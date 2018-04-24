/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file TimeQueue.c
 */

#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"
#pragma file_attr("OS_Internals")

///////////////////////////////////////////////////////////////////////////////
//   Macros for comparing tick values
///////////////////////////////////////////////////////////////////////////////

#define TICKS_LT(a,b)     ((int)((a)-(b)) < 0)
#define TICKS_LEQ(a,b)    ((int)((a)-(b)) <= 0)
#define TICKS_GT(a,b)     ((int)((a)-(b)) > 0)
#define TICKS_GEQ(a,b)    ((int)((a)-(b)) >= 0)

///////////////////////////////////////////////////////////////////////////////
//   Static function declarations
///////////////////////////////////////////////////////////////////////////////

static void
timeQueueInvoke(TMK_DpcElement *pDpcElement);

///////////////////////////////////////////////////////////////////////////////
/// @internal
/// @brief  The Global Time Queue
/// @hideinitializer
///////////////////////////////////////////////////////////////////////////////

TimeQueue tmk_timeQueue =
{
	{                                                        // dpc
		NULL,                                                // pNext
		timeQueueInvoke                                      // pfInvoke
	},
	LCK_INIT_GLOBAL_INTERRUPT_LOCK(TMK_MAX_LOCK_PRECEDENCE), // lock
	{                                                        // list
		&tmk_timeQueue.list,                                 // pNext
		&tmk_timeQueue.list,                                 // pPrev
		0,                                                   // tickNumber
		NULL                                                 // pfTimeout
	},
	0                                                        // uptimeTicks
};

///////////////////////////////////////////////////////////////////////////////
//   Time Queue Functions
///////////////////////////////////////////////////////////////////////////////

/*
DPC deferred invocation function for the time queue.

Return type: void
*/
static void
timeQueueInvoke(TMK_DpcElement *pDpcElement)
{
    TimeQueue *pTimeQueue = (TimeQueue*)pDpcElement;

    assert(pTimeQueue == &tmk_timeQueue);

	// We can't assume that there's going to be any work to do.  It is
	// (just) possible for a timed-out element to be removed from the
	// queue by a prior DPC, after it has caused the timer ISR to
	// raise the kernel interrupt but before this DPC gets to time it
	// out. Also, if there are no other timeouts happening then we
	// will see a spurious one every 2^32 ticks when uptimeTicks
	// wraps around to equal the list-stop ticknumber (it's not
	// worth handling this case in the timer ISR as it should never
	// occur in the real world and is harmless even if it does).

    // We're going to pull each timed-out element in turn off the
    // front of the timelist and call its Timeout() method.
	//
    for (;;)    // middle-breaking loop
    {
		// Changes to tmk_timeQueue.list must be atomic wrt. the TimerISR.
    	TMK_LockState state =
    	   TMK_AcquireGlobalInterruptLock(&pTimeQueue->lock);

		// Get the first item in the list.
		TMK_TimeElement *pElement = pTimeQueue->list.pNext;

		// We're looking for elements which have been reached or
		// overtaken by uptimeTicks and which are *ahead* of the
		// list-stop. The *ahead* condition ensures that the list
		// itself won't be timed-out (which would be bad since it
		// doesn't have a timeout function *and* we would detach
		// it from the rest of the list).
		//
		if (TICKS_GT(pElement->tickNumber, pTimeQueue->list.tickNumber) &&
			TICKS_LEQ(pElement->tickNumber, pTimeQueue->uptimeTicks))
		{
            // Remove the element from the list
            pElement->pPrev->pNext = pElement->pNext;
            pElement->pNext->pPrev = pElement->pPrev;
            pElement->pNext = pElement;
            pElement->pPrev = pElement;
		}
		else
		{
			pElement = NULL;  // will cause the loop to exit

			// Advance the timequeue's tick position to the current tick.
			pTimeQueue->list.tickNumber = pTimeQueue->uptimeTicks;
		}

		// As soon as we release this lock we may have a timer ISR occur
		// (if the timer interrupt was latched while the lock was held)
		// so we can't trust the master uptimeTicks not to change.
		//
        TMK_ReleaseGlobalInterruptLock(&pTimeQueue->lock, state);

		if (NULL == pElement) break; // loop breaks here (only)
		
		// Timeout this element by calling its method
		assert(pElement != &tmk_timeQueue.list);
        pElement->pfTimeout(pElement);
    }
}


/** Initialises the fields of a time element structure.
 *
 * @param[out] pElement - the TimeElement to be initialized
 * @param[in] pfTimeout - the pointer to the function to be called if/when the element times out
 */
void
TMK_InitTimeElement(
    TMK_TimeElement *pElement,
    void           (*pfTimeout)(struct TMK_TimeElement *)
)
{
    pElement->pNext = pElement;
    pElement->pPrev = pElement;
    pElement->tickNumber = 0;
    pElement->pfTimeout = pfTimeout;
}


/** Registers a time element with the time queue.
 *
 * This function is non-deterministic, due to the need to traverse the time
 * queue to find the insertion point.
 *
 * @param[in,out] pElement - the TimeElement to be registered
 * @param timeout - the number of ticks until the element times out
 */
void
TMK_SetTimeout(
    TMK_TimeElement *pElement,
    int timeout
)
{
	/** @pre @a timeout must be greater than zero */
    assert(timeout > 0);
	/** @pre The master kernel lock must be held */
    assert(tmk.masterLock.lockHeld);
	/** @pre The element must not currently be in the time queue */
    assert(pElement->pNext == pElement);
    assert(pElement->pPrev == pElement);

    // Turn interrupts off until we know roughly where the insertion
    // point is going to be (i.e. front or not).
    TMK_LockState state = TMK_AcquireGlobalInterruptLock(&tmk_timeQueue.lock);

	// Setup the element we're inserting
	pElement->tickNumber = tmk_timeQueue.uptimeTicks + timeout;
    
    TMK_TimeElement *pCurr = tmk_timeQueue.list.pNext;

    if (pCurr == &tmk_timeQueue.list ||
		TICKS_GT(pCurr->tickNumber, pElement->tickNumber))
    {
		// We are inserting at the front of the list, we have to do
		// it with interrupts off so that the insertion is atomic
		// wrt. the TimerISR.
		pElement->pNext          = pCurr; // pCurr == tmk_timeQueue.list.pNext
		pElement->pPrev          = &tmk_timeQueue.list;
		tmk_timeQueue.list.pNext = pElement;
		pCurr->pPrev             = pElement;
		
		// Now we can re-enable interrupts
        TMK_ReleaseGlobalInterruptLock(&tmk_timeQueue.lock, state);
    }
    else
    {
		// The insertion isn't going to be at the front of the list,
		// so we don't have to protect against the TimerISR.
		//
        TMK_ReleaseGlobalInterruptLock(&tmk_timeQueue.lock, state);

		// Find the place to insert the element. Note that the
		// lazy evaluation of the loop test means that we will never
		// look at tmk_timeQueue.list.tickNumber, so we don't have
		// to worry about locking.
		//
		do
			pCurr = pCurr->pNext;
		while ((pCurr != &tmk_timeQueue.list) &&
			   TICKS_LEQ(pCurr->tickNumber, pElement->tickNumber));

		// Insert the new element ahead of pCurr
		pElement->pNext     = pCurr;
		pElement->pPrev     = pCurr->pPrev;
		pCurr->pPrev->pNext = pElement;
		pCurr->pPrev        = pElement;
    }
}


/** Removes a time element from the time queue.
 *
 * @param[in,out] pElement - the TimeElement to be cancelled
 *
 * @return The number of ticks remaining until expiry
 */
int
TMK_CancelTimeout(
    TMK_TimeElement *pElement
)
{
	/** @pre The master kernel lock must be held */
    assert(tmk.masterLock.lockHeld);
	assert(&tmk_timeQueue.list != pElement);

    // Changes to the front of the time queue must be atomic
    // with respect to the TimerISR. Since the removal is
	// deterministic we can just acquire the global lock
	// on the timequeue without needing to care whether
	// it is the head of the queue that we are removing.
	//
    TMK_LockState state =
        TMK_AcquireGlobalInterruptLock(&tmk_timeQueue.lock);
   
	int timeRemaining = pElement->tickNumber - tmk_timeQueue.uptimeTicks;

    // Remove the element from the list
    pElement->pPrev->pNext = pElement->pNext;
    pElement->pNext->pPrev = pElement->pPrev;
    pElement->pNext = pElement;
    pElement->pPrev = pElement;

    // Updates to the list itself are finished.
    TMK_ReleaseGlobalInterruptLock(&tmk_timeQueue.lock, state);

	return timeRemaining;
}

/** Removes a time element from the time queue, whether it was on the
 * queue or not, then registers the element (again) with a new timeout.
 *
 * Equivalent to doing TMK_CancelTimeout() followed by TMK_SetTimeout(), but faster.
 *
 * @note This function is non-deterministic(unbounded), due to the need to traverse the
 * time queue to find the insertion point.
 *
 * @param[in,out] pElement - the TimeElement to be (re-)registered
 * @param timeout - the number of ticks until the element times out
 *
 * @return The number of ticks remaining of the previous timeout
 */
int
TMK_ResetTimeout(
    TMK_TimeElement *pElement,
    int timeout
)
{
	/**@pre @a timeout must be greater than zero. */
    assert(timeout > 0);
	/**@pre The master kernel lock must be currently held. */
    assert(tmk.masterLock.lockHeld);
    
    // Changes to the front of the time queue must be atomic
    // with respect to the TimerISR.
    // Turn interrupts off until we know roughly where the insertion
    // point is going to be (i.e. front or not).
    TMK_LockState state =
        TMK_AcquireGlobalInterruptLock(&tmk_timeQueue.lock);
   
	int timeRemaining = pElement->tickNumber - tmk_timeQueue.uptimeTicks;
    
    // Remove the element from the list
    pElement->pPrev->pNext = pElement->pNext;
    pElement->pNext->pPrev = pElement->pPrev;

	// At this point the element has been unlinked from the list
	// (i.e. there are no pointers from the list to it) but its
	// own prev and next pointers have not been set to point to
	// itself. This is because those pointers are going to be
	// changed below.
		
	// Setup the element we're inserting
	pElement->tickNumber = tmk_timeQueue.uptimeTicks + timeout;
    
	// Start scanning from the head of the list
    TMK_TimeElement *pCurr = tmk_timeQueue.list.pNext;

    if (pCurr == &tmk_timeQueue.list ||
		TICKS_GT(pCurr->tickNumber, pElement->tickNumber))
    {
		// We are inserting at the front of the list, so we have to do
		// it with interrupts off so that the insertion is atomic
		// wrt. the TimerISR.
		pElement->pNext          = pCurr; // pCurr == tmk_timeQueue.list.pNext
		pElement->pPrev          = &tmk_timeQueue.list;
		tmk_timeQueue.list.pNext = pElement;
		pCurr->pPrev             = pElement;
		
		// Now we can re-enable interrupts
        TMK_ReleaseGlobalInterruptLock(&tmk_timeQueue.lock, state);
    }
    else
    {
		// The insertion isn't going to be at the front of the list,
		// so we don't have to protect against the TimerISR.
        TMK_ReleaseGlobalInterruptLock(&tmk_timeQueue.lock, state);

		// Find the place to insert the element. Note that the
		// lazy evaluation of the loop test means that we will never
		// look at tmk_timeQueue.list.tickNumber, so we don't have
		// to worry about locking.
		//
		do
			pCurr = pCurr->pNext;
		while ((pCurr != &tmk_timeQueue.list) &&
			   TICKS_LEQ(pCurr->tickNumber, pElement->tickNumber));
		
		// Insert the new element ahead of pCurr
		pElement->pNext     = pCurr;
		pElement->pPrev     = pCurr->pPrev;
		pCurr->pPrev->pNext = pElement;
		pCurr->pPrev        = pElement;
    }

	return timeRemaining;
}

/** Returns the time remaining for a time element.
 *
 * @param[in] pElement - the TimeElement to be queried
 *
 * @return The number of ticks remaining until expiry, may be -ve
 *         if element has expired or is overdue
 */
int
TMK_GetTimeRemaining(
    TMK_TimeElement *pElement
)
{
	/**@pre The element must currently be on the time queue. */
	assert(pElement->pNext != pElement);
    assert(pElement->pPrev != pElement);

	return (int) (pElement->tickNumber - tmk_timeQueue.uptimeTicks);
}


/** Returns the uptime ticks from the globals struct.
 *
 * The Blackfin and TigerSHARC versions of this function are implemented in
 * assembly, The SHARC version has to be in C due to differences in
 * calling conventions between C/C++ and assembly.
 *
 * @return The current uptime value
 */
#if defined(__ADSP21000__) || defined(DOXYGEN)
unsigned int
TMK_GetUptime(void)
{
	return tmk_timeQueue.uptimeTicks;
}
#endif

