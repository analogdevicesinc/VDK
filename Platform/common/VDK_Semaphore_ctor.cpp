/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Semaphore class constructor
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Semaphores provide a mechanism that prevents multiple clients 
 *  from accessing the same system resource inappropriately..
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#include "Semaphore.h"
#include "VDK_Thread.h"
#include <new>
#include "TMK.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")
#pragma file_attr("SignalConstruction")
#pragma file_attr("SemaphoreConstruction")



/** ----------------------------------------------------------------------------
 * FUNC   : VDK::Semaphore::Semaphore()
 *
 * DESC   : Constructor for a semaphore class.
 *
 * PARAMS : enum_value, initial_value, initial_delay, period
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
VDK::Semaphore::Semaphore( VDK::SemaphoreID inEnumValue, unsigned int inInitialValue, 
								unsigned int inMaxCount,
								VDK::Ticks inInitialDelay, VDK::Ticks inPeriod)
								: m_MaxCount(inMaxCount), m_Value(inInitialValue), 
								  m_Period(inPeriod)
{
	// The semaphore functions may log history while holding the semaphore lock,
	// so the semaphore lock must be of lower precedence than the history buffer
	// lock (which currently has precedence TMK_MAX_LOCK_PRECEDENCE == 255).
	//
	TMK_InitGlobalInterruptLock(&m_lock, TMK_MAX_LOCK_PRECEDENCE - 1);
	TMK_InitTimeElement(this, Timeout);
	TMK_InitDpcElement(this,Invoke);
	TMK_InitThreadList(&m_PendingThreads);

	m_InitialDelay = inInitialDelay;
	ID() = inEnumValue;
}

