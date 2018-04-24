/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file TMK.h
 * This header file defines the Kernel Programming Interface (KPI) for
 * the Trinity microkernel.
 *
 * The KPI functions fall into a number of groups:
 *
 * Thread functions
 * Ready queue functions
 * Time queue functions
 * DPC queue functions
 * Thread lists
 * Locking
 * History logging
 */
#ifndef _TMK_H_
#define _TMK_H_

/* for delta we don't want to inline even in release mode but the macros are
 * defined in the common makefile so we need to undef them
 */

#if defined (DO_NOT_INLINE)
#undef TMK_INLINE
#undef LCK_INLINE
#endif

#include <stddef.h> // for offsetof()
#include "LCK.h"

#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
//   Manifest Constants
///////////////////////////////////////////////////////////////////////////////

#define TMK_MIN_LOCK_PRECEDENCE    LCK_MIN_PRECEDENCE
#define TMK_KERNEL_LOCK_PRECEDENCE 128
#define TMK_MAX_LOCK_PRECEDENCE    LCK_MAX_PRECEDENCE

///////////////////////////////////////////////////////////////////////////////
//   Simple type definitions
///////////////////////////////////////////////////////////////////////////////

#if defined(__ADSPBLACKFIN__)
typedef LCK_IMaskUInt TMK_IMaskUInt;
typedef LCK_LockState   TMK_LockState;
#elif defined(__ADSPTS__)
typedef LCK_IMaskUInt TMK_IMaskUInt;
typedef LCK_LockState TMK_LockState;
#elif defined(__ADSP21000__)
typedef LCK_IMaskUInt TMK_IMaskUInt;
typedef LCK_LockState TMK_LockState;
#else
#error Unknown processor family
#endif

typedef enum TMK_HookSelector
{
    TMK_RUNTIMEHOOK,
    TMK_INTERRUPTHOOK,
    TMK_FIRSTTIMEHOOK,
    TMK_KILLTHREADHOOK,
	TMK_NUM_HOOK_FNS
} TMK_HookSelector;

///////////////////////////////////////////////////////////////////////////////
//   Structures
///////////////////////////////////////////////////////////////////////////////

/** The TimeElement structure.  The pointed-to function is called from
 * kernel level after the timeout expires.
 */
typedef struct TMK_TimeElement {
    struct TMK_TimeElement *pNext;      /**< Pointer to the next TimeElement */
    struct TMK_TimeElement *pPrev;      /**< Pointer to the previous TimeElement */
    unsigned int            tickNumber; /**< Ticknumber at which element will expire */
	/** The timeout function pointer */
    void                  (*pfTimeout)(struct TMK_TimeElement *);
} TMK_TimeElement;


/** The DpcElement structure.  The pointed-to function is called from
 * kernel level at the earliest convenient time. DPCs are the only
 * means of signalling to the microkernel from interrupt level.
 */
typedef struct TMK_DpcElement {
    struct TMK_DpcElement *pNext; /**< Pointer to the next DpcElement */
	/** The DPC function pointer */
    void                 (*pfInvoke)(struct TMK_DpcElement *);
} TMK_DpcElement;


/** The Thread structure.
 */
typedef struct TMK_Thread {
	// These two fields *must* come at the very start of the struct
    struct TMK_Thread *pNext;           ///< pointer to next Thread
    struct TMK_Thread *pPrev;           ///< pointer to previous Thread
    void              *pStuff;          ///< thread-specific data

#if defined(__ADSPTS__)
    unsigned int       priority;        ///< 0 - 31 (at present)
    int                runStatus;       ///< ready, sleeping, etc.
    unsigned int       contextLevel;    ///< private to scheduler

    void              *pContextSaveJ;
    void              *pContextSaveK;
#elif defined(__ADSPBLACKFIN__)
    unsigned char      priority;        ///< 0 - 31 (at present)
    char               runStatus;       ///< ready, sleeping, etc.
    unsigned char      contextLevel;    ///< private to scheduler
    void              *pContextSave;    ///< stacktop on Blackfin
    void              *pStackBase;      ///< stackbase on Blackfin
#elif defined(__ADSP21000__)
    unsigned int       priority;        ///< 0 - 31 (at present)
    int                runStatus;       ///< ready, sleeping, etc.
    unsigned int       contextLevel;    ///< private to scheduler

    void              *pContextSave;
#else
#error Unknown processor family
#endif
} TMK_Thread;


typedef struct TMK_ThreadList {
    struct TMK_Thread *pHead;
    struct TMK_Thread *pTail;
} TMK_ThreadList;


typedef LCK_Lock TMK_Lock;


///////////////////////////////////////////////////////////////////////////////
//   Function pointer types
///////////////////////////////////////////////////////////////////////////////

typedef void (*TMK_PFCSwitch)(TMK_Thread *, TMK_Thread *);
typedef void (*TMK_PFDispatch)(void);


///////////////////////////////////////////////////////////////////////////////
//   Thread Functions
///////////////////////////////////////////////////////////////////////////////

unsigned int
TMK_QueryContextRecordSize(void);

void
TMK_InitThread
(
    TMK_Thread  *pThread,    // the thread to be initialised
    unsigned int priority,   // initial priority
    void       (*pfRunFunc)(TMK_Thread*),// thread run function
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
);

void
TMK_DeInitThread
(
    TMK_Thread *pThread     // the thread to be de-initialised
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
);

bool
TMK_ChangeThreadPriority
(
    TMK_Thread  *pThread,   // thread to be changed
    unsigned int priority   // new priority
);


///////////////////////////////////////////////////////////////////////////////
//   Ready Queue Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_MakeThreadReady
(
    TMK_Thread *pThread     
);

void
TMK_MakeThreadNotReady
(
    TMK_Thread *pThread,
    int newStatus  // the new (non-ready) state of the thread
);

bool
TMK_YieldThread(
    TMK_Thread *pThread
);


///////////////////////////////////////////////////////////////////////////////
//   Scheduler Functions
///////////////////////////////////////////////////////////////////////////////

TMK_Thread *
TMK_GetCurrentThread(void);

bool
TMK_AtKernelLevel(void);

void
TMK_Reschedule(void);

void
TMK_Block(void);

void
TMK_Run(void);

void
TMK_Stop(void);

void
TMK_SuspendScheduling(void);

void
TMK_ResumeScheduling(void);

bool
TMK_IsSchedulingSuspended(void);

TMK_PFCSwitch
TMK_HookThreadSwitch(TMK_HookSelector which, TMK_PFCSwitch pfHookFn);

TMK_PFDispatch
TMK_HookDispatch(TMK_PFDispatch pfHookFn);


///////////////////////////////////////////////////////////////////////////////
//   Time Queue Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_InitTimeElement(
    TMK_TimeElement *pElement,
    void           (*pfTimeout)(struct TMK_TimeElement *)
);

void
TMK_SetTimeout(
    TMK_TimeElement *pElement,
    int timeout
);

int
TMK_CancelTimeout(
    TMK_TimeElement *pElement
);

int
TMK_ResetTimeout(
    TMK_TimeElement *pElement,
    int timeout
);

int
TMK_GetTimeRemaining(
    TMK_TimeElement *pElement
);

#if defined(__ADSPBLACKFIN__)
#pragma regs_clobbered "R0 P1"
#endif
unsigned int
TMK_GetUptime(void);


///////////////////////////////////////////////////////////////////////////////
//   DPC Queue Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_InitDpcElement(
    TMK_DpcElement *pDpc,
    void          (*pfInvoke)(struct TMK_DpcElement *)
);

#if defined(__ADSPBLACKFIN__)
#pragma regs_clobbered "P1"
#endif
void
TMK_InsertQueueDpc(
    TMK_DpcElement *pDPC
);


///////////////////////////////////////////////////////////////////////////////
//   Thread List Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_InitThreadList(
    TMK_ThreadList *pList
);

void
TMK_AppendThreadToList(
    TMK_ThreadList *pList,
    TMK_Thread     *pThread
);

void
TMK_AddThreadToOrderedList(
    TMK_ThreadList *pList,
    TMK_Thread     *pThread
);

void
TMK_RemoveThreadFromList(
    TMK_Thread     *pThread
);

TMK_Thread *
TMK_GetNextThreadFromList(
    TMK_ThreadList *pList
);

TMK_Thread *
TMK_QueryNextThreadInList(
    TMK_ThreadList *pList
);

int
TMK_QueryThreadlist(
    TMK_ThreadList *pList,
    TMK_Thread     *vpThreads[],
    int arraySize
);

bool
TMK_IsThreadListEmpty(
    TMK_ThreadList *pList
);


///////////////////////////////////////////////////////////////////////////////
//   Locking Functions
///////////////////////////////////////////////////////////////////////////////

#define TMK_InitInterruptLock(pLock, precedence, interruptMask) \
        LCK_InitInterruptLock((pLock), (precedence), (interruptMask))

#define TMK_InitGlobalInterruptLock(pLock, precedence) \
        LCK_InitGlobalInterruptLock((pLock), (precedence))

#define TMK_AcquireGlobalInterruptLock(pLock)  \
        LCK_AcquireGlobalInterruptLock((pLock))

#define TMK_ReleaseGlobalInterruptLock(pLock, state) \
        LCK_ReleaseGlobalInterruptLock((pLock), (state))

#define TMK_AcquireInterruptLock(pLock) \
        LCK_AcquireInterruptLock((pLock))

#define TMK_ReleaseInterruptLock(pLock, state) \
        LCK_ReleaseInterruptLock((pLock), (state))

#define TMK_AcquireInterruptLockFromInterruptLevel(pLock) \
        LCK_AcquireInterruptLockFromInterruptLevel((pLock))

#define TMK_ReleaseInterruptLockFromInterruptLevel(pLock) \
        LCK_ReleaseInterruptLockFromInterruptLevel((pLock))

#define TMK_ChangeInterruptMaskBits(pLock, pState, setBits, clrBits) \
        LCK_ChangeInterruptMaskBits((pLock), (pState), (setBits), (clrBits))

#define TMK_GetInterruptMaskBits(pLock, state) \
        LCK_GetInterruptMaskBits((pLock), (state))


///////////////////////////////////////////////////////////////////////////////
//   Master Kernel Locking Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_AcquireMasterKernelLock(void);

#if defined(__ADSPBLACKFIN__)
#pragma regs_clobbered "P0 P1 P2 R0 R1 R2 R3 ASTAT RETS"
#endif
void
TMK_ReleaseMasterKernelLock(void);

void
TMK_AcquireMasterKernelLockFromKernelLevel(void);

void
TMK_ReleaseMasterKernelLockFromKernelLevel(void);

void
TMK_ReleaseGlobalInterruptAndMasterKernelLocks(
    TMK_Lock *pLock,
    TMK_LockState state
);


///////////////////////////////////////////////////////////////////////////////
//   Startup Functions
///////////////////////////////////////////////////////////////////////////////

void
TMK_Initialize(void);

///////////////////////////////////////////////////////////////////////////////
//   Macros
///////////////////////////////////////////////////////////////////////////////

/** Converts a pointer to a structure field into a pointer to the enclosing
 * struct, using the name of the field and the type of the struct.
 *
 * This macro is for use where embedding - rather than inheritance -
 * is used to aggregate TMK structs into macrokernel-defined
 * objects. It will therefore tend to be used more in C-based
 * macrokernels than in C++, even though the use of the macro is
 * scarcely any less elegant than the typecasting which is required
 * with inheritance. Also, the macro carries none of the potential
 * overhead which has to be worked around in C++ by the use of
 * #pragma suppress_null_check.
 *
 * @param T - the type of the enclosing struct
 * @param member - the name of the field within the struct
 * @param ptr - the pointer to the field
*/
#define TMK_GET_ENCLOSING(T, member, ptr) ((T *)(((char*)(ptr)) - offsetof(T, member)))

#define TMK_INIT_GLOBAL_INTERRUPT_LOCK(precedence) \
        LCK_INIT_GLOBAL_INTERRUPT_LOCK(precedence)

#if defined(__ADSPBLACKFIN__)
#define TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */       \
      (precedence),           \
      0, /* lockHeld */        \
      2 /* RESCHEDULE_IRPL*/ \
    }
#elif defined(__ADSPTS20x__)
#define TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */       \
      (precedence),           \
      0, /* lockHeld */        \
      1 /* RESCHEDULE_IRPL*/ \
    }
#elif defined(__ADSPTS101__)
#define TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */       \
      (precedence),           \
      0, /* lockHeld */        \
      3 /* RESCHEDULE_IRPL*/ \
    }
#elif defined(__ADSP21000__)
#ifdef __2106x__
#define TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */       \
      (precedence),           \
      0, /* lockHeld */        \
      2 /* RESCHEDULE_IRPL*/ \
    }
#else // all Hammerheads
#define TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */       \
      (precedence),           \
      0, /* lockHeld */        \
      3 /* RESCHEDULE_IRPL*/ \
    }
#endif // __ADSP216X
#else
#error Unknown processor family
#endif

///////////////////////////////////////////////////////////////////////////////
//   Function inlining support begins here
///////////////////////////////////////////////////////////////////////////////

// BEGIN_INTERNALS_STRIP

///////////////////////////////////////////////////////////////////////////////
//   Private globals
///////////////////////////////////////////////////////////////////////////////

#if defined(_TRINITY_H_) || defined(TMK_INLINE)

struct tmk_Globals
{
    TMK_LockState masterState;
    TMK_Lock      masterLock;
    TMK_Thread   *pCurrentThread;
    unsigned      schedulerRegionCount;
    TMK_PFCSwitch vpfCtxtSwitch[TMK_NUM_HOOK_FNS];
    TMK_PFDispatch pfDispatchHook;
};

extern struct tmk_Globals tmk;

#endif


#ifdef TMK_INLINE

#include <assert.h>



///////////////////////////////////////////////////////////////////////////////
//   Inline Thread Functions
///////////////////////////////////////////////////////////////////////////////

/** @return the required size of the context record. On Blackfin this space
 * isn't allocated separately but should be added to the stack size.
 */
TMK_INLINE
unsigned int
TMK_QueryContextRecordSize(void)
{
    extern const unsigned int tmk_ContextRecordSize;
    return tmk_ContextRecordSize;
}


///////////////////////////////////////////////////////////////////////////////
//   Inline Scheduler Functions
///////////////////////////////////////////////////////////////////////////////

/** Returns a pointer to the currently-running thread.
 *
 * @return a pointer to the structure for the currently-running
 * thread, as stored in the global data (never NULL unless at
 * kernel level, which is illegal for this function).
 */
TMK_INLINE
TMK_Thread *
TMK_GetCurrentThread(void)
{
	/** @pre Not at kernel level */
    assert(NULL != tmk.pCurrentThread);
    return tmk.pCurrentThread;
}


/** Indicates whether execution is at kernel level. Used by
 * macrokernel code to avoid calling functions such as
 * TMK_GetCurrentThread(), which depend on the current thread pointer.
 *
 * @return true if at kernel level, otherwise false
*/
TMK_INLINE
bool
TMK_AtKernelLevel(void)
{
	return NULL == tmk.pCurrentThread;
}


/** Suspends scheduling by incrementing the region count.
 *
 * In an SMP system, this call would suspend scheduling on the
 * current core *only*. There would be separate scheduler region
 * counts for each core.
 */
TMK_INLINE
void
TMK_SuspendScheduling(void)
{
	/**@pre Must not be called with the master kernel lock held. */
    assert(!tmk.masterLock.lockHeld);

    ++tmk.schedulerRegionCount;
}


/** Queries the suspend state of the scheduler.
 *
 * @return false if scheduling is active, true if suspended
 */
TMK_INLINE
bool
TMK_IsSchedulingSuspended(void)
{
    return 0 != tmk.schedulerRegionCount;
}


///////////////////////////////////////////////////////////////////////////////
//   Inline Thread List Functions
///////////////////////////////////////////////////////////////////////////////

/** Initializes a threadlist structure to the empty state.
*/
TMK_INLINE
void
TMK_InitThreadList(
    TMK_ThreadList *pList
)
{
    pList->pHead = (TMK_Thread*)pList;
    pList->pTail = (TMK_Thread*)pList;
}

/** Appends a thread to the specified thread list, in FIFO order
*/
TMK_INLINE
void
TMK_AppendThreadToList(
    TMK_ThreadList *pList,
    TMK_Thread     *pThread
)
{
    // Integrity check
    assert(pList->pHead->pPrev == (TMK_Thread*)pList);
    assert(pList->pTail->pNext == (TMK_Thread*)pList);

    assert(pThread->pNext == pThread);
    assert(pThread->pPrev == pThread);
    
    // Insert at tail of list
    // The cast might look like a hack, but in practice it's much
    // cleaner than the syntactic hoops we'd have to jump through to
    // make this work without it.
    pThread->pNext      = (TMK_Thread*)pList;
    pThread->pPrev      = pList->pTail;
    pList->pTail->pNext = pThread;
    pList->pTail        = pThread;
}


/** Removes the specified thread from the specified thread list.
 *
 * We don't actually need to know which list the
 * thread is on in order to remove it, so we don't
 * expect it as an argument.
 */
TMK_INLINE
void
TMK_RemoveThreadFromList(
    TMK_Thread     *pThread
)
{
    // Integrity check
    assert(pThread->pNext->pPrev == pThread);
    assert(pThread->pPrev->pNext == pThread);

    // Remove the thread from the list
    pThread->pPrev->pNext = pThread->pNext;
    pThread->pNext->pPrev = pThread->pPrev;
    pThread->pNext = pThread;
    pThread->pPrev = pThread;
}


/** Removes a thread from the head of the specified thread list.
 *
 * @return a pointer to the removed thread struct, or NULL if the list is empty
*/
TMK_INLINE
TMK_Thread *
TMK_GetNextThreadFromList(
    TMK_ThreadList *pList
)
{
    TMK_Thread *pThread = NULL;

    // Integrity check
    assert(pList->pHead->pPrev == (TMK_Thread*)pList);
    assert(pList->pTail->pNext == (TMK_Thread*)pList);

    if (pList->pHead != (TMK_Thread*)pList)
    {
        // The list isn't empty, get the first element
        pThread = pList->pHead;
        assert(pThread->pPrev == (TMK_Thread*)pList);
        
        // And remove it from the list
        pList->pHead = pThread->pNext;
        pThread->pNext->pPrev = (TMK_Thread*)pList;
        pThread->pNext = pThread;
        pThread->pPrev = pThread;
    }
    
    return pThread;
}


/** Returns a pointer to the thread at the head of the specified thread list, without removing it.
 *
 * @return a pointer to the first thread in the list, or NULL if the list is empty
 */
TMK_INLINE
TMK_Thread *
TMK_QueryNextThreadInList(
    TMK_ThreadList *pList
)
{
	TMK_Thread *pNextThread = NULL;

    // If the list's head ptr points to the list itself...
    if (pList->pHead == (TMK_Thread*)pList)
    {
		// ...then the list is empty, and we expect the
		// tail ptr also to point to the list.
		//
		assert(pList->pTail == (TMK_Thread*)pList);
    }
    else
    {
		// ...otherwise the list isn't empty. The head ptr
		// points to the first element in the list, and we
		// expect the tail ptr *not* to point to the list itself.
		//
		assert(pList->pTail != (TMK_Thread*)pList);
		pNextThread = pList->pHead;
    }

	return pNextThread;
}


/** Indicates whether a ThreadList is empty.
 *
 * @return true iff empty
 */
TMK_INLINE
bool
TMK_IsThreadListEmpty(
    TMK_ThreadList *pList
)
{
    return pList->pHead == (TMK_Thread*)pList;
}


///////////////////////////////////////////////////////////////////////////////
//   Inline Master Locking functions
///////////////////////////////////////////////////////////////////////////////

/** Acquire the master kernel lock.
 *
 * Note that the reschedule imask bit will *not* necessarily always be 1
 * outside the locked region, as there may be locks of lower
 * precedence than the master kernel lock, and one of these may be
 * held on entry here.
 */
TMK_INLINE
void
TMK_AcquireMasterKernelLock(void)
{
	/**@pre The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
	/**@pre The current prededence is lower than the master kernel lock precedence. */
    assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
    
    // Use the reschedule interrupt lock function
    tmk.masterState = LCK_AcquireRescheduleInterruptLock(&tmk.masterLock);
}


/** Release the master kernel lock.
 *
 * This should basically be equivalent to releasing an interrupt lock
 * on the reschedule interrupt level. Note that the reschedule imask
 * bit will *not* necessarily always be 1 outside the locked region,
 * as there may be locks of lower precedence than the master kernel
 * lock, and one of these may be held on entry here.
 */
#if defined(__ADSPBLACKFIN__)
#pragma regs_clobbered "P0 P1 P2 R0 R1 R2 R3 ASTAT RETS"
#endif
TMK_INLINE
void
TMK_ReleaseMasterKernelLock(void)
{
	/** @pre The Master kernel lock is currently held. */
    assert(tmk.masterLock.lockHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == lck_currLockPrecedence);
    
    // Use the general InterruptLock function
    LCK_ReleaseRescheduleInterruptLock(&tmk.masterLock, tmk.masterState);

	/**@post The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
	/**@post The current prededence is lower than the master kernel lock precedence. */
    assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
}


/** Faster release of two locks at once.
 *
 * This function is equivalent to calling TMK_ReleaseGlobalInterruptLock()
 * followed by TMK_ReleaseMasterKernelLock(), but is more efficient.
 *
 * @param[in,out] pLock - pointer to the global lock to be released
 * @param state - the previous interrupt state
 */
TMK_INLINE
void
TMK_ReleaseGlobalInterruptAndMasterKernelLocks(
    TMK_Lock *pLock,
    const TMK_LockState state
)
{
    assert(TMK_KERNEL_LOCK_PRECEDENCE == pLock->precedence);

    LCK_ReleaseGlobalAndRescheduleInterruptLocks(pLock,
						 state,
						 &tmk.masterLock,
						 tmk.masterState);

    assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
}


/** Acquires the master kernel lock without changing the interrupt mask.
 */
TMK_INLINE
void
TMK_AcquireMasterKernelLockFromKernelLevel(void)
{
	/**@pre The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
	/**@pre There are no other locks currently held. */
    assert(0 == lck_InterruptLocksHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
	/**@pre The current prededence is lower than the master kernel lock precedence. */
    assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
    
    // Use the reschedule interrupt lock function
    TMK_AcquireInterruptLockFromInterruptLevel(&tmk.masterLock);
}


/** Releases the master kernel lock without changing the interrupt mask.
*/
TMK_INLINE
void
TMK_ReleaseMasterKernelLockFromKernelLevel(void)
{
	/** @pre The Master Kernel lock is currently held. */
    assert(tmk.masterLock.lockHeld);
	/**@pre There are no other locks currently held. */
    assert(1 == lck_InterruptLocksHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == lck_currLockPrecedence);
    
    // Use the general InterruptLock function
    TMK_ReleaseInterruptLockFromInterruptLevel(&tmk.masterLock);

	/**@post The Master kernel lock is not currently held. */
    assert(!tmk.masterLock.lockHeld);
	/**@post There are no other locks currently held. */
    assert(0 == lck_InterruptLocksHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == tmk.masterLock.precedence);
	/**@post The current prededence is lower than the master kernel lock precedence. */
    assert(lck_currLockPrecedence < TMK_KERNEL_LOCK_PRECEDENCE);
}


///////////////////////////////////////////////////////////////////////////////
//   End Of Function Inlining Support
///////////////////////////////////////////////////////////////////////////////

#endif /* TMK_INLINE */

// END_INTERNALS_STRIP

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _TMK_H_ */
