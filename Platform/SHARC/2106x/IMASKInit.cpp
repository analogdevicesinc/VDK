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
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Device-specific initialization of IMASK. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <platform_include.h>

#pragma file_attr("Startup")

namespace VDK
{
/* the code needs to be in internal memory because we are modifying IRPTL
   and otherwise we could hit anomaly 14000022 if the code goes in external
   memory. */
#pragma section("seg_int_code", DOUBLEANY)
	bool 
	InitISRMask()
	{
		asm("IMASKP=0;");		// no nested interrupts pending
		asm("IRPTL=0;");		// no interrupts pending
		return true;
	}

}  // namespace VDK


