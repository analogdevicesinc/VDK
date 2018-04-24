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
.file_attr OS_Component=Semaphores;
.file_attr Semaphores;

.EXTERN TMK_InsertQueueDpc;

/****************************************************************************/
.segment/dm        internal_memory_data;

.extern _g_SemaphoreTable__3VDK;
.extern _ISR_save_I2__3VDK;
.extern _ISR_save_ASTAT__3VDK;
.extern _ISR_save_MODE1__3VDK;
.extern _kMaxNumActiveSemaphores__3VDK;
.extern _KernelPanic;
.endseg;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_int_code_sw;
#else
.SECTION/PM seg_int_code;
#endif

#ifdef __SIMDSHARC__
.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases
#endif

.global _ISR_POST_SEMAPHORE_code__3VDK;

/**
 * Post a semaphore
 * R4 = SemaphoreID
 * I2 = pointer to a semaphore
 * I8 = location to JUMP to on return
 */
_ISR_POST_SEMAPHORE_code__3VDK:
	R1 = MODE1;

	// Ints off here, it's safe to save registers to the globals after this
    JUMP (PC, .SH_INT_DISABLED_1) (DB);
    BIT CLR MODE1 IRPTEN;
	DM(_ISR_save_MODE1__3VDK) = R1;
.SH_INT_DISABLED_1:

    DM(_ISR_save_I2__3VDK) = I2;
    DM(_ISR_save_ASTAT__3VDK) = ASTAT;

    // Log the Semaphore's posting in the history buffer -- if necessary!
#if VDK_INSTRUMENTATION_LEVEL_ ==2
    // Log the event, note R4 = SemaphoreID
	VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kSemaphorePosted_, R4);
	
#endif /* VDK_INSTRUMENTATION_LEVEL_ ==2 */

    // Point to the semaphore
    R1 = 0x3FF; // kLowMask
    R4 = R1 and R4;  // This is the real element ID (count removed)
#if VDK_INSTRUMENTATION_LEVEL_ >0 
    R1 = DM(_kMaxNumActiveSemaphores__3VDK);
    R1 = R4 - R1 ;
    IF NOT AC JUMP (PC,.validIDFound);
.invalidIDFound:
    R12 = R4;
    R8 = ENUM_kUnknownSemaphore_;
    JUMP (PC,_KernelPanic) (DB);
    R4 = ENUM_kISRError_;
    DM(I7,M7) = PC;
.validIDFound:
#endif
    R1 = DM (_g_SemaphoreTable__3VDK + OFFSET_IDTable_m_IDTable_); // g_SemaphoreTable.m_IDTable
    R4 = R4 + R4;    // right offset from the beginning of the table
		     // (There are two words in every entry - count and index)
	R1 = R4 + R1;
	I2 = R1;
    R4 = DM (0, I2);  // Pointer to a Semaphore*

    //  R4 = &(g_Semaphores[inID])

    // Get the address of semaphore->m_PendingThreads
    //      R4 = &(semaphore->m_PendingThreads)
    R1 = OFFSET_Semaphore_m_PendingThreads_;
    R4 = R4 + R1, I2 = R4;    //  I2 = &(g_Semaphores[inID])
	
    // Get the tle that would be receiving the semaphore
    R1 = DM(OFFSET_Semaphore_m_PendingThreads_pHead_, I2);

    // See if the semaphore has any threads
    R4 = R4 - R1;
    IF NE JUMP (PC, .threadsPresent)(DB);
    R4 = DM(OFFSET_Semaphore_m_Value_, I2);	// increment the semaphore's value
    R4 = R4 + 1;
    
    // Clamp the semaphore's value to the MaxCount and store it
    R1 = DM(OFFSET_Semaphore_m_MaxCount_,I2);
    R1 = R4 - R1;  // m_Value - m_MaxCount

    IF AC R4 = DM(OFFSET_Semaphore_m_MaxCount_,I2);   
    JUMP (PC, .done) (DB);
    DM(OFFSET_Semaphore_m_Value_, I2) = R4;	 // store the semaphore's new value
	NOP;

.threadsPresent:
    //
    // If there are pending threads then we don't clamp the value,
    // in case the number of pending threads is greater than m_MaxCount.
    // The max count will be applied in Semaphore::Invoke().
    DM(OFFSET_Semaphore_m_Value_, I2) = R4;	 // store the semaphore's new value

    //
    // Check whether the semaphore is already on the DPC queue
    //
    R4 = I2;    // I2 == semaphore
    R1 = POINTER_CONVERSION_Semaphore_TO_TMK_DpcElement_;
    R4 = R4 + R1;

    //
    // Append the semaphore to the DPC queue
	// I2 = R4 = semaphore as DpcQueueElement
    //
	R1 = PCSTK;
	POP PCSTK;
	CALL TMK_InsertQueueDpc;
	PUSH PCSTK;
	PCSTK = R1;

.done:
    // Restore trampled variables.
    I2 = DM(_ISR_save_I2__3VDK);		// finished with I2

    // We're done -- restore the variables trampled in the ISR
    // The MODE register is restored in the inline macro code
    RTS(DB);
	MODE1 = DM(_ISR_save_MODE1__3VDK);  // ints may become live after this
    ASTAT = DM(_ISR_save_ASTAT__3VDK);
._ISR_POST_SEMAPHORE_code__3VDK.end:
.endseg;



