/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file Thread.c
 */
#include <stdlib.h>
#include <assert.h>
#include "Trinity.h"
#pragma file_attr("Threads")
#pragma file_attr("OS_Component=Threads")

#ifdef __ADSPTS__
#include "TSCtxtRec.h"
const unsigned int tmk_ContextRecordSize = sizeof(TSContext);
#elif defined(__ADSP21000__)
#include "SHCtxtRec.h"
const unsigned int tmk_ContextRecordSize = sizeof(SHContext);
#elif defined(__ADSPBLACKFIN__)
const unsigned int tmk_ContextRecordSize = 45*4; // counted by hand
#endif

///////////////////////////////////////////////////////////////////////////////
//   Thread Functions
///////////////////////////////////////////////////////////////////////////////

/** Initialises the fields of a thread structure.
 * Does *not* put the thread into the ready state.
 *
 * @param[out] pThread - the thread to be initialised
 * @param priority - initial priority
 * @param[in] pStack - the start of the thread's stack memory
 * @param[in] pfRunFunc - thread run function
 */
void
TMK_InitThread
(
    TMK_Thread     *pThread,    // the thread to be initialised
    unsigned int    priority,   // initial priority
    void          (*pfRunFunc)(TMK_Thread*),// thread run function
#if defined(__ADSPTS__)
    void           *pStackJ,
    void           *pStackK,
    void           *pCtxtJ,
    void           *pCtxtK
#elif defined(__ADSPBLACKFIN__)
    void           *pStack,
    void           *pStackBase
#elif defined(__ADSP21000__)
    void           *pStack,
    unsigned int    stackSize,
    void           *pCtxt
#else
#error Unknown processor family
#endif
)
{
    assert(priority < NUM_PRI_LEVELS);

    // Initialize every field in the TMK_Thread structure
    //
    pThread->pNext        = pThread;    // priority list entries
    pThread->pPrev        = pThread;    // priority list entries
    pThread->priority     = priority;   // priority list entries
    pThread->runStatus    = LIMBO;      // not run yet (limbo state)
    pThread->contextLevel = NOTRUN;     // no context
    pThread->pStuff       = pfRunFunc;  // run function passed in thread-specific field

    // Processor-specific initializations
    //
#if defined(__ADSPTS__)
    pThread->pContextSaveJ = pCtxtJ;     // only for TS
    pThread->pContextSaveK = pCtxtK;     // only for TS

    // Store the stack pointers in the context records. This is where they
    // will be picked up from by the first-time context switch. Note that -
    // - confusingly - the *J* regs are stored in *K* save region, and vice-
    // -versa. This is because the csave records are named for the registers
    // which will be used to *access* them, not the regs they will contain.
    //
    TSContext *pTSCtxtJ = pCtxtJ;
    TSContext *pTSCtxtK = pCtxtK;
    pTSCtxtK->JK27_24 =__builtin_compose_128(0LL, __builtin_compose_64(0,(int)pStackJ));
    pTSCtxtJ->JK27_24 =__builtin_compose_128(0LL, __builtin_compose_64(0,(int)pStackK));
#elif defined(__ADSPBLACKFIN__)
    pThread->pContextSave = pStack;     // only for blackfin
    pThread->pStackBase = pStackBase;
#elif defined(__ADSP21000__)
    pThread->pContextSave = pCtxt;     // only for SHARC

    unsigned int stackBase = (unsigned int)pStack - stackSize + 1;

    SHContext *pSHCtxt = pCtxt;
    pSHCtxt->L6 = stackSize;
    pSHCtxt->L7 = stackSize;
    pSHCtxt->B6 = stackBase;
    pSHCtxt->B7 = stackBase;
    pSHCtxt->I6 = (unsigned int)pStack;
    pSHCtxt->I7 = (unsigned int)pStack;
#else
#error Unknown processor family
#endif
}


/** De-initialises a thread structure. Frees stack space, etc.
 *
 * @param[in,out] pThread - the thread to be de-initialised
 */
void
TMK_DeInitThread
(
    TMK_Thread *pThread         // the thread to be de-initialised
#if defined(__ADSPTS__)
    ,
    void           **ppCtxtJ,
    void           **ppCtxtK
#elif defined(__ADSPBLACKFIN__)
#elif defined(__ADSP21000__)
    ,
    void           **ppCtxt
#else
#error Unknown processor family
#endif
)
{
    // We expect the thread to be in the "limbo" state
    assert(LIMBO == pThread->runStatus);
    assert(pThread->pNext == pThread);
    assert(pThread->pPrev == pThread);
    
    // Other than that we don't have to do anything here
#if defined(__ADSPTS__)
    *ppCtxtJ = pThread->pContextSaveJ;
    *ppCtxtK = pThread->pContextSaveK;
#elif defined(__ADSPBLACKFIN__)
#elif defined(__ADSP21000__)
    *ppCtxt = pThread->pContextSave;
#else
#error Unknown processor family
#endif
    
}

/** Alters the priority of a running thread.
 * A true result indicates that rescheduling is required.
 *
 * @param[in,out] pThread - thread to be changed
 * @param priority - new priority
 *
 * @return True if successful, false if thread is not in ready state.
 */
bool
TMK_ChangeThreadPriority
(
    TMK_Thread  *pThread,
    unsigned int priority
)
{
    if (READY != pThread->runStatus)
    {
	/// If the thread is not READY then the priority change won't take
	/// effect until the thread is put back into the ready state.
	///
        pThread->priority = priority;       // set new priority
        return false;
    }

    // The thread *is* in the ready state
    //
    TMK_MakeThreadNotReady(pThread, LIMBO); // put it into limbo
    pThread->priority = priority;           // set new priority
    TMK_MakeThreadReady(pThread);           // put back into readyqueue

    return true;
}

