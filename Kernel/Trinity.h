/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @internal
    @file Trinity.h
    The internal declarations for the Trinity microkernel
    
    Anything declared in here is not meant to be accessible
    to anything other than the Trinity source modules. I.e.
    these declarations are hidden from the macrokernel(s).
*/

#ifndef _TRINITY_H_
#define _TRINITY_H_


#include "ADI_attributes.h"

//
// This part is for both assembly and C/C++
//
#ifdef TMK_FOR_AOS
#define NUM_PRI_LEVELS 128U
#else
#define NUM_PRI_LEVELS 32U
#endif

#define NOCONTEXT    0
#define NOTRUN       1
#define RUNTIME      2
#define INTERRUPTED  3

#define _tmk_pCurrentThread _tmk->pCurrentThread

#ifdef __ECC__

#define tmk_pCurrentThread _tmk.pCurrentThread

//
// This part is only for C/C++ compilations, not assembly
//
#include "TMK.h"

/// The run status of a thread
///
typedef enum RunStatus
{
    LIMBO = -1,
    READY = 0
} RunStatus;

///////////////////////////////////////////////////////////////////////////////
//   The Trinity of Queues
///////////////////////////////////////////////////////////////////////////////

/// @internal
/// @brief The ready queue structure
///
typedef struct ReadyQueue {
#ifdef TMK_FOR_AOS
    unsigned int levelBits[4];
#else
    unsigned int levelBits;
#endif
    TMK_ThreadList vLevels[NUM_PRI_LEVELS];
} ReadyQueue;


/// @internal
/// @brief The time queue structure
///
typedef struct TimeQueue {
    TMK_DpcElement  dpc;     // allows time queue to be queued for DPC
    TMK_Lock        lock;
    TMK_TimeElement list;
    unsigned	    uptimeTicks;
} TimeQueue;


/// @internal
/// @brief The deferred procedure call queue structure
///
typedef struct DPCQueue {
    TMK_Lock         lock;
	TMK_DpcElement **ppTail;
	TMK_DpcElement  *pHead;
} DPCQueue;



///////////////////////////////////////////////////////////////////////////////
//   Trinity private macros
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//   Trinity private functions - should all begin with tmk_
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//   Locking Functions
///////////////////////////////////////////////////////////////////////////////

void
tmk_MaskMasterKernelLockForThreadLevel(void);


///////////////////////////////////////////////////////////////////////////////
//   Ready Queue Functions
///////////////////////////////////////////////////////////////////////////////

TMK_Thread *
tmk_GetHighestPriorityThread(void);


///////////////////////////////////////////////////////////////////////////////
//   Time Queue Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//   DPC Queue Functions
///////////////////////////////////////////////////////////////////////////////

void
tmk_DispatchDPCs(void);


///////////////////////////////////////////////////////////////////////////////
//   Scheduler Functions
///////////////////////////////////////////////////////////////////////////////

void
tmk_InitScheduler(void);

void
tmk_Dispatcher(void);


///////////////////////////////////////////////////////////////////////////////
//   DPC Queue Functions
///////////////////////////////////////////////////////////////////////////////

void
tmk_DispatchDPCs(void);


///////////////////////////////////////////////////////////////////////////////
//   Context Switch Functions
///////////////////////////////////////////////////////////////////////////////

void
tmk_SwitchContext_FirstThread(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

void
tmk_SwitchContext_Runtime(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

void
tmk_SwitchContext_ISR(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

void
tmk_SwitchContext_KillThread(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

void
tmk_RunOnSystemStack(TMK_Thread *pOldThread, TMK_Thread *pNewThread, TMK_PFCSwitch pfFunc);


///////////////////////////////////////////////////////////////////////////////
//   Function Inlining Support
///////////////////////////////////////////////////////////////////////////////

#ifdef TMK_INLINE

#include <assert.h>

#ifndef TMK_FOR_AOS

///////////////////////////////////////////////////////////////////////////////
//   Inline ReadyQueue Functions
///////////////////////////////////////////////////////////////////////////////

#if defined(__ADSPBLACKFIN__)
#elif defined(__ADSPTS__)
#include <builtins.h>
#elif defined(__ADSP21000__)

static inline int tmk_leftz(int in_val)
{
	int ret_val;
	asm("%0 = LEFTZ %1;"	// ret_val = # of leading sign bits
		: "=d" (ret_val) : "d" (in_val));

	return ret_val;
}

#else
#error Unknown processor family
#endif


/** @internal
 * @brief Get the thread which has highest run priority.
 *
 * @return Pointer to the struct for highest-priority thread in the ready queue
 */
TMK_INLINE
TMK_Thread *
tmk_GetHighestPriorityThread(void)
{
    extern ReadyQueue tmk_readyQueue;

    /**@
     * @pre The master kernel lock must be held.
     */
    assert(tmk.masterLock.lockHeld);
    /**@internal The sign bit of levelBits must be clear. */
    assert((int)tmk_readyQueue.levelBits >= 0);

    // range is from 31 (bit 30 set) to 0 (no bits set)
#if defined(__ADSPBLACKFIN__)
	unsigned int highestPriority = 31 - __builtin_norm_fr1x32(tmk_readyQueue.levelBits);
#elif defined(__ADSPTS__)
	unsigned int highestPriority = 31 + __builtin_exp(tmk_readyQueue.levelBits);
#elif defined(__ADSP21000__)
	unsigned int highestPriority = 32 - tmk_leftz(tmk_readyQueue.levelBits);
#endif

    TMK_ThreadList *pLevel = &tmk_readyQueue.vLevels[highestPriority];
    assert(pLevel->pHead != (TMK_Thread*)pLevel); // level should not be empty,
    assert(pLevel->pTail != (TMK_Thread*)pLevel); // this will be a kernel panic
    assert(READY == pLevel->pHead->runStatus);

    return pLevel->pHead; // get first thread at this level;
}

#endif /* TMK_FOR_AOS */

///////////////////////////////////////////////////////////////////////////////
//   End Of Function Inlining Support
///////////////////////////////////////////////////////////////////////////////

#endif /* TMK_INLINE */


#endif /* __ECC__ */

// Used for BF561 r0.4/0.5 branch anomaly 05-00-0428
// "Lost/Corrupted Write to L2 Memory Following Speculative 
// Read by Core B from L2 Memory."
//
// Message suppress is for cases where the third instruction
// has a load from an MMR register
#if (defined(__SILICON_REVISION__) && \
         defined(__ADSPBF561__)        && \
         ((__SILICON_REVISION__ == 0xffff) || \
          (__SILICON_REVISION__ > 0x3) ))
#define ANOM05000428_3NOPS NOP;NOP;NOP;
#define ANOM05000428_2NOPS NOP;NOP;
#define ANOM05000428_SUPPRESS_MMR_READ  .MESSAGE/SUPPRESS 5517 FOR 1 LINES;
#else
#define ANOM05000428_3NOPS 
#define ANOM05000428_2NOPS 
#define ANOM05000428_SUPPRESS_MMR_READ
#endif

#endif /* _TRINITY_H_ */

