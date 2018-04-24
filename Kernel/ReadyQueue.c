/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file ReadyQueue.c
    Contains the ready queue and the
    functions which operate on it.

    The ready queue is a data structure which tracks the set
    of runnable threads, by priority.
*/

#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"
#pragma file_attr("Threads")
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("OS_Internals")

#ifndef PRIORITY_ZERO_HIGHEST
#define PRIORITY_MASK(pri) ((1U << (pri)) >> 1)
#else
#define PRIORITY_MASK(pri) (0x40000000U >> (pri))
#endif

// Macro used to initialize the levels of the ready queue, below
#define INIT_QUEUE_LEVEL(n) { (TMK_Thread*)&tmk_readyQueue.vLevels[n], (TMK_Thread*)&tmk_readyQueue.vLevels[n] }

///////////////////////////////////////////////////////////////////////////////
/// @internal
/// @brief  The Global Ready Queue
/// @hideinitializer
///////////////////////////////////////////////////////////////////////////////

ReadyQueue tmk_readyQueue =
{
    0, // levelBits
    {  // vLevels
	INIT_QUEUE_LEVEL(0),
	INIT_QUEUE_LEVEL(1),
	INIT_QUEUE_LEVEL(2),
	INIT_QUEUE_LEVEL(3),
	INIT_QUEUE_LEVEL(4),
	INIT_QUEUE_LEVEL(5),
	INIT_QUEUE_LEVEL(6),
	INIT_QUEUE_LEVEL(7),
	INIT_QUEUE_LEVEL(8),
	INIT_QUEUE_LEVEL(9),
	INIT_QUEUE_LEVEL(10),
	INIT_QUEUE_LEVEL(11),
	INIT_QUEUE_LEVEL(12),
	INIT_QUEUE_LEVEL(13),
	INIT_QUEUE_LEVEL(14),
	INIT_QUEUE_LEVEL(15),
	INIT_QUEUE_LEVEL(16),
	INIT_QUEUE_LEVEL(17),
	INIT_QUEUE_LEVEL(18),
	INIT_QUEUE_LEVEL(19),
	INIT_QUEUE_LEVEL(20),
	INIT_QUEUE_LEVEL(21),
	INIT_QUEUE_LEVEL(22),
	INIT_QUEUE_LEVEL(23),
	INIT_QUEUE_LEVEL(24),
	INIT_QUEUE_LEVEL(25),
	INIT_QUEUE_LEVEL(26),
	INIT_QUEUE_LEVEL(27),
	INIT_QUEUE_LEVEL(28),
	INIT_QUEUE_LEVEL(29),
	INIT_QUEUE_LEVEL(30),
	INIT_QUEUE_LEVEL(31)
    }
};

#undef INIT_QUEUE_LEVEL

///////////////////////////////////////////////////////////////////////////////
//   Ready Queue Functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Make a thread ready to run.
 *
 * Puts the specified thread into the ready-to-run state
 *
 * @param[in,out] pThread - the thread to be made runnable
 */
void
TMK_MakeThreadReady
(
    TMK_Thread *pThread     
)
{
    /**@pre The thread must not be in the ready state. */
    assert(READY != pThread->runStatus);
    /**@pre The thread must not be on a ThreadList. */
    assert(pThread == pThread->pNext && pThread == pThread->pPrev);
    /**@pre The thread must have a legal priority level. */
    assert(pThread->priority < NUM_PRI_LEVELS);
    /**@pre The master kernel lock must be held. */
    assert(tmk.masterLock.lockHeld);

    // This is the threadlist we're inserting into
    TMK_ThreadList *pLevel = &tmk_readyQueue.vLevels[pThread->priority];
    
    /// This is the only way for a thread to become ready.
    pThread->runStatus = READY;

    // Insert at tail of list
    // The cast might look like a hack, but in practice it's much
    // cleaner than the syntactic hoops we'd have to jump through to
    // make this work without it.
    // Equivalent to TMK_AppendThreadToList(pLevel, pThread);
    pThread->pNext       = (TMK_Thread*)pLevel;
    pThread->pPrev       = pLevel->pTail;
    pLevel->pTail->pNext = pThread;
    pLevel->pTail        = pThread;

    // Set the level's bit to indicate that there is at least one
    // ready thread at this priority level.
    //
    tmk_readyQueue.levelBits |= PRIORITY_MASK(pThread->priority);
}


