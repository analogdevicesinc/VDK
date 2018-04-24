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

#include <vdk_asmmacros.h>
#pragma file_attr("System")
#pragma file_attr("Startup")

namespace VDK
{
    extern unsigned int 	g_ClockPeriod;
	bool                    SetTimer();
}

/////////////////////////////////////////////////////////////////////
//  Activate timer , enable it and put timer in autoreload
//  mode so that the timer counter will be reloaded with the
//  timer period when it reached zero.


bool VDK::SetTimer(){
	asm("#define TIMER_EXIT_LOW_POWER_MODE 	0x00000001");
	asm("#define TIMER_ENABLE				0x00000002");
	asm("#define TIMER_AUTO_RELOAD_TCOUNT	0x00000004");
	asm("#define TIMER_RUN_SETTING			( TIMER_EXIT_LOW_POWER_MODE | TIMER_ENABLE | TIMER_AUTO_RELOAD_TCOUNT )");
	asm(".extern  _g_ClockPeriod__3VDK;");

    // stop the timer 
       asm("P0.L = 0x3000;" 
       "P0.H = 0xFFE0;" 
       "R1 = 0x00000001| 0x00000004;"           
           "[P0] = R1;" 
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
       "CLI R1;" 
#endif
       "CSYNC;"  
#if defined (WA_05000312)
        "STI R1;"
#endif
		: : :  "R1", "P0");   

	asm(
        //
        //      Bring the chip out of low-power mode by setting bit zeroof the
        //      timer control word.  This must be done BEFORE enabling the timer.
        //
        "P0.H = 0xFFE0;"
                                              // Timer Control Register is
        "P0.L = 0x3000;"                      //   memory mapped here...
        "R1 = TIMER_EXIT_LOW_POWER_MODE;"     // Must be out of low power mode 
        "[P0] = R1;"                          // BEFORE  enabling the timer
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
		"CLI R1;"
#endif
        "CSYNC;"
#if defined (WA_05000312)
		"STI R1;"
#endif

		//
		// Set the timer period	
		// 0xFFE03004 is the location of the TPERIOD register
		//
		"P1.H = _g_ClockPeriod__3VDK;"	// Read the user's period value
		"P1.L = _g_ClockPeriod__3VDK;"
		"R1 = [P1];"			
		"P0.H = 0xFFE0;"           // TPERIOD is memory mapped at 0xFFE03004
		"P0.L = 0x3004;"
		"[P0] = R1;"               // Set the clock period to the user's value
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
		"CLI R1;"
#endif
		"CSYNC;"
#if defined (WA_05000312)
		"STI R1;"
#endif

		//
		// Set the initial counter value so the timer is triggered after one period
		// 0xFFE0300C is the location of the TCOUNT register
		//
		"R1 = [P1];"                // P1 still has the Clock Period
		"P0.H = 0xFFE0;"			// TCOUNT is memory mapped at 0xFFE0300C
		"P0.L = 0x300C;"
		"[P0] = R1;"
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
		"CLI R1;"
#endif
		"CSYNC;"
#if defined (WA_05000312)
		"STI R1;"
#endif

		//
		// Set the pre scale register to 0
		// 0xFFE03008 is the location of the TSCALE register
		//
	    "P0.H = 0xFFE0;"			// TSCALE is memory mapped at 0xFFE03008
	    "P0.L = 0x3008;"      
		"R1 = 0;" 					// Mask except for lower 8 bits
		"[P0] = R1;"  				// write the value back
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
		"CLI R1;"
#endif
	    "CSYNC;"
#if defined (WA_05000312)
		"STI R1;"
#endif
			: : : "P0", "P1", "R1" );

		//
		//	Now that all of the initial values are set, turn the timer on and set it to 
		//	auto reload by setting bits 1 and 2. 0xFFE03000 is the location of the TCNTL register.
		//

// Turn the timer on and set it to auto reload by setting bits 1 and 2.
// 0xFFE03000 is the location of the TCNTL register.
        asm("P0.L = 0x3000;" 
	    "P0.H = 0xFFE0;" 
	    "R1 = 0x00000001| 0x00000002 | 0x00000004;"           
        "[P0] = R1;" 
#if defined (WA_05000312)
// workaround for anomaly 05000312
// Errors when SSYNC, CSYNC, or Loads to LT, LB and LC Registers
// Are Interrupted
        "CLI R1;" 
#endif
	    "CSYNC;" 
#if defined (WA_05000312)
        "STI R1;" 
#endif
		: : : "R1", "P0");    
	return true;
}



