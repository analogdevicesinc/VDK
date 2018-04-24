/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Returns whether VDK::Run has been called or not. Currently 
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

#pragma file_attr("FuncName=IsRunning")
#pragma file_attr("FuncName=_IsRunning__3VDKFv")
#pragma file_attr("RTL_support")

#include "VDK_API.h"
#include "vdk_macros.h"

extern "C" bool vdkMainMarker;

namespace VDK{

/* Undocumented API for VisualDSP 5.0 */
/* This API can be called at startup and at any level */

  bool IsRunning(void)
  {
	  return vdkMainMarker;
  }

}