/**
 * Make a thread not ready to run
 *
 * Takes the specified thread out of the ready-to-run state by
 * removing it from the ready queue.
 *
 * @param[in,out] pThread - the thread to be made not runnable
 * @param newStatus - the thread's new (non-ready) status
 */
void
TMK_MakeThreadNotReady
(
    TMK_Thread *pThread,
    int newStatus  // the new (non-ready) state of the thread
)
{
    /**@pre The thread must be changing to a non-ready state. */
    assert(READY != newStatus);
    /**@pre The thread must currently be in the ready state. */
    assert(READY == pThread->runStatus);
    /**@pre The master kernel lock must be held. */
    assert(tmk.masterLock.lockHeld);

    // Integrity check
    assert(pThread->pPrev->pNext == pThread);
    assert(pThread->pNext->pPrev == pThread);

    assert(pThread->pPrev != pThread);
    assert(pThread->pNext != pThread);

    // Shouldn't be removing the current thread from the
    // ready queue if scheduling is suspended. The exception is
    // if the new status is zero (LIMBO) which we use for temporarily
    // removing from the ready queue for priority changes (and also
    // for thread destruction).
    assert(0 == tmk.schedulerRegionCount || pThread != tmk.pCurrentThread || LIMBO == newStatus);

    // This is the threadlist we're removing from (presumably)
    TMK_ThreadList *pLevel = &tmk_readyQueue.vLevels[pThread->priority];

    // Remove from ready queue, equivalent to TMK_RemoveThreadFromList(pThread)
    pThread->pPrev->pNext = pThread->pNext;
    pThread->pNext->pPrev = pThread->pPrev;
    pThread->pNext = pThread;
    pThread->pPrev = pThread;

    // Set new status
    /// This is the only route by which a thread can leave the ready state
    pThread->runStatus = newStatus;
    
    // If there are no more ready threads at this priority level then
    // clear the level's bit.
    //
    if (pLevel->pHead == (TMK_Thread*)pLevel)
    {
        // Threadlist is empty - no threads at this priority
        assert(pLevel->pTail == (TMK_Thread*)pLevel);

        tmk_readyQueue.levelBits &= ~PRIORITY_MASK(pThread->priority);
    }   
}


/**
 * Moves the specified thread to the tail of its priority level.
 *
 * The thread must already be in the ready state. It remains in this
 * state, and at the same priority, but is simply moved to the end of the
 * list of runnable threads at that priority.
 *
 * If the thread is the only one of its priority in the ready queue
 * then nothing needs to happen.
 *
 * @param[in,out] pThread - the yielding thread
 *
 * @return true if the order of threads in the ready queue was
 * changed (indicating that rescheduling is required), otherwise
 * returns false.
 */
bool
TMK_YieldThread(
    TMK_Thread *pThread
)
{
    /**@pre The thread must be in the ready state. */
    assert(READY == pThread->runStatus);
    /**@pre The master kernel lock must be held. */
    assert(tmk.masterLock.lockHeld);

    // This is the threadlist we're removing from (presumably)
    TMK_ThreadList *pLevel = &tmk_readyQueue.vLevels[pThread->priority];
        
    // The level shouldn't be empty
    assert(pLevel->pHead != (TMK_Thread*)pLevel);
    assert(pLevel->pTail != (TMK_Thread*)pLevel);

    // If the thread is already at the end of the list then do nothing
    // and return false to indicate that the list wasn't changed. This also
    // covers the case where there is only one thread in the list.
    //
    if (pLevel->pTail == pThread)
    {
        assert(pThread->pNext == (TMK_Thread*)pLevel);
        return false;
    }

    // Remove from ready queue
    pThread->pPrev->pNext = pThread->pNext;
    pThread->pNext->pPrev = pThread->pPrev;

    // Re-insert at tail of list
    pThread->pNext       = (TMK_Thread*)pLevel;
    pThread->pPrev       = pLevel->pTail;
    pLevel->pTail->pNext = pThread;
    pLevel->pTail        = pThread;

    return true;
}

