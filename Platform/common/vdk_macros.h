/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Defines macros used during a self test build
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Provides self-consistency checking and trace capability for
 *  debug builds.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

// BEGIN_INTERNALS_STRIP

#ifndef VDK_MACROS_H_
#define VDK_MACROS_H_

#include <stdio.h>
#include <assert.h>

#include <VDK_AsmMacros.h>

#if (VDK_INSTRUMENTATION_LEVEL_>0)
#define ERRCHK(x) (x)
#define NERRCHK(x) (x)
#else
#define ERRCHK(x) (true)
#define NERRCHK(x) (false)
#endif



#endif /* VDK_MACROS_H_ */

// END_INTERNALS_STRIP



