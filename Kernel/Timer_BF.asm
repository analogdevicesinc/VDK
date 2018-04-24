// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/*
 * Timer.asm
 *
 * This module contains the timer interrupt service routine for
 * the Trinity microkernel.
 *
 */

// This file selects which blackfin file is to be included depending on 
// the processor we are using

#include <sys/platform.h>

#include "Trinity.h"

.file_attr ISR;

#if defined(NUM_CORES) && (NUM_CORES > 1)
#error SMP spinlock support not in place yet
#endif

.IMPORT "Trinity.h";

.EXTERN STRUCT TimeQueue _tmk_timeQueue;
.EXTERN STRUCT DPCQueue  _tmk_dpcQueue;

// These macros provide a shorthand for the lengthy offset expressions which are made
// necessary by the fact that OFFSETOF() does not recognise nested structs.
//
#define OFFSETOF_dpc(x,y)  (OFFSETOF(TimeQueue, dpc) + OFFSETOF(TMK_DpcElement, y))
#define OFFSETOF_list(x,y) (OFFSETOF(TimeQueue, list) + OFFSETOF(TMK_TimeElement, y))


.SECTION program;
.ALIGN 2;

/*
 * This is the timer ISR for the microkernel. It initiates all time-driven pre-emptive
 * activity in the system. Logically it forms part of the time queue, but it is also a
 * client of the DPC queue. It also updates the global elapsed time count, 'uptimeTicks'.
 */
.global _tmk_TimerISR;
_tmk_TimerISR:

#if defined (__SILICON_REVISION__) && (defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
	SSYNC;
#endif

    // Save off the registers that we'll be using.
    [--SP] = (R7:6, P5:3);

    // Load up pointers to the Trinity time and DPC queues
    P5.L = LO(_tmk_timeQueue);
    P5.H = HI(_tmk_timeQueue);		// P5 = &tmk_timeQueue
    P4.L = LO(_tmk_dpcQueue);
    P4.H = HI(_tmk_dpcQueue);		// P5 = &tmk_dpcQueue

    // Save the arithmetic status.
	[--SP] = ASTAT;

#if defined (__SILICON_REVISION__) && !(defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
	// Workaround for anomaly 05-00-0283. 
    // A system MMR write is stalled indefinitely when killed in a particular 
    // stage.
    // The workaround is to reset the MMR logic with another killed MMR.
    // access that does not have other side effects on the application
    // No breakpoints should be placed before the wa_05000283_skip label
    // This anomaly affects all silicon revisions to date (0.1 0.2 0.3 0.4).
	//
	// P3, R7, and ASTAT must have been saved prior to this point.
	//
	CC = R0 == R0; // always true
	P3.H = 0XFFC0; // MMR space CHIPID
	P3.L = 0X0014;
	IF CC JUMP .wa_05000283_skip; // always skip MMR access, however, MMR access will be fetched and killed
    ANOM05000428_SUPPRESS_MMR_READ  //MMR is not in L2 so will not hit 05-00-0428 
	R7 = [P3]; // bogus MMR read to work around the anomaly
	ANOM05000428_2NOPS  // Only two NOPS required to avoid 05-00-0428
	
.wa_05000283_skip:  // continue with ISR
#endif
	
	// Load a pointer to the first element in the time queue.
    P3 = [P5 + OFFSETOF_list(_tmk_timeQueue, pNext)];      // P3 = tmk_timeQueue.list.pNext

#if defined (__SILICON_REVISION__) && !(defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
	// Workaround for anomaly 05-00-0257. Reload the loop counter registers to force the clearing
	// of the loop buffers, and hence prevent the wrong fetch address from being sent to the
	// instruction fetch unit if returning into a short hardware loop.

.message/suppress  5515 for 5 lines;
	R6=LC0;
	R7=LC1;
	LC0=R6;
	LC1=R7;
#endif

	// Increment the global uptime ticks and compare against the tick number of the first element in the time queue.
	//
    R7 = [P5 + OFFSETOF(_tmk_timeQueue, uptimeTicks)];     // R7 = tmk_timeQueue.uptimeTicks
    R6 = [P3 + OFFSETOF(TMK_TimeElement, tickNumber)];     // R6 = tmk_timeQueue.list.pNext->tickNumber
    R7 += 1;											   // tmk_timeQueue.uptimeTicks++
    [P5 + OFFSETOF(_tmk_timeQueue, uptimeTicks)] = R7;	   // tmk_timeQueue.uptimeTicks = R7

    // Test whether uptimeTicks has caught up with the list head
    CC = R7 == R6;

	IF !CC JUMP .HeadElementNotExpired;  // else skip over
    ANOM05000428_3NOPS
    
	// We only do this when the uptimeTicks catches up with the tickNumber of
	// the element at the head of the list.
	// Once this has happened, and the timequeue has been queued for DPC, it cannot
	// happen again until the DPC has been executed and the expired item has been
	// removed from the head of the timequeue.
	//
	// For this reason there is no explicit test for the time queue already being
	// on the dpc queue. This differs from all other insertions to the dpc queue.
	// Since the TimerISR is time-critical with respect to interrupt latency it
	// would be wasteful, as well as pointless, to perform such a test.

	// This sequence depends on timeQueue being the first field in the Globals struct, tmk.
	// We take advantage of the fact that the address of &tmk_TimeQueue == &tmk, which
	// allows us just to store the value in P5 (&tmk) rather than having to copy it to
	// another register and add an offset to get &tmk_timeQueue).
	//
	// We also depend on pNext being the first field in TMK_DpcElement, which allows us
	// to use the contents of P5 yet again as &dpc.pNext
	
	// The following three-instruction sequence depends on a number of offsets being zero,
	// i.e. on certain fields being the first within their containing structs:
	//
	// - dpc       must come first within struct TimeQueue
	// - pNext      "    "    "     "     struct TMK_DpcElement
	//
	// These contraints allow the same address in P5 to have the following meanings:
	//
	// - &tmk_timeQueue
	// - &tmk_timeQueue.dpc
	// - &tmk_timeQueue.dpc.pNext
	//
	// All three of these interpretations are in use below. Note that the expression
	// OFFSETOF_dpc(_tmk_timeQueue, pNext) is actually zero (and so the first line could
	// actually be [P5] = P5;) but since there is no run-time cost to include the offset
	// (provided it is small) we do so for clarity.
	//
	P3 = [P4 + OFFSETOF(_tmk_dpcQueue, ppTail)];	 // P3 = tmk_dpcQueue.ppTail

	// Now we raise the kernel interrupt to have the DPC serviced.
    RAISE EVT_IVG14_P;

	// Insert the timequeue into the DPC queue so the timeouts can be serviced at kernel level
	[P5 + OFFSETOF_dpc(_tmk_timeQueue, pNext)] = P5; // tmk_timeQueue.dpc.pNext = &tmk_timeQueue.dpc (list end stop)
	[P4 + OFFSETOF(_tmk_dpcQueue, ppTail)]	   = P5; // tmk_dpcQueue.ppTail = &tmk_timeQueue.dpc.pNext
	[P3 + OFFSETOF(TMK_DpcElement, pNext)]	   = P5; // *(previous)tmk_dpcQueue.ppTail = &dpc.timeQueue.dpc

.HeadElementNotExpired:

    // Restore the registers that we saved on entry.
    //
	ASTAT = [SP++];
    (R7:6, P5:3) = [SP++];

    // Return from the interrupt
#if defined (__SILICON_REVISION__) && (defined (__ADSPBF535__) || defined(__AD6532__) || defined(__AD6900__))
    // Workaround for ADSP-BF535 caching anomaly number 05000164 "store to load
    // forwarding in write through mode". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_wt_fix option.
    SSYNC;
    RTI;
    NOP;
    NOP;
#else
    RTI;
#endif
    
._tmk_TimerISR.end:


/*
TMK_GetUptime
Returns the uptime ticks from the globals struct.

C-callable, but preserves *all* registers except the return
value (R0).

Return type: unsigned int
*/
.global _TMK_GetUptime;
.global TMK_GetUptime;
_TMK_GetUptime:
TMK_GetUptime:
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 Data Cache Dirty Bit Set when a 
	// Load-Miss-Fill Is in Progress 
	SSYNC;
#endif
	[--SP] = P5;
    P5.L = _tmk_timeQueue;
    P5.H = _tmk_timeQueue;		   // P5 = &tmk_timeQueue

	R0 = [P5 + OFFSETOF(_tmk_timeQueue, uptimeTicks)]; // R0 = tmk_timeQueue.uptimeTicks

	P5 = [SP++];

	RTS;
.TMK_GetUptime.end:
._TMK_GetUptime.end:
