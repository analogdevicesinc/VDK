/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file ThreadList.c
 * Functions for managing ThreadLists.
 *
 * ThreadLists are containers for threads, which are implemented as doubly-linked
 * lists. A Thread can be in at most one list at a time, because the 'previous'
 * and 'next' pointers are part of the Thread structure itself, and there is
 * only one of each in a thread.
 *
 * ThreadLists can be used in FIFO order, or they can be maintained in increasing
 * order of thread priority. The choice is made by the selection of "add"
 * function - TMK_AppendThreadToList() or TMK_AddThreadToOrderedList(). One
 * or other should be used for any particular list, they should not be mixed.
 */

#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"

#pragma file_attr("Threads")
#pragma file_attr("OS_Component=Threads")

///////////////////////////////////////////////////////////////////////////////
//   Thread List Functions
///////////////////////////////////////////////////////////////////////////////

/** Inserts a thread into the specified thread list, maintaining
 * priority order. This function is non-deterministic, due to the need
 * to traverse the list to find the insertion point.
 *
 * The insertion is *after* and threads of the same priority, so that
 * FIFO ordering applies amongst thread of the same priority.
 *
 * Because of the need to search for the insertion point this function
 * necessarily contains a loop and hence is not suited for inlining.
 *
 * @param[in,out] pList - pointer to the list to be added to
 * @param[in,out] pThread - pointer to the thread to be added
 *
 * @note Non-deterministic (unbounded)
 */
void
TMK_AddThreadToOrderedList(
    TMK_ThreadList *pList,
    TMK_Thread     *pThread
)
{
    TMK_Thread *pInsPt = NULL;

    // Integrity check
    assert(pList->pHead->pPrev == (TMK_Thread*)pList);
    assert(pList->pTail->pNext == (TMK_Thread*)pList);

    assert(pThread->pNext == pThread);
    assert(pThread->pPrev == pThread);
    
    for (pInsPt = pList->pHead;
         pInsPt != (TMK_Thread*)pList;
         pInsPt = pInsPt->pNext)
    {
        // We can only be in here if pInsPt is *not* pointing
        // at the list itself, so it's safe to examine
        // pInsPt->priority.
#ifdef PRIORITY_ZERO_HIGHEST
        if (pThread->priority < pInsPt->priority)
            break;
#else
        if (pThread->priority > pInsPt->priority)
            break;
#endif
    }
    
    // pInsPt is either pointing at the list itself, or
    // at a thread which is of *lesser* priority than pThread.
    // Either way we insert ahead of pInsPt.
    pThread->pNext       = pInsPt;
    pThread->pPrev       = pInsPt->pPrev;
    pInsPt->pPrev->pNext = pThread;
    pInsPt->pPrev        = pThread;
}


/** Traverses a threadlist and returns the number of threads in the list
 * and (optionally) the pointers to those threads.
 *
 * This function is non-deterministic, due to the need
 * to traverse the list.
 *
 * Because of the need to walk the list this function
 * necessarily contains a loop and hence is not suited for inlining.
 *
 * @param[in]  pList - pointer to the list to be queried
 * @param[out] vpThread - array of returned thread pointers
 * @param[in]  arraySize - number of elements in the vpThread array
 *
 * @return The number of threads in the list
 *
 * @note Non-deterministic (unbounded)
 */
int
TMK_QueryThreadlist(
    TMK_ThreadList *pList,
    TMK_Thread     *vpThreads[],
	int arraySize
)
{
	int totalFound = 0;
	int numWritten = 0;
	TMK_Thread *pThread = NULL;

	for(pThread = pList->pHead;
		pThread != (TMK_Thread*)pList;
		pThread = pThread->pNext)
	{
		++totalFound;

		if (numWritten < arraySize)
			vpThreads[numWritten++] = pThread;
	}

	assert(numWritten <= arraySize);
	assert(numWritten <= totalFound);

	return totalFound;
}

