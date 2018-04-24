// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Semaphore APIs used from the interrupt domain
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The Semaphore APIs used from the interrupt domain (for Blackfin)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include <VDK_ISR_API.h>

#include <sys/platform.h>
#include <sys/anomaly_macros_rtl.h> // Used for BF561 05-00-0428 anomaly

#include "asm_offsets.h"
.file_attr ISR;
.file_attr OS_Component=Semaphores;
.file_attr Semaphores;

// Used for BF561 r0.4/0.5 branch anomaly 05-00-0428
// "Lost/Corrupted Write to L2 Memory Following Speculative 
// Read by Core B from L2 Memory."
#if WA_05000428
#define ANOM05000428 NOP;NOP;NOP;
#else
#define ANOM05000428 
#endif

.SECTION     data1;
	.align 4;
	.global _g_SemaphoreTable__3VDK;
	.type _g_SemaphoreTable__3VDK,STT_OBJECT;
	.global _g_TimeQueue__3VDK;
	.type _g_TimeQueue__3VDK,STT_OBJECT;
	.global _VDK_ISR_POST_SEMAPHORE_code__3VDK;
	.global _C_ISR_PostSemaphore__3VDKF11SemaphoreID;
	.global _kMaxNumActiveSemaphores__3VDK;
	.type _kMaxNumActiveSemaphores__3VDK,STT_OBJECT;

.EXTERN TMK_InsertQueueDpc;

.SECTION VDK_ISR_code;
	.align 2;
_VDK_ISR_POST_SEMAPHORE_code__3VDK:
_C_ISR_PostSemaphore__3VDKF11SemaphoreID:

		// save some temps on the stack (R0 is already saved by macro)
		[--SP] = ASTAT;
		[--SP] = (R7:5,P5:3);

		// Perform the instrumentation
#if (VDK_INSTRUMENTATION_LEVEL_==2)

		// Setup the value
		// The high bit is set if the old value of the semaphore was true
		R6 = R0;
		R5 = CC;
		R5 = R5 << 31;
		R5 = R5 | R6;
		IF CC R6 = R5;

		// Log the history
		VDK_ISR_INTERNAL_HISTORY_LOG_(ENUM_VDK_kSemaphorePosted_, R6);
#endif

		P4.L = _g_SemaphoreTable__3VDK;
		P4.H = _g_SemaphoreTable__3VDK;
		// Upon entry, R0 contains the SemaphoreID to post
        P3 = [P4+ OFFSET_IDTable_m_IDTable_]; // g_SemaphoreTable.m_IDTable
		R5 = 0x3FF;      // kLowMask
		R6 = R5 & R0;   // This is the real element ID (count removed)
#if (VDK_INSTRUMENTATION_LEVEL_ > 0)
		P5.L = _kMaxNumActiveSemaphores__3VDK;
		P5.H = _kMaxNumActiveSemaphores__3VDK;
		R5 = [P5];
		CC = R5 <= R6;
		IF !CC JUMP .validIDFound;
		// we are going to KernelPanic so we are not too bothered about
		// trashing registers
.invalidIDFound:
		R0.L =ENUM_kISRError_ & 0xFFFF;
		R0.H =(ENUM_kISRError_ >> 16) & 0xFFFF;
		R1.L =ENUM_kUnknownSemaphore_ & 0xFFFF;
		R1.H =(ENUM_kUnknownSemaphore_ >> 16) & 0xFFFF;
		R2 = R0; //this is the semaphore ID including the count
		CALL.X _KernelPanic;
#endif
.validIDFound:
        R6 =  R6 <<  2; // right offset from the beginning of the table
        P5 = R6 ;
        P3 = P3 + (P5<<1);
		P5 = [P3 + OFFSET_IDTableElement_m_ObjectPtr_];    // Pointer to a Semaphore*

		
		// P5 = semaphore = ptr to the semaphore object we are interested in

		CLI R5;			// disable interrupts

		// P3 --> The pending thread (if there is one)
		P3 = [P5 + OFFSET_Semaphore_m_PendingThreads_];

		// R7 = address of first element in the semaphore's pending thread list
		R7 = P5;
		R7 += OFFSET_Semaphore_m_PendingThreads_pHead_;

		// CC is set if there are no threads pending on this semaphore
		//   i.e. semaphore's TLE points to itself
		R6 = P3 ;
		CC = R6 == R7;

		// Increment the semaphore's value, but don't store it yet
		R6 = [P5 + OFFSET_Semaphore_m_Value_] ; 
		R6 += 1;

		// If there are no threads pending on this semaphore
		IF !CC JUMP .threadsPresent;	   // threads pending
        ANOM05000428
        
		// Clamp the semaphore's value to the MaxCount and store it
		R7 = [P5 + OFFSET_Semaphore_m_MaxCount_] ;
		CC = R7 < R6(IU);
		IF CC R6 = R7;
		[P5 + OFFSET_Semaphore_m_Value_] = R6;	 // store the semaphore's new value

		JUMP .done;  // then we can restore trampled temps & return

		// Otherwise we have to punt the semaphore onto the Dpc queue:
.threadsPresent:
		[--SP] = RETS;  // save the call return address

		//
		// If there are pending threads then we don't clamp the value,
		// in case the number of pending threads is greater than m_MaxCount.
		// The max count will be applied in Semaphore::Invoke().
		[P5 + OFFSET_Semaphore_m_Value_] = R6;	 // store the semaphore's new value

		// P5 == semaphore
		R0 = P5;
		R0 += POINTER_CONVERSION_Semaphore_TO_TMK_DpcElement_;
		CALL.X TMK_InsertQueueDpc;

		RETS = [SP++];
.done:
		STI R5;				// re-enable interrupts

		(R7:5,P5:3) = [SP++];
		ASTAT = [SP++];
		RTS;
._C_ISR_PostSemaphore__3VDKF11SemaphoreID.end:
._VDK_ISR_POST_SEMAPHORE_code__3VDK.end:



