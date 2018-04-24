// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The APIs related to devices used from the interrupt domain
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The APIs used from the interrupt domain (for Blackfin)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include <VDK_ISR_API.h>
#include <sys/platform.h>

#include "asm_offsets.h"
.file_attr ISR;
.file_attr OS_Component=DeviceDrivers;
.file_attr DeviceDrivers;

.EXTERN TMK_InsertQueueDpc;

.SECTION     data1;
.extern _g_IOdevTable__3VDK;
.extern _g_DpcQueue__3VDK;

//////////////////////////////////////////////////////////////////////////////
/*
 *	The code for VDK_ISR_ACTIVATE_DEVICE_(dev_num_)
 *    The parameter (device num) is passed in R0.
 */

.SECTION VDK_ISR_code;
.ALIGN 2;
.global _VDK_ISR_ACTIVATE_DEVICE_code__3VDK;
_VDK_ISR_ACTIVATE_DEVICE_code__3VDK:
.global _C_ISR_ActivateDevice__3VDKF4IOID;
_C_ISR_ActivateDevice__3VDKF4IOID:

    // Save some registers we're gonna trample
	[--SP] = ASTAT;
	[--SP] = (R7:5,P5:3);
	[--SP] = RETS;  // save the call return address


#if (VDK_INSTRUMENTATION_LEVEL_==2)
		
	// Log the event
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kDeviceActivated_, R0);

#endif /* VDK_INSTRUMENTATION_LEVEL_ */

	P4.L = _g_IOdevTable__3VDK;
	P4.H = _g_IOdevTable__3VDK;
	// Upon entry, R0 contains the IOid to activate
    P3 = [P4 + OFFSET_IDTable_m_IDTable_]; // g_IOdevTable.m_IDTable
	R5 = 0x3FF;      // kLowMask
	R6 = R5 & R0;   // This is the real element ID (count removed)
#if (VDK_INSTRUMENTATION_LEVEL_ > 0)
		P5.L = _kMaxNumIOObjects__3VDK;
		P5.H = _kMaxNumIOObjects__3VDK;
		R5 = [P5];
		CC = R5 <= R6;
		IF !CC JUMP .validIDFound;
		// we are going to KernelPanic so we are not too bothered about
		// trashing registers
.invalidIDFound:
		R0.L =ENUM_kISRError_ & 0xFFFF;
		R0.H =(ENUM_kISRError_ >> 16) & 0xFFFF;
		R1.L =ENUM_kBadIOID_ & 0xFFFF;
		R1.H =(ENUM_kBadIOID_ >> 16) & 0xFFFF;
		R2 = R0; //this is the semaphore ID including the count
		CALL.X _KernelPanic;
#endif
.validIDFound:
    R6 =  R6 <<  2; // right offset from the beginning of the table
    P5 = R6 ;
    P3 = P3 + (P5<<1);
	P5 = [P3 + OFFSET_IDTableElement_m_ObjectPtr_];    // Pointer to a DeviceDriver

	// P5 == driver
	R0 = P5;
	R0 += POINTER_CONVERSION_DeviceDriver_TO_TMK_DpcElement_;
	CALL.X TMK_InsertQueueDpc;
	

	// Restore the registers
	RETS = [SP++];
	(R7:5,P5:3) = [SP++];
	ASTAT = [SP++];

	// ...and exit
	RTS;
._C_ISR_ActivateDevice__3VDKF4IOID.end:
._VDK_ISR_ACTIVATE_DEVICE_code__3VDK.end:


