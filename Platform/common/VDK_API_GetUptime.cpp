/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <TMK.h>
// This API is only provided for compatibility of libraries built with 
// previous releases of VDSP. 
// Files compiled with VDSP++ 4.5 will not have any references to this API 
// but to TMK_GetUptime

namespace VDK {

  unsigned int GetUptime (void)
  {
 	return TMK_GetUptime();
  }
}
