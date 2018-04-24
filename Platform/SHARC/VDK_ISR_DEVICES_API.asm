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
.file_attr OS_Component=DeviceDrivers;
.file_attr DeviceDrivers;


.EXTERN TMK_InsertQueueDpc;

/****************************************************************************/
.segment/dm        seg_dmda;

.extern _g_IOdevTable__3VDK;
.extern _ISR_save_I2__3VDK;
.extern _ISR_save_MODE1__3VDK;
.extern _ISR_save_ASTAT__3VDK;
.extern _KernelPanic;
.extern _kMaxNumIOObjects__3VDK;


#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_int_code_sw;
#else
.SECTION/PM seg_int_code;
#endif

.global _VDK_ACTIVATE_DEVICE_code__3VDK;

_VDK_ACTIVATE_DEVICE_code__3VDK:
	R1 = MODE1;

	// Ints off here, it's safe to save registers to the globals after this
    JUMP (PC, .SH_INT_DISABLED_1) (DB);
    BIT CLR MODE1 IRPTEN;
	DM(_ISR_save_MODE1__3VDK) = R1;
.SH_INT_DISABLED_1:

    DM(_ISR_save_ASTAT__3VDK) = ASTAT;
    DM(_ISR_save_I2__3VDK) = I2;

    // Log the event if necessary
#if VDK_INSTRUMENTATION_LEVEL_ ==2
    // Log the event
    VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kDeviceActivated_, R4);

#endif /* VDK_INSTRUMENTATION_LEVEL_ ==2 */

    // Point to the device
    R1 = 0x3FF; // kLowMask
    R4 = R1 and R4;  // This is the real element ID (count removed)
#if VDK_INSTRUMENTATION_LEVEL_ >0
    R1 = DM(_kMaxNumIOObjects__3VDK);
    R1 = R4 - R1 ;
    IF NOT AC JUMP (PC,.validIDFound);
.invalidIDFound:
    R12 = R4;
    R8 = ENUM_kBadIOID_;
    JUMP (PC,_KernelPanic) (DB);
    R4 = ENUM_kISRError_;
    DM(I7,M7) = PC;
#endif
.validIDFound:
    R1 = DM (_g_IOdevTable__3VDK + OFFSET_IDTable_m_IDTable_);
    R4 = R4 + R4;    // right offset from the beginning of the table
					 // (There are two words in every entry - count and index)
    R4 = R4 + R1;
	I2 = R4;
    R4 = DM (0,I2);  // Pointer to a Driver*

    R1 = POINTER_CONVERSION_DeviceDriver_TO_TMK_DpcElement_;
    R4 = R4 + R1;

	R1 = PCSTK;
	POP PCSTK;
	CALL TMK_InsertQueueDpc;
	PUSH PCSTK;
	PCSTK = R1;

    // Restore trampled variables.
    I2 = DM(_ISR_save_I2__3VDK);		// finished with I2

    // We're done -- restore the variables trampled in the ISR
    RTS(DB);
	MODE1 = DM(_ISR_save_MODE1__3VDK);  // ints may become live after this
    ASTAT = DM(_ISR_save_ASTAT__3VDK);
._VDK_ACTIVATE_DEVICE_code__3VDK.end:
.endseg;
