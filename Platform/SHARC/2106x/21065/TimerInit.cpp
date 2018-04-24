/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Timer initialization
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Device-specific initialization of the timer. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <def21065L.h>
#pragma file_attr("System")
#pragma file_attr("Startup")

namespace VDK
{
	extern unsigned int g_ClockPeriod;
	bool
	SetTimer(){
		asm("#include <def21065L.h>");
		// timer_off
		asm("BIT CLR MODE2 TIMEN0;");
		// Turn the clock on ..

        asm("DM(TPERIOD0)=%0;" : :"d" (g_ClockPeriod) );
		asm("R0=0;DM(TCOUNT0)=R0;" : : : "R0");
		asm("R0=1;DM(TPWIDTH0)=R0;": : : "R0");

		// Turn timer0 on in PWMOUT mode and set it to use the TMZHI interrupt
		asm("BIT SET MODE2 TIMEN0 | PWMOUT0| INT_HI0;");
		return true;

	}

}  // namespace VDK


