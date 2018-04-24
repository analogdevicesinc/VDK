/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *  $RCSfile$
 *
 *  Description: The template for the APIs exposed to the user
 *
 *  Last modified $Date$
 *	Last modified by $Author$
 *  $Revision$
 *  $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Description: Main RTOS header file.
 *  The makefile creates a header file for the end user that encompasses all
 *	public functions and definitions.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#include "VDK_CPP_Names.h"

/**
 * Include the APIs.  When including the API we don't
 * want to define the types defined by the user in the
 * IDDE...  Incidentally, the types defined by the user
 * in the IDDE, are defined in vdk.h. To accomplish this, 
 * VDK_API.h checks to see if vdk.h was included.
 */
#ifdef __ECC__
#include <VDK_API.h>
#include <asm_offsets.h>
#else
#include <VDK_API_asm.h>
#endif
// the ISR part has stuff for C ISRs now so we allow it to be in all the time.
// The file itself will have a #if ECC part
#include <VDK_ISR_API.h>

/**
 * Include VDK_Thread.h if we are a C++ compilation
 */
#ifdef __cplusplus
#include <VDK_Thread.h>
#include <vdk_driver.h>
#include <VDK_Message.h>
#endif /* __cplusplus */



