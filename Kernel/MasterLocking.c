/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file MasterLocking.c
 *
 * Contains the non-inlined master kernel lock functions.
 *
 * At present this is only tmk_MaskMasterKernelLockForThreadLevel
 * as this function is called from the context switch (only).
 * 
 */
#include <stdlib.h>
#include <assert.h>

#include "LCK.h"
#include "Trinity.h"
#pragma file_attr("Startup")


/**@internal
 * @brief Complete the acquisition of the master kernel lock.
 * 
 * This function is called if we acquired the master kernel lock
 * from kernel level and now want to return to thread level whilst
 * still holding the lock.
 *
 * Basically this means doing the work that we were able to skip when
 * acquiring the lock because we were at interrupt level, i.e. masking-
 * out the interrupt.
 *
 * This function is presently used only from
 * .RestoreRuntimeContextFromInterruptLevel in Switch.asm.
*/
void tmk_MaskMasterKernelLockForThreadLevel(void)
{
    assert(lck_InterruptLocksHeld); // kernel lock is an int lock
	/**@pre The master kernel lock is currently held. */
    assert(tmk.masterLock.lockHeld);
    assert(TMK_KERNEL_LOCK_PRECEDENCE == lck_currLockPrecedence);
    
    // Use the general InterruptLock function
    tmk.masterState = LCK_MaskRescheduleInterruptLockForThreadLevel(&tmk.masterLock);
}

