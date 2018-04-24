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

// Macros used to convert a priority level into a bitmask and an index
// for use with the multi-word levelBits field.
//
#define PRIORITY_MASK(pri) (1U << ((pri) & 0x1f))
#define PRIORITY_INDEX(pri) (pri >> 5)

// Macro used to initialize the levels of the ready queue, below
#define INIT_QUEUE_LEVEL(n) { (TMK_Thread*)&tmk_readyQueue.vLevels[n], (TMK_Thread*)&tmk_readyQueue.vLevels[n] }

///////////////////////////////////////////////////////////////////////////////
/// @internal
/// @brief  The Global Ready Queue
/// @hideinitializer
///////////////////////////////////////////////////////////////////////////////


#if NUM_PRI_LEVELS > 128
#error Priorities > 128 not supported
#endif /* NUM_PRI_LEVELS > 128 */

ReadyQueue tmk_readyQueue =
{
    { 0 }, // levelBits
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
	INIT_QUEUE_LEVEL(31),
	INIT_QUEUE_LEVEL(32),
	INIT_QUEUE_LEVEL(33),
	INIT_QUEUE_LEVEL(34),
	INIT_QUEUE_LEVEL(35),
	INIT_QUEUE_LEVEL(36),
	INIT_QUEUE_LEVEL(37),
	INIT_QUEUE_LEVEL(38),
	INIT_QUEUE_LEVEL(39),
	INIT_QUEUE_LEVEL(40),
	INIT_QUEUE_LEVEL(41),
	INIT_QUEUE_LEVEL(42),
	INIT_QUEUE_LEVEL(43),
	INIT_QUEUE_LEVEL(44),
	INIT_QUEUE_LEVEL(45),
	INIT_QUEUE_LEVEL(46),
	INIT_QUEUE_LEVEL(47),
	INIT_QUEUE_LEVEL(48),
	INIT_QUEUE_LEVEL(49),
	INIT_QUEUE_LEVEL(50),
	INIT_QUEUE_LEVEL(51),
	INIT_QUEUE_LEVEL(52),
	INIT_QUEUE_LEVEL(53),
	INIT_QUEUE_LEVEL(54),
	INIT_QUEUE_LEVEL(55),
	INIT_QUEUE_LEVEL(56),
	INIT_QUEUE_LEVEL(57),
	INIT_QUEUE_LEVEL(58),
	INIT_QUEUE_LEVEL(59),
	INIT_QUEUE_LEVEL(60),
	INIT_QUEUE_LEVEL(61),
	INIT_QUEUE_LEVEL(62),
	INIT_QUEUE_LEVEL(63),
	INIT_QUEUE_LEVEL(64),
	INIT_QUEUE_LEVEL(65),
	INIT_QUEUE_LEVEL(66),
	INIT_QUEUE_LEVEL(67),
	INIT_QUEUE_LEVEL(68),
	INIT_QUEUE_LEVEL(69),
	INIT_QUEUE_LEVEL(70),
	INIT_QUEUE_LEVEL(71),
	INIT_QUEUE_LEVEL(72),
	INIT_QUEUE_LEVEL(73),
	INIT_QUEUE_LEVEL(74),
	INIT_QUEUE_LEVEL(75),
	INIT_QUEUE_LEVEL(76),
	INIT_QUEUE_LEVEL(77),
	INIT_QUEUE_LEVEL(78),
	INIT_QUEUE_LEVEL(79),
	INIT_QUEUE_LEVEL(80),
	INIT_QUEUE_LEVEL(81),
	INIT_QUEUE_LEVEL(82),
	INIT_QUEUE_LEVEL(83),
	INIT_QUEUE_LEVEL(84),
	INIT_QUEUE_LEVEL(85),
	INIT_QUEUE_LEVEL(86),
	INIT_QUEUE_LEVEL(87),
	INIT_QUEUE_LEVEL(88),
	INIT_QUEUE_LEVEL(89),
	INIT_QUEUE_LEVEL(90),
	INIT_QUEUE_LEVEL(91),
	INIT_QUEUE_LEVEL(92),
	INIT_QUEUE_LEVEL(93),
	INIT_QUEUE_LEVEL(94),
	INIT_QUEUE_LEVEL(95),
	INIT_QUEUE_LEVEL(96),
	INIT_QUEUE_LEVEL(97),
	INIT_QUEUE_LEVEL(98),
	INIT_QUEUE_LEVEL(99),
	INIT_QUEUE_LEVEL(100),
	INIT_QUEUE_LEVEL(101),
	INIT_QUEUE_LEVEL(102),
	INIT_QUEUE_LEVEL(103),
	INIT_QUEUE_LEVEL(104),
	INIT_QUEUE_LEVEL(105),
	INIT_QUEUE_LEVEL(106),
	INIT_QUEUE_LEVEL(107),
	INIT_QUEUE_LEVEL(108),
	INIT_QUEUE_LEVEL(109),
	INIT_QUEUE_LEVEL(110),
	INIT_QUEUE_LEVEL(111),
	INIT_QUEUE_LEVEL(112),
	INIT_QUEUE_LEVEL(113),
	INIT_QUEUE_LEVEL(114),
	INIT_QUEUE_LEVEL(115),
	INIT_QUEUE_LEVEL(116),
	INIT_QUEUE_LEVEL(117),
	INIT_QUEUE_LEVEL(118),
	INIT_QUEUE_LEVEL(119),
	INIT_QUEUE_LEVEL(120),
	INIT_QUEUE_LEVEL(121),
	INIT_QUEUE_LEVEL(122),
	INIT_QUEUE_LEVEL(123),
	INIT_QUEUE_LEVEL(124),
	INIT_QUEUE_LEVEL(125),
	INIT_QUEUE_LEVEL(126),
	INIT_QUEUE_LEVEL(127)
    }
};

#undef INIT_QUEUE_LEVEL

///////////////////////////////////////////////////////////////////////////////
//   Ready Queue Functions
///////////////////////////////////////////////////////////////////////////////

/** @internal
 * @brief Get the thread which has highest run priority.
 *
 * @return Pointer to the struct for highest-priority thread in the ready queue
 */
TMK_Thread *
tmk_GetHighestPriorityThread(void)
{
    extern ReadyQueue tmk_readyQueue;

    /**@internal
     * @pre The master kernel lock must be held.
     */
    assert(tmk.masterLock.lockHeld);

    unsigned int highestPriority;

    if (__builtin_expected_true(0 != tmk_readyQueue.levelBits[3]))
	highestPriority = 127 - __builtin_norm_fr1x32(tmk_readyQueue.levelBits[3] >> 1);
    else if (__builtin_expected_true(0 != tmk_readyQueue.levelBits[2]))
	highestPriority = 95 - __builtin_norm_fr1x32(tmk_readyQueue.levelBits[2] >> 1);
    else if (__builtin_expected_true(0 != tmk_readyQueue.levelBits[1]))
	highestPriority = 63 - __builtin_norm_fr1x32(tmk_readyQueue.levelBits[1] >> 1);
    else
	highestPriority = 31 - __builtin_norm_fr1x32(tmk_readyQueue.levelBits[0] >> 1);

    TMK_ThreadList *pLevel = &tmk_readyQueue.vLevels[highestPriority];
    assert(pLevel->pHead != (TMK_Thread*)pLevel); // level should not be empty,
    assert(pLevel->pTail != (TMK_Thread*)pLevel); // this will be a kernel panic
    assert(READY == pLevel->pHead->runStatus);

    return pLevel->pHead; // get first thread at this level;
}


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
    //    assert((pThread->priority & 0x1F) != 0x1F);
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
    tmk_readyQueue.levelBits[PRIORITY_INDEX(pThread->priority)] |= PRIORITY_MASK(pThread->priority);
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

    // Remove from ready queue
    // Equivalent to TMK_RemoveThreadFromList(pLevel, pThread),
    // although the first argument is not used
    pThread->pPrev->pNext = pThread->pNext;
    pThread->pNext->pPrev = pThread->pPrev;
    pThread->pNext = pThread;
    pThread->pPrev = pThread;

    // Set new status
    /// This is the only way for a thread to leave the ready state
    pThread->runStatus = newStatus;
    
    // If there are no more ready threads at this priority level then
    // clear the level's bit.
    //
    if (pLevel->pHead == (TMK_Thread*)pLevel)
    {
        // Threadlist is empty - no threads at this priority
        assert(pLevel->pTail == (TMK_Thread*)pLevel);

        tmk_readyQueue.levelBits[PRIORITY_INDEX(pThread->priority)] &= ~PRIORITY_MASK(pThread->priority);
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

    // The thread must be at the head of its level.
    assert(pLevel->pHead == pThread);
    assert(pThread->pPrev = (TMK_Thread*)pLevel);

    // If there is only one thread in the list then do nothing
    // and return false to indicate that the list wasn't changed.
    if (pLevel->pTail == pThread)
    {
        assert(pThread->pNext == (TMK_Thread*)pLevel);
        return false;
    }
        
    assert(pLevel->pTail != (TMK_Thread*)pLevel);
    assert(pThread->pNext != (TMK_Thread*)pLevel);
    
    pLevel->pHead         = pThread->pNext;
    pThread->pNext->pPrev = (TMK_Thread*)pLevel;
    pThread->pNext        = (TMK_Thread*)pLevel;
    pThread->pPrev        = pLevel->pTail;
    pLevel->pTail->pNext  = pThread;
    pLevel->pTail         = pThread;
    
    return true;
}

