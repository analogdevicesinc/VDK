/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "TMK.h"

#include "VDK_API.h"
#include "VDK_macros.h"
#include "VDK_Thread.h"
#pragma file_attr("System")


namespace VDK
{
	extern bool SetTimer();
	extern unsigned int g_ClockPeriod; // in clock cycles (core clock, except for TS201)
	extern float g_TickPeriod;         // in milliseconds
	extern float g_ClockFrequency;     // in kilohertz
	#ifdef __ADSPTS20x__
	extern BootFuncPointers g_pfTS20xSetTimer;
	#endif

	static inline void CalculateTimerParams() 
	{
	  #ifdef __ADSPTS20x__
	  // Fix for TAR19037. The timer is going at half the speed. This was 
	  // because VDK assumed that the timer decrements every CCLK cycle. It 
	  // doesn't: according to the HW Ref it decrements once every SOCCLK 
	  // cycle, where SOCCLK = CCLK / 2.

		g_ClockPeriod = (unsigned int)(g_ClockFrequency * g_TickPeriod / 2 );
	  #else
		g_ClockPeriod = (unsigned int)(g_ClockFrequency * g_TickPeriod);
	  #endif
        }

	void
	SetClockFrequencyKHz( float inFrequency )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		TMK_AcquireMasterKernelLock();
		g_ClockFrequency = inFrequency;
		CalculateTimerParams();
	// For TS201 we need to either call SetTimer or SetTimer0 so we have
	// a function pointer with the right function to call
	#ifdef __ADSPTS20x__
		(*g_pfTS20xSetTimer)();
	#else
	   	SetTimer();
	#endif
		TMK_ReleaseMasterKernelLock();
	}

	void
	SetClockFrequency( unsigned int inFrequency )
	{
		// This original API uses megahertz,
		// scale to kilohertz.
		//
		SetClockFrequencyKHz(inFrequency * 1000);
	}

	float
	GetClockFrequencyKHz(void)
	{
		return g_ClockFrequency;
	}

	unsigned int
	GetClockFrequency(void)
	{
		return (unsigned int)(g_ClockFrequency / 1000);
	}

	void
	SetTickPeriod( float inPeriod )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		TMK_AcquireMasterKernelLock();
		g_TickPeriod = inPeriod ;
		CalculateTimerParams();
	// For TS201 we need to either call SetTimer or SetTimer0 so we have
	// a function pointer with the right function to call
	#ifdef __ADSPTS20x__
		(*g_pfTS20xSetTimer)();
	#else
	   	SetTimer();
	#endif
		TMK_ReleaseMasterKernelLock();
	}

	float
	GetTickPeriod( void)
	{
		return g_TickPeriod;
	}

} // namespace VDK


