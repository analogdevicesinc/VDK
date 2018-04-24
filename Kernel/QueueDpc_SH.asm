// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************


#include "Trinity.h"
.file_attr ISR;
.file_attr OS_Internals;

#if defined(NUM_CORES) && (NUM_CORES > 1)
#error No SMP spinlock support in SHARC
#endif

.IMPORT "Trinity.h";

#include <platform_include.h>

#include <asm_sprt.h>

// The assembler gives a warning for non-hammerhead processors if we add
// the following directive

#ifdef __SIMDSHARC__
.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases
#endif

.EXTERN STRUCT DPCQueue _tmk_dpcQueue;

.SECTION/DM seg_dmda;

.VAR .SaveR0, .SaveI2, .SaveI4;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/PM seg_swco;
#elif defined (__2137x__)
/* the code needs to be in internal memory to avoid anomaly 09000011 */
.SECTION/PM seg_int_code;
#else
.SECTION/PM seg_pmco;
#endif

.GLOBAL _TMK_InsertQueueDpc;
_TMK_InsertQueueDpc:    // C-callable function entry

	CALL TMK_InsertQueueDpc;

#if defined(__2116x__)
	// no BR glitch on this load as 2nd delay slot of RTS from CALL above
	// is NOP.
	I7=I6;
	I12=DM(-1,I6);
	NOP;					// Glitch workaround
	JUMP (M14,I12) (DB);
		I6=DM(0,I6);			// daren't use RFRAME on 21160
		NOP;
#else
	I12=DM(-1,I6);
	JUMP (M14,I12) (DB);
		NOP;
		RFRAME;
#endif
	
._TMK_InsertQueueDpc.end:


#if defined (__2106x__)
/* the code needs to be in internal memory because we are modifying IRPTL
   and otherwise we could hit anomaly 14000022 if the code goes in external
   memory. */
.SECTION/PM seg_int_code;
#endif

.GLOBAL TMK_InsertQueueDpc;
TMK_InsertQueueDpc:    // Assembly-callable subroutine entry

    //
    // This function is intended for use both from assembly and from C/C++.
    // Therefore it must preserve all the registers that it uses, so as not
    // to force assembly callers to have to worry about saving scratch regs.
    //
	PUSH STS;  // preserve MODE1 and ASTAT
	JUMP (PC,.SH_INT_DISABLED_1)(DB);
		BIT CLR MODE1 IRPTEN;
		DM(.SaveR0) = R0;
.SH_INT_DISABLED_1:
	DM(.SaveI4) = I4;
	I4 = R4;
	DM(.SaveI2) = I2;
	
	R0 = DM(OFFSETOF(TMK_DpcElement, pNext), I4);
	R0 = PASS R0;

	// If NE then the DpcElement is already on the DPC queue
	//
	IF NE JUMP .done;

	// R4 = DpcElement (first argument to function)
	// Append the element to the DPC queue and raise the kernel interrupt
	//
	I2 = DM(_tmk_dpcQueue->ppTail);			    // I2 = tmk_dpcQueue.ppTail

	DM(OFFSETOF(TMK_DpcElement,pNext), I4) = R4; // pElement->pNext = pElement (list end stop)
	DM(_tmk_dpcQueue->ppTail) = R4;			 // tmk_dpcQueue.ppTail = &pElement.pNext
	DM(0, I2) = R4;								 // *tmk_dpcQueue.ppTail = pElement

	// Now we raise the reschedule interrupt to have the DPC serviced.
	//
	BIT SET IRPTL SFT3I;
	

.done:
	R0 = DM(.SaveR0);
	I2 = DM(.SaveI2);
	I4 = DM(.SaveI4);

	RTS(DB);
		POP STS;  // restore MODE1 and ASTAT
		NOP;
.TMK_InsertQueueDpc.end:





