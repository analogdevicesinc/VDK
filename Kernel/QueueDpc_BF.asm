/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <sys/platform.h>

#include "Trinity.h"
.file_attr ISR;
.file_attr OS_Internals;

#if defined(NUM_CORES) && (NUM_CORES > 1)
#error SMP spinlock support not in place yet
#endif

.IMPORT "Trinity.h";

.EXTERN STRUCT DPCQueue _tmk_dpcQueue;
 
.SECTION program;
.align 2;

.GLOBAL TMK_InsertQueueDpc;
TMK_InsertQueueDpc:    // Assembly-callable subroutine entry
.GLOBAL _TMK_InsertQueueDpc;
_TMK_InsertQueueDpc:    // C-callable function entry
    //
    // This function is intended for use both from assembly and from C/C++.
    // Therefore it must preserve all the registers that it uses, so as not
    // to force assembly callers to have to worry about saving scratch registers.
    //
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 Data Cache Dirty Bit Set when a
	// Load-Miss-Fill Is in Progress
	SSYNC;
#endif
	[--SP] = (R7:7,P5:3);       // preserve regs
	[--SP] = ASTAT;
	
	P5 = R0;					// get dpc ptr into P0
	P4.L = _tmk_dpcQueue;		// P1 = DPC queue
	P4.H = _tmk_dpcQueue;
#if defined (__SILICON_REVISION__) && (defined (__ADSPBF535))
	//Not sure if we should include this workaround. As written it trashed RETI,
	// which will be a problem if it is called from a non-reentrant ISR. Without
	// the workaround, on the other hand, it won't be entirely safe to call it
	// from, thread level. In practice neither case should apply to the Callback
	// Manager, and so it should be safe with or without the workaround.
	RETI = [SP++];
	CLI R7;     // disable interrupts while we access the DPC queue
	[--SP] = RETI;
#else
	CLI R7;                     // disable interrupts - we don't need the 535 anomaly workaround
#endif

	// In SMP we'd need to acquire the DPC queue's spinlock here,
	// for non-SMP it's sufficient just to disable interrupts.
	// Not having to call the locking functions makes it *much* easier to
	// make this function assembly-callable, and also makes it a lot faster.
	//
	P3 = [P5 + OFFSETOF(TMK_DpcElement, pNext)];
	CC = P3 == 0;
	
	IF !CC JUMP .done; // dpc.pNext != NULL, DPC is already on queue
    ANOM05000428_3NOPS
    
	P3 = [P4 + OFFSETOF(DPCQueue,ppTail)];		// P3 = tmk_dpcQueue.ppTail_m

	// Raise the kernel interrupt
	RAISE EVT_IVG14_P;
	
	// Append the DPC to the queue
	//
	[P5 + OFFSETOF(TMK_DpcElement,pNext)] = P5;	// device.pNext_m = device (list end stop)
	[P4 + OFFSETOF(DPCQueue,ppTail)]	  = P5;	// g_DpcQueue.ppTail_m = &device.pNext_m
	[P3]								  = P5;	// *(previous)g_DpcQueue.ppTail_m = device
	
.done:
	// in SMP we'd need to release the spinlock here
	STI R7;		// re-enable interrupts
	ASTAT = [SP++];
	(R7:7,P5:3) = [SP++];       // restore regs
	RTS;        // and return
._TMK_InsertQueueDpc.end:
.TMK_InsertQueueDpc.end:

