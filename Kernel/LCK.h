/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file LCK.h
 * Contains the locking & synchronisation primitives
 * provided - and used - by the microkernel.
 *
 * The functions in this module have no dependency on any
 * other part of the microkernel and could, in principle,
 * be used to provide locking and synchronisation independently
 * of the microkernel itself.
 *
 * As well as providing intra-processor synchronisation through
 * masking of interrupts, these primitives also (optionally) implement
 * inter-processor (or inter-core) synchronisation via spinlocks, for
 * use on symmetric multiprocessing (SMP) platforms [Blackfin only, untested].
 *
 * To provide detection of potential deadlock situations, each lock
 * is associated with a numeric "precedence" level, in the range 1-255.
 * Lock acquisitions can only be nested in increasing numeric order
 * of precedence, violations of this will be detected at runtime in
 * debugging builds. The predecence ordering is entirely *independent*
 * of the interrupt levels of the locks.
 */

#ifndef _LCK_H_
#define _LCK_H_


#include <assert.h>

#ifdef __ADSPBLACKFIN__
#include <sys/platform.h>

#endif

#ifdef __ADSPTS20x__
#include <defts201.h>
#include <sysreg.h>
#endif

#ifdef __ADSPTS101__
#include <defts101.h>
#include <sysreg.h>
#endif

#ifdef __ADSP21000__
#include <sysreg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
//#define USE_GENERIC_LOCKING

#ifdef USE_GENERIC_LOCKING
#error Generic locking not tested recently, and implemented only for Blackfin
#endif

/****************************************************************************
 *   Manifest Constants
 *****************************************************************************/

#define LCK_MIN_PRECEDENCE    1
#define LCK_MAX_PRECEDENCE    255

/****************************************************************************
 * Symmetric Multiprocessing (SMP) support macros (placeholders only)
 *****************************************************************************/

#define ACQUIRE_SPINLOCK(ptr)
#define RELEASE_SPINLOCK(ptr)

/****************************************************************************
 * Debugging macros
 *****************************************************************************/

#ifndef NDEBUG  // if building for debug

#define debug(x) (x)
#define debug_swap_precedence(pLock)          \
{                                             \
    unsigned char tmp = (pLock)->precedence;  \
    (pLock)->precedence = lck_currLockPrecedence; \
    lck_currLockPrecedence = tmp;                 \
}

#else   // not building for debug

#define debug(x)
#define debug_swap_precedence(pLock)

#endif

/****************************************************************************
 *   Simple type definitions
 *****************************************************************************/

#if defined(__ADSPBLACKFIN__)

typedef unsigned short LCK_IMaskUInt;
typedef unsigned int   LCK_LockState;

#elif defined(__ADSPTS__)

typedef unsigned long long LCK_IMaskUInt;
typedef unsigned long long LCK_LockState;

#elif defined(__ADSP21000__)

typedef unsigned int LCK_IMaskUInt;
typedef unsigned int LCK_LockState;

#else
#error Unknown processor family
#endif

/****************************************************************************
 *   Structures
 *****************************************************************************/

/** The Lock structure. Size is four bytes (one 32-bit word).
 */
typedef struct LCK_Lock {
    volatile unsigned char spinByte:8;   ///< Only used in SMP builds.
	                            ///< Must be at the start of the struct so it
                                ///< will be on a 4-byte boundary.
    unsigned char precedence:8; ///< For acquisition ordering.
	                            ///< Used in debug builds only
    unsigned char lockHeld:8;   ///< True iff lock is held.
	                            ///< Used in debug builds only
    unsigned char irpl:8;       ///< Interrupt priority level. Controls which
	                            ///< interrupts are masked while the lock is
	                            ///< being held.
} LCK_Lock;


/****************************************************************************
 *   Debug-only globals
 *****************************************************************************/

extern unsigned int lck_InterruptLocksHeld;
extern unsigned char lck_currLockPrecedence;


/****************************************************************************
 *   Locking & Synchronization Functions
 *****************************************************************************/

void
LCK_InitInterruptLock(
    LCK_Lock *pLock,
    int precedence,
    LCK_IMaskUInt interruptMask
);

void
LCK_InitGlobalInterruptLock(
    LCK_Lock *pLock,
    int precedence
);

LCK_LockState
LCK_AcquireGlobalInterruptLock(
    LCK_Lock *pLock
);

void
LCK_ReleaseGlobalInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
);

LCK_LockState
LCK_AcquireInterruptLock(
    LCK_Lock *pLock
);

void
LCK_ReleaseInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
);

LCK_LockState
LCK_AcquireRescheduleInterruptLock(
    LCK_Lock *pLock
);

void
LCK_ReleaseRescheduleInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
);

LCK_LockState
LCK_MaskRescheduleInterruptLockForThreadLevel(
    LCK_Lock *pLock
);

void
LCK_AcquireInterruptLockFromInterruptLevel(
    LCK_Lock *pLock
);

void
LCK_ReleaseInterruptLockFromInterruptLevel(
    LCK_Lock *pLock
);

void
LCK_ReleaseGlobalAndRescheduleInterruptLocks(
    LCK_Lock *pGlobalLock,
    const LCK_LockState globalState,
    LCK_Lock *pMasterLock,
    const LCK_LockState masterState
);

LCK_LockState
LCK_MaskInterruptLockForThreadLevel(
    LCK_Lock *pLock
);

void
LCK_ChangeInterruptMaskBits(
    LCK_Lock *pLock,
    LCK_LockState *pState,
    LCK_IMaskUInt setBits,
    LCK_IMaskUInt clrBits
);

LCK_IMaskUInt
LCK_GetInterruptMaskBits(
    LCK_Lock *pLock,
    LCK_LockState state
);


#ifdef __ADSP21000__
void
LCK_ReleaseGlobalInterruptLockAndIdle(
    LCK_Lock *pLock,
    LCK_LockState state
);
#endif

/****************************************************************************
 *   Macros
 *****************************************************************************/

#if defined(__ADSPBLACKFIN__)
#define LCK_INIT_GLOBAL_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */    \
      (precedence),        \
      0, /* lockHeld */     \
      11 /* GLOBAL_IRPL*/ \
    }
#elif defined(__ADSPTS20x__)
#define LCK_INIT_GLOBAL_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */    \
      (precedence),        \
      0, /* lockHeld */     \
      64 /* GLOBAL_IRPL*/ \
    }
#elif defined(__ADSPTS101__)
#define LCK_INIT_GLOBAL_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */    \
      (precedence),        \
      0, /* lockHeld */     \
      60 /* GLOBAL_IRPL*/ \
    }
#elif defined(__ADSP21000__)
#define LCK_INIT_GLOBAL_INTERRUPT_LOCK(precedence) \
    { 0, /* spinByte */    \
      (precedence),        \
      0, /* lockHeld */     \
      32 /* GLOBAL_IRPL*/ \
    }
#else
#error Unknown processor family
#endif


///////////////////////////////////////////////////////////////////////////////
//   Function Inlining Support
///////////////////////////////////////////////////////////////////////////////

// BEGIN_INTERNALS_STRIP

#ifdef LCK_INLINE

#include <stdlib.h>
#include <assert.h>


/****************************************************************************
 *   Inline Locking Functions
 *****************************************************************************/

#ifdef __ADSPBLACKFIN__

/****************************************************************************
 * Blackfin-specific
 ****************************************************************************/

#define assert_bf(x) assert(x)
#define assert_ts(x)
#define assert_sh(x)

#define IRPL_MASK(irpl) (0xFFFF >> (irpl))
#define GLOBAL_IRPL 11
#define RESCHEDULE_IRPL 2
#define MIN_IRPL 2
#define MASK_MERGE(l,r,irpl) \
    ((l & IRPL_MASK(irpl)) | (r & ~IRPL_MASK(irpl)))

#if !defined (__WORKAROUNDS_DISABLED) && (defined (__ADSPBF535__))
static inline unsigned int lck_CLI(void)
{
    unsigned int imask;
    asm volatile ("RETI = [SP++]; cli %0; [--sp] = RETI;" : "=d" (imask) : :);
    return imask;
}
#else
#define lck_CLI()  __builtin_cli()
#endif

#define lck_STI(x) __builtin_sti(x)

#elif defined(__ADSPTS20x__)

/****************************************************************************
 * TS20x(Danube)-specific
 *****************************************************************************/

#define assert_bf(x)
#define assert_ts(x) assert(x)
#define assert_sh(x)

#define IRPL_MASK(irpl) (0xFFFFFFFFFFFFFFFFULL << (irpl))
#define GLOBAL_IRPL 64
#define RESCHEDULE_IRPL 1
#define MIN_IRPL 1

#if 1
#define MASK_MERGE(l,r,irpl) \
    ((l & IRPL_MASK(irpl)) | (r & ~IRPL_MASK(irpl)))
#else
static inline unsigned long long
MASK_MERGE(unsigned long long l,
		   unsigned long long r,
		   int irpl)
{
#if 1
	unsigned long long mask = IRPL_MASK(irpl);
	unsigned long long result = l;
	asm volatile("%0 += MASK %1 BY %2(NF);;"
				 : "+ylL" (result)
				 : "yl" (r) , "yl" (mask));
#else
	unsigned long long mask = IRPL_MASK(irpl);
	unsigned long long result = l;
	asm volatile("%0 += MASK %1 BY %2(NF);;"
				 : "+ylL" (result)
				 : "yl" (r) , "yl" (mask));
#endif
	return result;
}
#endif

//
// Globally disable interrupts
//
static inline unsigned int lck_GID(void)
{
	unsigned int mask = __builtin_sysreg_read(__SQCTL) & SQCTL_GIE;

    asm volatile("SQCTLCL = 0xFFFFFFFB;;"); /* clear GIE*/

	return mask;
}

//
// Globally (conditionally) re-enable interrupts
//
static inline void lck_GIE(unsigned int mask)
{
	assert(0 == (mask & ~SQCTL_GIE));

	__builtin_sysreg_write(__SQCTLST, mask);
}

//
// Return the whole IMASK as a 64-bit value
//
static inline unsigned long long lck_getImask(void)
{
    return __builtin_compose_64u(__builtin_sysreg_read(__IMASKL),
								 __builtin_sysreg_read(__IMASKH));
}

//
// Set the whole IMASK to the supplied 64-bit value
//
static inline void lck_setImask(unsigned long long imask)
{
	__builtin_sysreg_write(__IMASKL, __builtin_low_32u(imask));
	__builtin_sysreg_write(__IMASKH, __builtin_high_32u(imask));
}

//
// Wait for the effect of clearing imask bits to make
// its way through the pipe.
//
static inline void lck_imaskWait(void)
{
    asm volatile("J0 = IMASKH;;"
				 "nop;;"
				 "J0 = J0 + J31(NF);;" : : : "J0" );
}

#elif defined(__ADSPTS101__)

/****************************************************************************
 * TS10x(Nile)-specific
 ****************************************************************************/

extern unsigned int lck_TS10xIMASKHShadow;

#define assert_bf(x)
#define assert_ts(x) assert(x)
#define assert_sh(x)

#define IRPL_MASK(irpl) (0xFFFFFFFFFFFFFFFFULL << (irpl))
#define GLOBAL_IRPL 60
#define RESCHEDULE_IRPL 3
#define MIN_IRPL 3
#define MASK_MERGE(l,r,irpl) \
    ((l & IRPL_MASK(irpl)) | (r & ~IRPL_MASK(irpl)))

//
// Globally disable interrupts and return the contents of IMASK.
//
static inline unsigned long long lck_CLI()
{
    // This is the critical vulnerability to anomaly 03000268, which has to
	// be worked-around here and in the context switch. We can take an
	// interrupt in the 2 cycles after we clear GIE in IMASKH, and this can
	// lead to us being in the reschedule ISR with the GIE bit clear. In
	// this case *only* the context-switch back to this thread will restore
	// what was in IMASKH at the time of the interrupt.
	//
	unsigned long newMask = INT_SW | INT_EMUL;  // used twice below
	__builtin_sysreg_write(__IMASKH, newMask);
	asm volatile ("NOP;;NOP;;");

	// From this point interrupts should be fully disabled, so it's
	// safe to actually read the state of things.
	//
    unsigned long long result = __builtin_compose_64u(__builtin_sysreg_read(__IMASKL),
								 lck_TS10xIMASKHShadow);
	lck_TS10xIMASKHShadow = newMask;  // now update the shadow
	__builtin_sysreg_write(__IMASKL, 0);

	return result;
}

//
// Set IMASK to the specified value and (depending on the restored value)
// globally re-enable interrupts
//
static inline void lck_STI(unsigned long long imask)
{
	__builtin_sysreg_write(__IMASKL, __builtin_low_32u(imask));

	__builtin_sysreg_write(__IMASKH,
						   lck_TS10xIMASKHShadow = __builtin_high_32u(imask));
}

#elif defined(__ADSP21000__)

/****************************************************************************
 * SHARC-specific
 ****************************************************************************/

#define assert_bf(x)
#define assert_ts(x)
#define assert_sh(x) assert(x)

#define IRPL_MASK(irpl) (0xFFFFFFFFUL >> (unsigned long)(irpl))
#define GLOBAL_IRPL 32

// The software interrupts (SFT0I-SFT3I) are at different bit positions (28:31)
// on 2106x than on the later SHARCs (27:30), so the IRPL values are different.
// Note that the reschedule interrupt level (SFT2I) is no longer the same as
// the minimum interrupt level (SFT3I), this is because we are using SFT3I as
// a "trampoline" to get to SFT2I in order to avoid the problem of the
// hardware force-clearing the IRPTL bit during an ISR.
//
#ifdef __2106x__
#define RESCHEDULE_IRPL 2
#define MIN_IRPL 1
#else
#define RESCHEDULE_IRPL 3
#define MIN_IRPL 2
#endif // __2106x__

#define MASK_MERGE(l,r,irpl) \
    ((l & IRPL_MASK(irpl)) | (r & ~IRPL_MASK(irpl)))

//
// Globally disable interrupts
//
static inline unsigned int lck_GID(void)
{
	unsigned int result = __builtin_sysreg_bit_tst(sysreg_MODE1, 0x1000 /*IRPTEN*/);

/* in VISA the length of the instructions can be different so we can no
   longer do a jump (PC,3). But we cannot have a straight label to avoid
   multiple definitions so we automatically generate them */
asm volatile(" #define atomic_disable JUMP (PC,.SH_INT_DISABLED?) (DB); \
         bit clr MODE1 0x1000; /* clear GIE*/ \
         nop; \
         .SH_INT_DISABLED?: "
"\natomic_disable");

	return result;
}

//
// Globally (conditionally) re-enable interrupts
//
static inline void lck_GIE(unsigned int mask)
{
	if (mask)
		asm volatile("bit SET MODE1 0x1000;"); /* set GIE*/
}

//
// Return the whole IMASK
//
static inline unsigned long lck_getImask(void)
{
    return __builtin_sysreg_read(sysreg_IMASK);
}

//
// Set the whole IMASK to the supplied value
//
static inline void lck_setImask(unsigned long imask)
{
	__builtin_sysreg_write(sysreg_IMASK, imask);
}

//
// Wait for the effect of clearing imask bits to make
// its way through the pipe.
//
static inline void lck_imaskWait(void)
{
		asm("nop;"	   	        \
			"nop;"	   	        \
			"nop;"	   	        \
			"nop;"	   	        \
			"nop;"	   	        \
			"nop;");
}

#else
#error Unknown processor family
#endif


/****************************************************************************
 * Private globals
 *****************************************************************************/

#ifndef NDEBUG
extern unsigned int lck_InterruptLocksHeld;
extern unsigned char lck_currLockPrecedence;
#endif

/****************************************************************************
 *   Locking & Synchronization Functions
 *****************************************************************************/

/**
 * Acquire a lock, selectively masking interrupts.
 *
 * This is the fundamental locking operation, all
 * lock acquisitions can be expressed using this function.
 *
 * @param[in,out] pLock - pointer to the lock to be acquired
 * @return the previous interrupt state
 */
LCK_INLINE
LCK_LockState
LCK_AcquireInterruptLock(
    LCK_Lock *pLock
)
{
    /**@pre The lock's precedence must be greater than the current precedence */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@pre The lock must not be currently held. */
    assert(!pLock->lockHeld);

    assert(pLock->irpl >= MIN_IRPL);

#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int state = lck_GID();
    const LCK_LockState imask = lck_getImask();
#else
    const LCK_LockState imask = lck_CLI();
#endif

    /// Interrupts are masked *before* acquiring the spinlock. This is
    /// necessary to ensure that we can't be interrupted once the spinlock
    /// has been acquired.
    ///
    const LCK_LockState newIMask = imask & IRPL_MASK(pLock->irpl);
    
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(newIMask);
	lck_imaskWait();
	lck_GIE(state);
#else
    lck_STI(newIMask);
#endif

	// We can't try to acquire a spinlock until we have set the new
	// imask and re-enabled global interrupts. Otherwise we would spin
	// at global irpl, which would be incorrect.
    ACQUIRE_SPINLOCK(pLock);
    
	// We do this stuff after the spinlock has been acquired because
	// we aren't really holding the lock until then.
    debug_swap_precedence(pLock);
    debug(++lck_InterruptLocksHeld);
    debug(pLock->lockHeld = true);
    
    return imask;
}


/**
 * Release a lock, selectively restoring interrupt state.
 *
 * Releasing the lock involves first releasing the spinlock
 * (by writing a zero byte to it) and then restoring the
 * previous state of the masked interrupt levels.
 *
 * @param[in,out] pLock - pointer to the lock to be released
 * @param state - the previous interrupt state
 */
LCK_INLINE
void
LCK_ReleaseInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
)
{
    /**@pre The lock must be currently held. */
    assert(pLock->lockHeld);
    assert(1 <= lck_InterruptLocksHeld);
    assert(pLock->precedence < lck_currLockPrecedence); // because of swap
    
    /// We allow for the possibility that IMASK may have been changed
    /// (by user code) while the lock was held. We do this by mask-merging
    /// the current contents of IMASK with the previous state, such that
    /// the bits for interrupt levels higher than the level of the lock
    /// remain unchanged in IMASK, and only those for levels masked by the
    /// lock are taken from the previous state. This is mainly of use
    /// with locks of low interrupt priority, such as reschedule locks.
    /// It serves no purpose with global interrupt locks as no bits will
    /// be retained from the current IMASK.
    ///
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int gid = lck_GID();
    LCK_LockState imask = lck_getImask();
#else
    LCK_LockState imask = lck_CLI();
#endif
    
    assert_bf(0 == (imask & ~IRPL_MASK(pLock->irpl) & 0xFFE0));

    // Keep unmasked bits and copy the masked bits from state
	imask = MASK_MERGE(imask, state, pLock->irpl);

    debug_swap_precedence(pLock);
    debug(--lck_InterruptLocksHeld);
    debug(pLock->lockHeld = false);
   
    /// The spinlock is released *before* restoring the interrupt mask.
    RELEASE_SPINLOCK(pLock);
       
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(imask);
	lck_GIE(gid);
#else
    lck_STI(imask);
#endif

    /**@post The current precedence is less than the lock's precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@post The lock is not currently held. */
    assert(!pLock->lockHeld);
}


/**
 * Acquire a lock without changing the interrupt mask.
 *
 * When executing at an interrupt level which is greater than
 * or equal to the level masked by a lock it is not necessary to
 * change the interrupt mask to prevent interruption. However, for
 * SMP environments it *is* still necessary to acquire the
 * spinlock.
 *
 * @param[in,out] pLock - pointer to the lock to be acquired
 */
LCK_INLINE
void
LCK_AcquireInterruptLockFromInterruptLevel(
    LCK_Lock *pLock
)
{
    /**@pre The lock's precedence must be greater than the current precedence */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@pre The lock must not be currently held. */
    assert(!pLock->lockHeld);

    // Check that the interrupt level of
    // the lock is <= the current irql.
#ifdef __ADSPLPBLACKFIN__
    assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) != 0);
    // SF I was getting this assertion in tests. BW has to reconsider what
    // the correct one is
    //assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) == (0x8000 >> pLock->irpl));
#endif

    ACQUIRE_SPINLOCK(pLock);
    
    debug_swap_precedence(pLock);
    debug(++lck_InterruptLocksHeld);
    debug(pLock->lockHeld = true);
}


/**
 * Release a lock without changing the interrupt mask.
 *
 * This function is used *only* where a lock has been acquired at
 * interrupt level, using LCK_AcquireInterruptLockFromInterruptLevel().
 * Since the interrupt mask was not changed when acquiring the lock it
 * is not necessary to restore the mask when releasing. For SMP
 * environments, however, it is still necessary to release the spinlock.
 *
 * @param[in,out] pLock - pointer to the lock to be released
 */
LCK_INLINE
void
LCK_ReleaseInterruptLockFromInterruptLevel(
    LCK_Lock *pLock
)
{
    assert(pLock->precedence < lck_currLockPrecedence); // because of swap
    /**@pre The lock must be currently held. */
    assert(pLock->lockHeld);

    /// Check that the interrupt level of
    /// the lock is <= the current irql.
#ifdef __ADSPLPBLACKFIN__
    assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) != 0);
    // SF I was getting this assertion in tests. BW has to reconsider what
    // the correct one is
    //assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) == (1 >> pLock->irpl));
#endif

    debug_swap_precedence(pLock);
    debug(--lck_InterruptLocksHeld);
    debug(pLock->lockHeld = false);
    
    RELEASE_SPINLOCK(pLock);
    
    /**@post The current precedence is less than the lock's precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@post The lock is not currently held. */
    assert(!pLock->lockHeld);
}


/**@internal
 * @brief Complete the acquisition of an interrupt-level lock.
 *
 * If a lock was acquired using LCK_AcquireInterruptLockFromInterruptLevel()
 * then the changing of IMASK was skipped because interrupts were
 * already blocked by the hardware. If we now want to return from
 * the ISR while still holding the lock then we have to finish the job
 * by changing IMASK.
 *
 * This function is presently used only in the implementation
 * of lck_MaskMasterKernelLockForThreadLevel(), which in turn is used
 * only from .RestoreRuntimeContextFromInterruptLevel in Switch.asm.
 * 
 * @return the previous interrupt state
*/
LCK_INLINE
LCK_LockState
LCK_MaskInterruptLockForThreadLevel(
    LCK_Lock *pLock
)
{
    /**@internal
     *@pre The lock must be currently held. */
    assert(pLock->lockHeld);
    assert(lck_InterruptLocksHeld);

    assert(pLock->irpl >= MIN_IRPL);

    /// Check that the interrupt level of
    /// the lock is <= the current irpl.
#ifdef __ADSPLPBLACKFIN__
    assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) != 0);
    // SF I was getting this assertion in tests. BW has to reconsider what
    // the correct one is
    //assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) == (1 >> pLock->irpl));
#endif
    
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int state = lck_GID();
    LCK_LockState imask = lck_getImask();
#else
    LCK_LockState imask = lck_CLI();
#endif
    
    assert_bf(imask != 0x001F);

    const LCK_LockState newIMask = imask & IRPL_MASK(pLock->irpl);

#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(newIMask);
	lck_imaskWait();
	lck_GIE(state);
#else
    lck_STI(newIMask);
#endif

    return imask;
}


/**
 * Acquire a lock, masking all interrupts.
 *
 * Since a global lock does not require selective masking of
 * interrupts this function is able to use a faster code sequence
 * than the general case. However, it includes an alternative
 * implementation which calls the general function and can be used
 * to help verify correctness.
 *
 * @param[in,out] pLock - pointer to the lock to be acquired
 * @return the previous interrupt state
 */
LCK_INLINE
LCK_LockState
LCK_AcquireGlobalInterruptLock(
    LCK_Lock *pLock
)
{
    /**@pre The lock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pLock->irpl);
    /**@pre The lock's precedence must be greater than the current precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@pre The lock must not be currently held. */
    assert(!pLock->lockHeld);
    
#ifdef USE_GENERIC_LOCKING
    // Use the general InterruptLock function
    return LCK_AcquireInterruptLock(pLock);
#else
    // Faster code which does only what is necessary for
    // disabling all interrupts.
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
    const LCK_LockState imask = lck_GID();
#else
    const LCK_LockState imask = lck_CLI();
#endif
    ACQUIRE_SPINLOCK(pLock);
    
    debug_swap_precedence(pLock);
    debug(++lck_InterruptLocksHeld);
    debug(pLock->lockHeld = true);
    
    return imask;
#endif
}


/**
 * Release a lock, restoring interrupt state.
 *
 * As for LCK_ReleaseInterruptLock(), releasing the lock
 * involves first releasing the spinlock (by writing a zero byte to
 * it) [SMP only] and then restoring the previous state of the masked interrupt
 * levels.
 *
 * Unlike LCK_ReleaseInterruptLock(), however, there is no need to
 * mask-merge any of the current IMASK bits into the new state, as
 * all of the interrupt level bits are taken from the saved state.
 *
 * @param[in,out] pLock - pointer to the lock to be released
 * @param state - the previous interrupt state
 */
LCK_INLINE
void
LCK_ReleaseGlobalInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
)
{
    /**@pre The lock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pLock->irpl); // check that it's really global

#ifdef USE_GENERIC_LOCKING
    // Use the general InterruptLock function
    LCK_ReleaseInterruptLock(pLock, state);
#else
    // Faster code which does only what is necessary for
    // re-enabling all interrupts.
    assert(pLock->precedence < lck_currLockPrecedence); // because of swap
    /**@pre The lock must be currently held. */
    assert(pLock->lockHeld);

    debug_swap_precedence(pLock);
    debug(--lck_InterruptLocksHeld);
    debug(pLock->lockHeld = false);
       
    RELEASE_SPINLOCK(pLock);
    
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	assert_ts(0 == (SQCTL_GIE & __builtin_sysreg_read(__SQCTL))); // GIE must be zero
	assert_sh(0 == __builtin_sysreg_bit_tst(sysreg_IMASK, 0x1000 /*IRPTEN*/));
	lck_GIE(state);
#else
    lck_STI(state);
#endif
#endif    
    
    /**@post The current precedence is less than the lock's precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@post The lock is not currently held. */
    assert(!pLock->lockHeld);
}

#ifdef __ADSP21000__

/**
 * Release a lock, restoring interrupt state.
 *
 * As for LCK_ReleaseInterruptLock(), releasing the lock
 * involves first releasing the spinlock (by writing a zero byte to
 * it) and then restoring the previous state of the masked interrupt
 * levels.
 *
 * Unlike LCK_ReleaseInterruptLock(), however, there is no need to
 * mask-merge any of the current IMASK bits into the new state, as
 * all of the interrupt level bits are taken from the saved state.
 *
 * @param[in,out] pLock - pointer to the lock to be released
 * @param state - the previous interrupt state
 */
LCK_INLINE
void
LCK_ReleaseGlobalInterruptLockAndIdle(
    LCK_Lock *pLock,
    LCK_LockState state
)
{
    /**@pre The lock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pLock->irpl); // check that it's really global
    /**@pre Must be the only lock currently held. */
	assert(1 == lck_InterruptLocksHeld);

#ifdef USE_GENERIC_LOCKING
	#error No generic locking support for IDLE
#else
    // Faster code which does only what is necessary for
    // re-enabling all interrupts.
    assert(pLock->precedence < lck_currLockPrecedence); // because of swap
    /**@pre The lock must be currently held. */
    assert(pLock->lockHeld);

    debug_swap_precedence(pLock);
    debug(--lck_InterruptLocksHeld);
    debug(pLock->lockHeld = false);
       
    RELEASE_SPINLOCK(pLock);
    
	assert(0 == __builtin_sysreg_bit_tst(sysreg_IMASK, 0x1000 /*IRPTEN*/));

    if (state)
		asm volatile("bit SET MODE1 0x1000;" // set GIE
					 "idle;");               // and idle
#endif    
    
    /**@post The current precedence is less than the lock's precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@post The lock is not currently held. */
    assert(!pLock->lockHeld);
}
#endif // __ADSP21000__

/**
 * Acquire a lock, masking the reschedule interrupt.
 *
 *
 * @param[in,out] pLock - pointer to the lock to be acquired
 * @return the previous interrupt state
 */
LCK_INLINE
LCK_LockState
LCK_AcquireRescheduleInterruptLock(
    LCK_Lock *pLock
)
{
    /**@pre The lock's precedence must be greater than the current precedence */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@pre The lock must not be currently held. */
    assert(!pLock->lockHeld);
    /**@pre The lock's irpl must be the reschedule level. */
    assert(RESCHEDULE_IRPL == pLock->irpl);

#ifdef USE_GENERIC_LOCKING
    // Use the general InterruptLock function
    return LCK_AcquireInterruptLock(pLock);
#else
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int state = lck_GID();
    const LCK_LockState imask = lck_getImask();
#else
    const LCK_LockState imask = lck_CLI();
#endif

    /// Interrupts are masked *before* acquiring the spinlock. This is
    /// necessary to ensure that we can't be interrupted once the spinlock
    /// has been acquired.
    ///
    const LCK_LockState newIMask = imask & IRPL_MASK(RESCHEDULE_IRPL);
    
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(newIMask);
	lck_imaskWait();
	lck_GIE(state);
#else
    lck_STI(newIMask);
#endif

	// We can't try to acquire a spinlock until we have set the new
	// imask and re-enabled global interrupts. Otherwise we would spin
	// at global irpl, which would be incorrect.
    ACQUIRE_SPINLOCK(pLock);
    
	// We do this stuff after the spinlock has been acquired because
	// we aren't really holding the lock until then.
    debug_swap_precedence(pLock);
    debug(++lck_InterruptLocksHeld);
    debug(pLock->lockHeld = true);
    
    return imask;
#endif
}


/**
 * Release a lock, selectively restoring interrupt state.
 *
 * Releasing the lock involves first releasing the spinlock
 * (by writing a zero byte to it) and then restoring the
 * previous state of the masked interrupt levels.
 *
 * @param[in,out] pLock - pointer to the lock to be released
 * @param state - the previous interrupt state
 */
LCK_INLINE
void
LCK_ReleaseRescheduleInterruptLock(
    LCK_Lock *pLock,
    LCK_LockState state
)
{
    /**@pre The lock must be currently held. */
    assert(pLock->lockHeld);
    assert(1 <= lck_InterruptLocksHeld);
    assert(pLock->precedence < lck_currLockPrecedence); // because of swap
    /**@pre The lock's irpl must be the reschedule level. */    
    assert(RESCHEDULE_IRPL == pLock->irpl);

#ifdef USE_GENERIC_LOCKING
    // Use the general InterruptLock function
    LCK_ReleaseInterruptLock(pLock, state);
#else
    /// We allow for the possibility that IMASK may have been changed
    /// (by user code) while the lock was held. We do this by mask-merging
    /// the current contents of IMASK with the previous state, such that
    /// the bits for interrupt levels higher than the level of the lock
    /// remain unchanged in IMASK, and only those for the reschedule
    /// interrupt (and below, if any) are taken from the previous state.
    ///
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int gid = lck_GID();
    LCK_LockState imask = lck_getImask();
#else
    LCK_LockState imask = lck_CLI();
#endif
    
    assert_bf(0 == (imask & ~IRPL_MASK(pLock->irpl) & 0xFFE0));

    // Keep unmasked bits and copy the masked bits from state
	imask = MASK_MERGE(imask, state, RESCHEDULE_IRPL);

    debug_swap_precedence(pLock);
    debug(--lck_InterruptLocksHeld);
    debug(pLock->lockHeld = false);
   
    /// The spinlock is released *before* restoring the interrupt mask.
    RELEASE_SPINLOCK(pLock);
       
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(imask);
	lck_GIE(gid);
#else
    lck_STI(imask);
#endif
#endif

    /**@post The current precedence is less than the lock's precedence. */
    assert(pLock->precedence > lck_currLockPrecedence);
    /**@post The lock is not currently held. */
    assert(!pLock->lockHeld);
}


/**@internal
 * @brief Complete the acquisition of an interrupt-level lock.
 *
 * If a lock was acquired using LCK_AcquireInterruptLockFromInterruptLevel()
 * then the changing of IMASK was skipped because interrupts were
 * already blocked by the hardware. If we now want to return from
 * the ISR while still holding the lock then we have to finish the job
 * by changing IMASK.
 *
 * This function is presently used only in the implementation
 * of lck_MaskMasterKernelLockForThreadLevel(), which in turn is used
 * only from .RestoreRuntimeContextFromInterruptLevel in Switch.asm.
 * 
 * @return the previous interrupt state
*/
LCK_INLINE
LCK_LockState
LCK_MaskRescheduleInterruptLockForThreadLevel(
    LCK_Lock *pLock
)
{
    /**@internal
     *@pre The lock must be currently held. */
    assert(pLock->lockHeld);
    assert(lck_InterruptLocksHeld);
    /**@pre The lock's irpl must be the reschedule level. */
    assert(RESCHEDULE_IRPL == pLock->irpl);

    /// Check that the interrupt level of
    /// the lock is <= the current irpl.
#ifdef __ADSPLPBLACKFIN__
    assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) != 0);
    // SF I was getting this assertion in tests. BW has to reconsider what
    // the correct one is
    //assert((*pIPEND & IRPL_MASK(pLock->irpl - 1)) == (1 >> pLock->irpl));
#endif
    
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	unsigned int state = lck_GID();
    LCK_LockState imask = lck_getImask();
#else
    LCK_LockState imask = lck_CLI();
#endif
    
    assert_bf(imask != 0x001F);

    const LCK_LockState newIMask = imask & IRPL_MASK(RESCHEDULE_IRPL);

#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	lck_setImask(newIMask);
	lck_imaskWait();
	lck_GIE(state);
#else
    lck_STI(newIMask);
#endif

    return imask;
}


/**
 * Faster release of two locks at once. The first (pGlobalLock) is the higher
 * in precedence and must therefore have been acquired last, it must
 * also be a global lock. The second lock (pMasterLock) must be a
 * reschedule lock.
 *
 * This function is equivalent to calling LCK_ReleaseGlobalInterruptLock()
 * followed by LCK_ReleaseRescheduleInterruptLock(), but is more efficient.
 *
 * @param[in,out] pGlobalLock - pointer to the global lock to be released
 * @param globalState - the interrupt state associated with *pGlobalLock
 * @param[in,out] pMasterLock - pointer to the interrupt lock to be released
 * @param masterState - the interrupt state associated with *pMasterLock
 */
LCK_INLINE
void
LCK_ReleaseGlobalAndRescheduleInterruptLocks(
    LCK_Lock *pGlobalLock,
    const LCK_LockState globalState,
    LCK_Lock *pMasterLock,
    const LCK_LockState masterState
)
{
    /**@pre *pGlobalLock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pGlobalLock->irpl); // check that it's really global
    /**@pre *pMasterLock's irpl must be the reschedule level. */
	assert(RESCHEDULE_IRPL == pMasterLock->irpl);

#ifdef USE_GENERIC_LOCKING
    LCK_ReleaseInterruptLock(pGlobalLock, globalState);
    LCK_ReleaseInterruptLock(pMasterLock, masterState);
#else
    /**@pre *pGlobalLock must be currently held. */
    assert(pGlobalLock->lockHeld);
    /**@pre *pMasterLock must be currently held. */
    assert(pMasterLock->lockHeld);

	// ???    assert(0 == (globalState & IRPL_MASK(pMasterLock->irpl)));

    debug_swap_precedence(pGlobalLock);
    debug_swap_precedence(pMasterLock);
    debug(lck_InterruptLocksHeld -= 2); // count both locks
    debug(pGlobalLock->lockHeld = false);
    debug(pMasterLock->lockHeld = false);

	/// If SMP is in use, then the spinlock of the global lock is released
	/// first, then that of the master kernel lock. Only then is the interrupt
	/// mask restored.
    RELEASE_SPINLOCK(pGlobalLock);
    RELEASE_SPINLOCK(pMasterLock);

    // There can be locks of lower precedence than masterLock,
    // so we must merge-in the old mask.
	//
#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
	assert_ts(0 == (SQCTL_GIE & __builtin_sysreg_read(__SQCTL))); // GIE must be zero
	assert_sh(0 == __builtin_sysreg_bit_tst(sysreg_IMASK, 0x1000 /*IRPTEN*/));
	lck_setImask(MASK_MERGE(lck_getImask(), masterState, RESCHEDULE_IRPL)); // ordering???
	lck_GIE(globalState);
#else
    // Copy the low-priority bits from masterState and the rest from globalState
	lck_STI(MASK_MERGE(globalState, masterState, RESCHEDULE_IRPL)); // ordering???
#endif
#endif
}


/**
 * Modifies the stored interrupt mask associated with a currently-held
 * lock. The result is that when the lock is released it is the updated mask which comes
 * into effect.
 *
 * @param[in] pLock - pointer to the lock to be used
 * @param[in,out] pState - the state to be updated
 * @param setBits - bits to be set in the interrupt mask
 * @param clrBits - bits to be cleared in the interrupt mask
 */
LCK_INLINE
void
LCK_ChangeInterruptMaskBits(
    LCK_Lock *pLock,
    LCK_LockState *pState,
    LCK_IMaskUInt setBits,
    LCK_IMaskUInt clrBits
)
{
    /**@pre *pLock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pLock->irpl);
    /**@pre *pLock must be currently currently held. */
    assert(pLock->lockHeld);
	/**@pre *pLock must be the only lock held. */
	assert(1 == lck_InterruptLocksHeld);

#if defined(__ADSPTS20x__) || defined(__ADSP21000__)
    lck_setImask((lck_getImask() | setBits) & ~clrBits);
#else
    *pState = (*pState | setBits) & ~clrBits;
#endif
}


/**
 * Returns the interrupt mask associated with a currently-held
 * lock.
 *
 * @param[in] pLock - pointer to the lock to be used
 * @param state - the state to be queried
 */
LCK_INLINE
LCK_IMaskUInt
LCK_GetInterruptMaskBits(
    LCK_Lock *pLock,
    LCK_LockState state
)
{
    /**@pre *pLock must have been initialized as a global lock. */
    assert(GLOBAL_IRPL == pLock->irpl);
    /**@pre *pLock must be currently held. */
    assert(pLock->lockHeld);
	/**@pre *pLock must be the only lock currently held. */
	assert(1 == lck_InterruptLocksHeld);

#if defined(__ADSPBLACKFIN__)
	// Mask out the "permanently-on" bits of the Blackfin IMASK
	return state & 0xFFE0;
#elif defined(__ADSPTS20x__)
	return lck_getImask();
#elif defined(__ADSP21000__)
	return lck_getImask() & 0xFFFFFFFC;
#elif defined(__ADSPTS101__)
	return state;
#else
#error unknown processor family
#endif
}


/****************************************************************************
 *   End Of Function Inlining Support
 *****************************************************************************/

#endif /* LCK_INLINE */

// END_INTERNALS_STRIP

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _LCK_H_ */
