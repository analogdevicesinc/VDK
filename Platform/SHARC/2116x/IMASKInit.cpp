/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: IMASK initialization
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Device-specific initialization of IMASK. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <platform_include.h>
#include <sysreg.h>
#include <VDK_API.h>

#pragma file_attr("Startup")

namespace VDK
{
	extern unsigned int g_InitialISRMask2;

	bool 
	InitISRMask()
	{
	// the only bits from LIRPTL that refer to interrupt masks are
	// 21,20,19,18,17,16 so we ignore all the others
	unsigned int realLIRPTL=__builtin_sysreg_read(sysreg_LIRPTL) & 0x3F0000;
		g_InitialISRMask2 |= realLIRPTL;
		asm("IMASKP=0;");		// no nested interrupts pending
		asm("IRPTL=0;");		// no interrupts pending

		// The initial value of g_InitialISRMask2 will have been constructed using
		// the interrupt latch bit names (e.g. LP0I) rather than the mask bit names (e.g. LP0MSK).
		// We therefore have to shift the bits into the mask positions.
		//
		g_InitialISRMask2 = (g_InitialISRMask2 & 0xFF) << 16;

		if (0 != g_InitialISRMask2)
			SetInterruptMaskBits(LPISUMI);

		// It is safe to update LIRPTL here as interrupts
		// will not have been globally enabled at this point.
		//
		asm("LIRPTL=%0;" : : "d" (g_InitialISRMask2) );  // set extended mask

		return true;
	}

}  // namespace VDK


