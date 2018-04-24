/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file Locking.c
 * Contains the locking & synchronisation primitives
 * provided, and used by, the microkernel.
 *
 * The functions in this module have no dependency on any
 * other part of the microkernel and could, in principle,
 * be used to provide locking and synchronisation independently
 * of the microkernel itself.
 *
 * As well as providing intra-processor synchronisation through
 * masking of interrupts, these primitives also (optionally) implement
 * inter-processor (or inter-core) synchronisation via spinlocks, for
 * use on symmetric multiprocessing (SMP) platforms.
 *
 * To provide detection of potential deadlock situations, each lock
 * is associated with a numeric "precedence" level, in the range 1-255.
 * Lock acquisitions can only be nested in increasing numeric order
 * of precedence, violations of this will be detected at runtime in
 * debugging builds. The predecence ordering is entirely independent
 * of the interrupt levels of the locks.
 *
 * Each supported processor family has its own interrupt-masking code.
 * The spinlock-testing code is specific to Blackfin precessors as this
 * is the only platform where there is any possibility of symmetric
 * multiprocessing.
 */

#undef LCK_INLINE
#define LCK_INLINE

#if defined(__ADSPBLACKFIN__)
// The compiler does not work around anomaly 05000371 enough. A TAR has been
// entered TAR41165 but in the meantime we need to stop the warning/error

asm(".message/info 5516;");
#endif

#include "LCK.h"

#pragma file_attr("OS_Internals")

//****************************************************************************
// Private globals
//****************************************************************************

#if defined(__ADSPTS101__)
unsigned int lck_TS10xIMASKHShadow;
#endif

#ifndef NDEBUG
//
/// @internal
/// @brief Count of the number of active locks.
/// Used in debugging builds only.
unsigned int lck_InterruptLocksHeld = 0;

/// @internal
/// @brief Temporary storage for the precedence of the innermost active lock.
/// Used in debugging builds only.
unsigned char lck_currLockPrecedence = 0;
#endif

///////////////////////////////////////////////////////////////////////////////
//   Non-inline functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Initialise an interrupt lock object.
 *
 * This is the most general way of initialising a lock. It allows the
 * lock to be associated with a particular interrupt level, i.e. to
 * mask all interrupts up to and including that level when acquired.
 *
 * No global state is either read or altered by this function, and the
 * only memory locations touched (apart from stack) are the members
 * of the specified lock structure. It is therefore safe to call this function
 * at any time, even prior to microkernel initialisation.
 *
 * @param[out] pLock - pointer to the lock object
 * @param precedence - the lock's numerical precedence (1-255)
 * @param interruptMask - the interrupt level(s) masked by the lock.
 *        Only the highest-priority interrupt specified in the mask is
 *        significant, the other bits are ignored.
 */
void
LCK_InitInterruptLock(
    LCK_Lock *pLock,
    int precedence,
    LCK_IMaskUInt interruptMask
)
{
    assert_bf(0 == (interruptMask & 0x001F));
    assert_bf(0 != (interruptMask & 0xFFE0));
    /**@pre @e precedence must be between 1 and 255. */
    assert(precedence >= 1);
    assert(precedence <= 255);
    
    // Loop through the interrupt priority levels (in ascending
    // order of priority) until we find one which masks
    // all of the bits specified in interruptMask.
    //
    unsigned int irpl;
    for (irpl = 0;
         irpl < GLOBAL_IRPL;
         ++irpl)
    {
        if (0 == (interruptMask & IRPL_MASK(irpl)))
            break;
    }
    
    assert(irpl <= GLOBAL_IRPL); // range check
    assert_bf(irpl >= 2);   // blackfin-specific value
    
    pLock->irpl = irpl;
    pLock->spinByte = 0;
    
    debug(pLock->lockHeld = false);
    debug(pLock->precedence = precedence);
}


/**
 * Initialise a global interrupt lock object.
 *
 * This initialises a lock that will mask all interrupts when acquired.
 *
 * No global state is either read or altered by this function, and the
 * only memory locations touched (apart from stack) are the members
 * of the specified lock structure. It is therefore safe to call this function
 * at any time, even prior to microkernel initialisation.
 *
 * @param[out] pLock - pointer to the lock object
 * @param precedence - the lock's numerical precedence (1-255)
 */
void
LCK_InitGlobalInterruptLock(
    LCK_Lock *pLock,
    int precedence
)
{
    /**@pre @e precedence must be less than or equal to 255. */
    assert(precedence <= 255);

#ifdef USE_GENERIC_LOCKING
    // Use the general InterruptLock function
    LCK_InitInterruptLock(pLock, precedence, EVT_IVHW);
    assert(GLOBAL_IRPL == pLock->irpl);
#else
    pLock->irpl = GLOBAL_IRPL;
    pLock->spinByte = 0;
    
    debug(pLock->lockHeld = false);
    debug(pLock->precedence = precedence);
#endif
}

