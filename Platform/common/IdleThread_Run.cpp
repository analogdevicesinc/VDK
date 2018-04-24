/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/*
 *=============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Run function of the VDK idle thread
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The idle thread is the lowest priority threads, and runs when no
 *	other threads exist or when all other threads are blocked.  The idle thread
 *	does not have an ID (not entered in g_ThreadTable) and cannot be destroyed.
 *	During idle time, the system destroys any threads whose destruction has been
 *	postponed.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include "IdleThread.h"
#include "SlowIdle.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("IdleThread")


/** ----------------------------------------------------------------------------
 * FUNC   : IdleThread::Run()
 *
 * DESC   : The overloaded virtual function from VDK::Thread
 *
 *          Keeps trying to delete threads (if necessary).
 *          
 * PARAMS : N/A
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
namespace VDK {

void
IdleThread::Run()
{
    // Never exit this loop
	for(;;)
    {
		// delete all threads that are ready to be deleted
		while (KillThreadsInPurgatory()) {};
#ifdef __ADSPTS__
       // These nops are required because otherwise the loop is too tight and
       // we don't get kernel interrupts (only TS201 but does not hurt in TS101)
        asm("nop;nop;nop;nop;;");
#endif
#ifndef __ADSPTS__
        // we need to make sure there isn't a timing hole when a thread is
        // sent to purgatory, after the above line, and before we go into
        // the IDLE instruction

        TMK_LockState state = TMK_AcquireGlobalInterruptLock(&m_lock);

        // Check to make sure there are no threads to delete.
        if (TMK_IsThreadListEmpty(&m_threadList) )
#ifdef __ADSP21000__
        {
            // enable interrupts the cycle after going into low power IDLE.
            LCK_ReleaseGlobalInterruptLockAndIdle(&m_lock, state);
        }
        else
        {
            TMK_ReleaseGlobalInterruptLock(&m_lock, state);
        }
#else
        {
            // enable interrupts the cycle after going into low power IDLE.
            SlowIdle();
        }

        // Just in case we didn't go to the SlowIdle function, let's
        // re-enable interrupts.
        TMK_ReleaseGlobalInterruptLock(&m_lock, state);
#endif
#endif

     }
}

} // namespace VDK

