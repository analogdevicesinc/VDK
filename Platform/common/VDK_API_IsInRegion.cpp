/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Returns whether we are in a region or not. Currently
 *                only used by OSAL, and is an undocumented 5.0 API.
 *
 *   Last modified $$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#pragma file_attr("FuncName=IsInRegion")
#pragma file_attr("FuncName=_IsInRegion__3VDKFv")
#pragma file_attr("RTL_support")

#include "TMK.h"
#include "VDK_API.h"
#include "vdk_macros.h"

extern "C" bool vdkMainMarker;

namespace VDK{

/* Undocumented API for VisualDSP 5.0, only available for Blackfin or SHARC */

#if defined(__ADSPBLACKFIN__) || defined(__ADSP21000__)

#pragma regs_clobbered ___vdkclobber_call__
  bool IsInRegion(void)
  {
    /* if this API is called at ISR or kernel level we will not be changing
       thread so we return true.  vdkMainMarker = false if we have not yet
       reached the end of startup, so there cannot be a context switch. */ 
	if (IS_ISR_LEVEL () || !vdkMainMarker)
      return true;

    /* if we are not at ISR level return the TMK information */
	return TMK_IsSchedulingSuspended();
  }

#endif /* __ADSPBLACKFIN__ ||__ADSP21000__ */
}
