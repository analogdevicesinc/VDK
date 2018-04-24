/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Stub support functions for Deferred Callback MAnager
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Context switching code.  This is specific to each processor
 * -----------------------------------------------------------------------------
 *  Copyright (c) 2001-2018 Analog Devices, Inc.,  All rights reserved
 *
 * ===========================================================================*/

#include <sys/platform.h>

#include "asm_offsets.h"
.file_attr OS_Internals;

/*******************************************************************
*
* This module contains C-callable functions to glue the Deferred
* Callback Manager (part of Blackfin System Services) onto VDK's
* DPC queue.
*
**********************************************************************/

.SECTION VDK_ISR_code;
.align 2;

.global _adi_dcb_RegisterISR;
.global _adi_dcb_Forward;


_adi_dcb_RegisterISR:
		//
		// We don't do anything here, as none of the DCB manager initialization
		// is relevant to the VDK environment. This function will be called by
		// the DCB Manager, but returns without doing anything.
		//
		NOP;
		NOP;
		NOP;
		NOP;
		RTS;
._adi_dcb_RegisterISR.end:

_adi_dcb_Forward:
		//
		// adi_dcb_Forward() takes three arguments:
		//
		//   Name     Type               Register
		//   ----     ----               --------
		//   Entry    ADI_DCB_ENTRY_HDR* R0
		//   IvgLevel interrupt_kind     R1
		//   Priority u16                R2
		//
		// VDK ignores all except the first argument, the pointer
		// to the DPC object to be queued. ADI_DCB_ENTRY_HDR has the
		// same physical layout as VDK::DpcQueueElement.
		//
		// This function is called from C/C++ only. This means that we can
		// work in the scratch registers so we don't have to save anything.
		//

		// If the passed-in object is already on the DPC queue (i.e. its
		// chaining pointer isn't NULL) then it is already on the DPC queue
		// and we do nothing.
		//
		// This function has now been replaced by a microkernel func
		// _TMK_InsertQueueDpc so we simply JUMP to it.
		JUMP.X _TMK_InsertQueueDpc;
._adi_dcb_Forward.end:
