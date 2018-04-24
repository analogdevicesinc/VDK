// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The APIs used from the interrupt domain
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
#include <sys/anomaly_macros_rtl.h> // Used for BF561 05-00-0428 anomaly

#include "asm_offsets.h"
.file_attr ISR;
.file_attr OS_Component=Events;
.file_attr Events;

// Used for BF561 r0.4/0.5 branch anomaly 05-00-0428
// "Lost/Corrupted Write to L2 Memory Following Speculative 
// Read by Core B from L2 Memory."
#if WA_05000428
#define ANOM05000428 NOP;NOP;NOP;
#else
#define ANOM05000428 
#endif

.EXTERN TMK_InsertQueueDpc;

.extern _g_EventsObject__3VDK;

//////////////////////////////////////////////////////////////////////////////
/*
 *	The code for _VDK_ISR_SET_EVENTBIT(bit_num_)
 *    The parameter (event bit num) is passed in R4.
 */

.SECTION VDK_ISR_code;
.ALIGN 2;
    // Make the procedure global
    .global _VDK_ISR_SET_EVENTBIT_code__3VDK;
_VDK_ISR_SET_EVENTBIT_code__3VDK:
    .global _C_ISR_SetEventBit__3VDKF10EventBitID;
_C_ISR_SetEventBit__3VDKF10EventBitID:

    // Save some registers we're gonna trample
	[--SP] = ASTAT;
	[--SP] = (R7:5,P5:3);

#if (VDK_INSTRUMENTATION_LEVEL_ > 0)
	P5.L = _kNumEventBits__3VDK;
	P5.H = _kNumEventBits__3VDK;
	R5 = [P5];
	CC = R5 <= R0;
	IF !CC JUMP .validIDFoundinSet;
	// we are going to KernelPanic so we are not too bothered about
	// trashing registers
.invalidIDFoundinSet:
	R0.L =ENUM_kISRError_ & 0xFFFF;
	R0.H =(ENUM_kISRError_ >> 16) & 0xFFFF;
	R1.L =ENUM_kUnknownEventBit_ & 0xFFFF;
	R1.H =(ENUM_kUnknownEventBit_ >> 16) & 0xFFFF;
	R2 = R0; //this is the event bit ID including the count
	CALL.X _KernelPanic;
.validIDFoundinSet:
#endif

	// Logging the history if necessary
#if (VDK_INSTRUMENTATION_LEVEL_==2)

	// Get the event bit value
	// The low part of the word if the eventbit number we're setting,
	// and the HIGH BIT contains the OLD VALUE of the eventbit (the value
	// BEFORE the SetEventBit).
	R5 = R0;
	R6.L = 0x0000;
	R6.H = 0x8000;
	R6 = R6 | R5;
	IF !CC R5 = R6;

	// allow users to record the log
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kEventBitSet_, R5);


#endif /* VDK_INSTRUMENTATION_LEVEL_ */

	// Shift the bit into the correct eventbit position
	R7 = 1 (X);
	R7 <<= R0;

    // Do we need to do a recalculate?
	// Recalculate if (R7 & g_EventBitState) == 0;
    // Determine the event value to set
    P5.L = _g_EventBitState__3VDK;
    P5.H = _g_EventBitState__3VDK;
	CLI R5;		// disable interrupts
	R6 = [P5];
	R6 = R6 & R7;
	CC = AZ;	

	IF !CC JUMP .VDK_ISR_EVENTBIT_DONE;
    ANOM05000428
    
    // Set the bit
	// P5 == _g_EventBitState__3VDK;
	R6 = [P5];
	R6 = R7 | R6;
	[P5] = R6;

BitChange:
	[--SP] = RETS;  // save the call return address (early)

    // Say that a recalculation needs to be performed
	P5.L = _g_EventBitNeedsRecalculate__3VDK;
	P5.H = _g_EventBitNeedsRecalculate__3VDK;
	R6 = [P5];
	R6 = R7 | R6;
	[P5] = R6;

	R0.L = LO(_g_EventsObject__3VDK);
	R0.H = HI(_g_EventsObject__3VDK);
	// No conversion needed from EventsObject to DpcQueueElement.

	CALL.X TMK_InsertQueueDpc;

	RETS = [SP++];

.VDK_ISR_EVENTBIT_DONE:
	STI R5;				// re-enable interrupts
	(R7:5,P5:3) = [SP++];
	ASTAT = [SP++];
	RTS;

    
//////////////////////////////////////////////////////////////////////////////
    // make the procedure global
    .global _VDK_ISR_CLEAR_EVENTBIT_code__3VDK;
_VDK_ISR_CLEAR_EVENTBIT_code__3VDK:
    .global _C_ISR_ClearEventBit__3VDKF10EventBitID;
_C_ISR_ClearEventBit__3VDKF10EventBitID:
    // Save some registers we're gonna trample
	[--SP] = ASTAT;
	[--SP] = (R7:5,P5:3);

	// Log the history if necessary
#if (VDK_INSTRUMENTATION_LEVEL_ > 0)
	P5.L = _kNumEventBits__3VDK;
	P5.H = _kNumEventBits__3VDK;
	R5 = [P5];
	CC = R5 <= R0;
	IF !CC JUMP .validIDFoundinClear;
	// we are going to KernelPanic so we are not too bothered about
	// trashing registers
.invalidIDFoundinClear:
	R0.L =ENUM_kISRError_ & 0xFFFF;
	R0.H =(ENUM_kISRError_ >> 16) & 0xFFFF;
	R1.L =ENUM_kUnknownEventBit_ & 0xFFFF;
	R1.H =(ENUM_kUnknownEventBit_ >> 16) & 0xFFFF;
	R2 = R0; //this is the event bit ID including the count
	CALL.X _KernelPanic;
.validIDFoundinClear:
#endif

#if (VDK_INSTRUMENTATION_LEVEL_==2)

	// Get the event bit value
	// The low part of the word if the eventbit number we're setting,
	// and the HIGH BIT contains the OLD VALUE of the eventbit (the value
	// BEFORE the SetEventBit).
	R5 = R0;
	R6.L = 0x0000;
	R6.H = 0x8000;
	R6 = R6 | R5;
	IF !CC R5 = R6;
	
	// Log the event
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kEventBitCleared_, R5);

#endif /* VDK_INSTRUMENTATION_LEVEL_ */

	// Shift the bit into the correct eventbit position
	R7 = 1 (X);
	R7 <<= R0;

    // Do we need to do a recalculate?
	// Recalculate if (R7 & g_EventBitState) == 0;
    // Determine the event value to set
    P5.L = _g_EventBitState__3VDK;
    P5.H = _g_EventBitState__3VDK;
	CLI R5;		// disable interrupts
	R6 = [P5];
	R6 = R6 & R7;
	CC = AZ;	

	IF CC JUMP .VDK_ISR_EVENTBIT_DONE;
    ANOM05000428
    
    // Clear the bit
	// P5 == &_g_EventBitState__3VDK;
	R6 = [P5];
	R7 = ~R7;		// 1's complement of mask...
	R6 = R7 & R6;
	R7 = ~R7;		// ...and flip it back
	[P5] = R6;

	// Jump to common code (shared with SET_EVENTBIT).
	JUMP BitChange;
._C_ISR_SetEventBit__3VDKF10EventBitID.end:
._VDK_ISR_SET_EVENTBIT_code__3VDK.end:
