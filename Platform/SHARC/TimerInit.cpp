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

#include <platform_include.h>
#pragma file_attr("System")
#pragma file_attr("Startup")

namespace VDK
{
	extern unsigned int g_ClockPeriod;

	bool
	SetTimer() {
		asm("#include <platform_include.h>");
        // Turn the clock off ..
		asm("BIT CLR MODE2 TIMEN;");
		// timer_set(g_ClockPeriod, g_ClockPeriod);
		asm("TPERIOD=%0;\n"
			"TCOUNT=%1;\n" : :"d" (g_ClockPeriod), "d" (g_ClockPeriod) );
        // Turn the clock on ..
		asm("BIT SET MODE2 TIMEN;");
		return true;
	}
}  // namespace VDK


