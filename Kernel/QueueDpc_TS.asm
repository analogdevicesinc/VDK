// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#if defined(__ADSPTS101__)
#include <defts101.h>

//
// These bit-position constants should be in defts101.h, but aren't.
//
#define ENABLE_TIMER_0_BIT 12
#define ENABLE_TIMER_1_BIT 13
#elif defined (__ADSPTS20x__)
#include <defts201.h>
#endif

#include "Trinity.h"
.file_attr ISR;
.file_attr OS_Internals;

#if defined(NUM_CORES) && (NUM_CORES > 1)
#error No SMP spinlock support in TS
#endif

.IMPORT "Trinity.h";

.EXTERN STRUCT DPCQueue _tmk_dpcQueue;
.EXTERN _tmk_RescheduleISR;

.SECTION program;

.GLOBAL TMK_InsertQueueDpc;
TMK_InsertQueueDpc:     // Assembly-callable subroutine entry
.GLOBAL _TMK_InsertQueueDpc;
_TMK_InsertQueueDpc:    // C-callable function entry
    //
    // This function is intended for use both from assembly and from C/C++.
    // Therefore it must preserve all the registers that it uses, so as not
    // to force assembly callers to have to worry about saving scratch registers.
    //
	Q[j27 += -4] = K31:28; Q[k27 += -4] = J31:28;;       // preserve regs

#ifdef __ADSPTS101__
.ALIGN_CODE 4;
	J28 = [J4 + OFFSETOF(TMK_DpcElement, pNext)];;
	COMP(J28,0);;
	
.ALIGN_CODE 4;
	IF NJEQ, JUMP .done;; // element already on DPC queue

	// J24 = DpcQueueElement
	// Append the device to the DPC queue
	// and assert the VDK_INT_ by starting and stopping timer0
	//
	J29 = [J31 + _tmk_dpcQueue->ppTail];			// J29 = tmk_dpcQueue.ppTail
		TMRIN0L = 2;;								// hit timer0 (Dominic Vorassi's trick)
	[J4 + OFFSETOF(TMK_DpcElement,pNext)] = J4;		// pElement->pNext = pElement (list end stop)
		K28 = MAKE_BITMASK_(ENABLE_TIMER_0_BIT);;	
	[J31 + _tmk_dpcQueue->ppTail] = J4;			// tmk_dpcQueue.ppTail = &pElement.pNext
		SQCTLST = K28;;								// start timer0
	[J29 + 0] = J4;									// *tmk_dpcQueue.ppTail = pElement
		K28 = ~MAKE_BITMASK_(ENABLE_TIMER_0_BIT);;	
	NOP;;											// wait
	SQCTLCL = K28;;									// stop timer0
.done:
	K31:28 = Q[j27 + 4]; J31:28 = Q[k27 + 4];;       // restore regs
	j27 = j27 + 4; k27 = k27 + 4;;        // and return, adjusting stack ptrs
.ALIGN_CODE 4;
	CJMP (ABS);; 
#else // TS20x
	//
	// Test the GIE bit in SQCTL to see whether interrupts are globally disabled
	// at present. Although this is a test-and-decide with interrupts (potentially)
	// still enabled it ought to be safe as the global enable state cannot change
	// on us. If interrupts are disabled at the time we read SQCTL then nothing can
	// happen between there and the write to SQCTLCL, and if interrupts are enabled
	// and we *are* interrupted after reading SQCTL then interrupts must still be
	// enabled when the interrupt returns.
	//
	k29 = SQCTL;;
	k29 = k29 AND SQCTL_GIE;;

	SQCTLCL = ~SQCTL_GIE;;                     // disable interrupts

	J28 = [J4 + OFFSETOF(TMK_DpcElement, pNext)];;
	COMP(J28,0);;
	
	// J4 = DpcQueueElement (first argument to function)
	// Append the element to the DPC queue and raise the kernel interrupt
	//
	J29 = [J31 + _tmk_dpcQueue->ppTail];;			    // J29 = tmk_dpcQueue.ppTail

	// If NJEQ then the DeviceDriver is already on the DPC queue
	//
	IF JEQ; DO, [J4 + OFFSETOF(TMK_DpcElement,pNext)] = J4;	// pElement->pNext = pElement (list end stop)
	        DO, K28 = _tmk_RescheduleISR;;					// get the vector address into K28
	IF JEQ; DO, [J31 + _tmk_dpcQueue->ppTail] = J4;;		// tmk_dpcQueue.ppTail = &pElement.pNext
	IF JEQ; DO, [J29 + 0] = J4;								// *tmk_dpcQueue.ppTail = pElement
	        DO, IVKERNEL = K28;;							// raise the kernel interrupt

	// k29 contains SQCTL_GIE if interrupts were enabled in SQCTL on entry,
	// otherwise it contains zero.
	//
	SQCTLST = k29;;		// re-enable interrupts

	K31:28 = Q[j27 + 4]; J31:28 = Q[k27 + 4];;       // restore regs
	CJMP (ABS);	j27 = j27 + 4(NF); k27 = k27 + 4(NF);;        // and return, adjusting stack ptrs
#endif
._TMK_InsertQueueDpc.end:
.TMK_InsertQueueDpc.end:
