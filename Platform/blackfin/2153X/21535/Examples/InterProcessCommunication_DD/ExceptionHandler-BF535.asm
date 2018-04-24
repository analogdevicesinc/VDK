/* =============================================================================
 *
 *   Description: This is the exception handler.
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 *  Copyright (c) 2002, Analog Devices, Inc.,  All rights reserved.
 *
 *  This software is the property of Analog Devices, Inc.
 *
 * ===========================================================================*/

#include "VDK.h"

.section data1;
/* declare external globals */

/* declare locals */

.section program;

/******************************************************************************
 * UserExceptionHandler
 */

/* User Exception Handler entry point */
.GLOBAL     UserExceptionHandler;
UserExceptionHandler:

	/**
	 * Handle the exception...
	 *
	 * Remember that the VDK reserves the user exception 0, which
	 * is accessed with the following instruction:
	 *
	 *	EXCPT 0;
	 *
	 * Any other exception (user or system), is your responsibility to handle
	 * here.
	 */

	RTX;

.UserExceptionHandler.end:

/* ========================================================================== */
