// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for source code formatting
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include <VDK_ISR_API.h>
#include "ADI_attributes.h"
#include "asm_offsets.h"
.file_attr ISR;
.file_attr OS_Component=Events;
.file_attr Events;

.EXTERN TMK_InsertQueueDpc;

.extern _g_EventsObject__3VDK;
.extern _g_EventBitState__3VDK;
.extern _ISR_save_I2__3VDK;
.extern _ISR_save_ASTAT__3VDK;
.extern _ISR_save_MODE1__3VDK;
.extern _KernelPanic;
.extern _kNumEventBits__3VDK;


#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_int_code_sw;
#else
.SECTION/PM seg_int_code;
#endif

#ifdef __SIMDSHARC__
.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases
#endif


.global     _ISR_SET_EVENTBIT_code__3VDK;
.global     _ISR_CLEAR_EVENTBIT_code__3VDK;
.extern     _g_EventBitNeedsRecalculate__3VDK;


_ISR_SET_EVENTBIT_code__3VDK:
	R1 = MODE1;

	// Ints off here, it's safe to save registers to the globals after this
    JUMP (PC, .SH_INT_DISABLED_1) (DB);
    BIT CLR MODE1 IRPTEN;
	DM(_ISR_save_MODE1__3VDK) = R1;
.SH_INT_DISABLED_1:

    DM(_ISR_save_ASTAT__3VDK) = ASTAT;
    DM(_ISR_save_I2__3VDK) = I2;


    // On entry, here's what we've got:
    // R4 = eventbit_num;
#if VDK_INSTRUMENTATION_LEVEL_ >0
    R1 = DM(_kNumEventBits__3VDK);
    R1 = R4 - R1 ;
    IF NOT AC JUMP (PC,.validIDFoundinSet);
.invalidIDFoundinSet:
    R12 = R4;
    R8 = ENUM_kUnknownEventBit_;
    JUMP (PC,_KernelPanic) (DB);
    R4 = ENUM_kISRError_;
    DM(I7,M7) = PC;
.validIDFoundinSet:
#endif


	R1 = DM(_g_EventBitState__3VDK);
	
#if VDK_INSTRUMENTATION_LEVEL_ == 2

    // Determine the event value to set
    BTST R1 BY R4;
    IF NOT sz R4 = BSET R4 by 31;

    // Log the event, note R4 = EventBitNum
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kEventBitSet_, R4);
	
    // Restore some variables
    R4 = BCLR R4 by 31;
#endif /* VDK_INSTRUMENTATION_LEVEL_ == 2 */

    R1 = DM(_g_EventBitState__3VDK);

    // Do we need to set the event?
    // Is the eventbit_num'th bit of g_EventBitState clear?
    BTST R1 BY R4;		
    IF NOT sz JUMP .done;		// if not then bail

    // We need to set the event bit

    // Set the eventbit_num'th bit of g_EventBitState
    R1 = BSET R1 BY R4;		
    DM(_g_EventBitState__3VDK) = R1;

.BitChange:
    // Set eventbit_num'th bit of _g_EventBitNeedsRecalculate
    R1 = DM(_g_EventBitNeedsRecalculate__3VDK);
    R1 = BSET R1 BY R4;	
    DM(_g_EventBitNeedsRecalculate__3VDK) = R1;

	// No conversion needed from EventsObject to DpcQueueElement.
	//
    R4 = _g_EventsObject__3VDK;

	R1 = PCSTK;
	POP PCSTK;
	CALL TMK_InsertQueueDpc;
	PUSH PCSTK;
	PCSTK = R1;

.done:
    // Restore trampled variables.
    I2 = DM(_ISR_save_I2__3VDK);		// finished with I2

    // We're done -- restore the variables trampled in the ISR
    RTS(DB);
	MODE1 = DM(_ISR_save_MODE1__3VDK);  // ints may become live after this
    ASTAT = DM(_ISR_save_ASTAT__3VDK);


_ISR_CLEAR_EVENTBIT_code__3VDK:
    NOP;
	R1 = MODE1;

	// Ints off here, it's safe to save registers to the globals after this
    JUMP (PC, .SH_INT_DISABLED_2) (DB);
    BIT CLR MODE1 IRPTEN;
	DM(_ISR_save_MODE1__3VDK) = R1;
.SH_INT_DISABLED_2:

    DM(_ISR_save_ASTAT__3VDK) = ASTAT;
    DM(_ISR_save_I2__3VDK) = I2;

    // On entry, here's what we've got:
    // R4 = eventbit_num;

#if VDK_INSTRUMENTATION_LEVEL_ >0
    R1 = DM(_kNumEventBits__3VDK);
    R1 = R4 - R1 ;
    IF NOT AC JUMP (PC,.validIDFound);
.invalidIDFound:
    R12 = R4;
    R8 = ENUM_kUnknownEventBit_;
    JUMP (PC,_KernelPanic) (DB);
    R4 = ENUM_kISRError_;
    DM(I7,M7) = PC;
.validIDFound:
#endif

	R1 = DM(_g_EventBitState__3VDK);

#if VDK_INSTRUMENTATION_LEVEL_ == 2

    // Determine the event value to set
    BTST R1 BY R4;
    IF NOT sz R4 = BSET R4 by 31;

    // Log the event, note R4 = eventBitNum
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kEventBitCleared_, R4);

    // Restore some variables
    R4 = BCLR R4 by 31;
#endif /* VDK_INSTRUMENTATION_LEVEL_ == 2 */

    // Do we need to clear the event?
    // Is the eventbit_num'th bit of g_EventBitState set?
    BTST R1 BY R4;		
    IF sz JUMP .done;		// if not then bail

    // We need to set the event bit

    // Clear the eventbit_num'th bit of g_EventBitState and
	// jump to common code (shared with SET_EVENTBIT).
	JUMP (PC, .BitChange)(DB);
    R1 = BCLR R1 BY R4;		
    DM(_g_EventBitState__3VDK) = R1;

.endseg;
