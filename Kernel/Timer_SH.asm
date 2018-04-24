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
#include "Trinity.h"

#if defined(NUM_CORES) && (NUM_CORES > 1)
#error No SMP spinlock support on SHARC
#endif

.IMPORT "Trinity.h";

#include <platform_include.h>

#include <asm_sprt.h>
.file_attr ISR;

// The assembler gives a warning for non-hammerhead processors if we add
// the following directive

#ifdef __SIMDSHARC__
.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases
#endif

.EXTERN STRUCT TimeQueue _tmk_timeQueue;
.EXTERN STRUCT DPCQueue  _tmk_dpcQueue;

.SECTION/DM seg_dmda;

.VAR .TimerPX1, .TimerPX2, .TimerR0L, .TimerR0H, .TimerI2;

#if defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#elif defined (__2137x__)
/* the code needs to be in internal memory to avoid anomaly 09000011 */
.SECTION/PM seg_int_code;
#elif defined (__2106x__)
/* the code needs to be in internal memory because we are modifying IRPTL
   and otherwise we could hit anomaly 14000022 if the code goes in external
   memory. */
.SECTION/PM seg_int_code;
#else
.SECTION/PM seg_pmco;
#endif

/*
 * This is the timer ISR for the microkernel. It initiates all time-driven pre-emptive
 * activity in the system. Logically it forms part of the time queue, but it is also a
 * client of the DPC queue. It also updates the global elapsed time count, 'uptimeTicks'.
 */
.global _tmk_TimerISR;
_tmk_TimerISR:
	// SIMD mode etc. automatically disabled by MMASK
	BIT CLR MODE1 ALUSAT;	// turn off ALU saturation mode

	// We use I2 to point at various data structures, in preference
	// to I0 because I0 can be subject to bit-reversal.
	DM(.TimerI2) = I2;

	// Save PX to memory, then use it to save the full 40 bits of R0 and R1. We
	// leave R1 in PX rather than writing it to memory.
	DM(.TimerPX1) = PX1;
	DM(.TimerPX2) = PX2;
	PX = R0;
	DM(.TimerR0L) = PX1;
	DM(.TimerR0H) = PX2;
	PX = R1;

    I2 = DM(_tmk_timeQueue->list->pNext);				  // I2 = tmk.timeQueue.pNext
    R0 = DM(_tmk_timeQueue->uptimeTicks);				  // R0 = tmk.uptimeTicks
    R1 = DM(OFFSETOF(TMK_TimeElement, tickNumber), I2);   // R1 = tmk.timeQueue.pNext->tickNumber
    R0 = R0 + 1;										  // increment uptimeTicks
	COMP (R0, R1);										  // Test whether uptimeTicks has caught up with the list head
	DM(_tmk_timeQueue->uptimeTicks) = R0;				  // tmk.uptimeTicks = R0

	IF NE JUMP .HeadElementNotExpired(DB);
		I2 = DM(_tmk_dpcQueue->ppTail);					  // I2 = tmk.dpcQueue.ppTail
		R0 = _tmk_timeQueue;								  // R0 = &tmk.timeQueue

	// We only do this when the uptimeTicks *equals* the tickNumber of the list head
	// Once this has happened, and the timequeue has been queued for DPC, it cannot
	// happen again until the DPC has been executed and the expired item has been
	// removed from the head of the timequeue.
	//
	// For this reason there is no explicit test for the time queue already being
	// on the dpc queue. This differs from all other insertions to the dpc queue.
	// Since the TimerISR is time-critical with respect to interrupt latency it
	// would be wasteful, as well as pointless, to perform such a test.

	// This sequence depends on timeQueue being the first field in the Globals struct, tmk.
	// We take advantage of the fact that the address of &tmk.TimeQueue == &tmk, which
	// allows us just to store the value in P5 (&tmk) rather than having to copy it to
	// another register and add an offset to get &tmk.timeQueue).
	//
	// We also depend on pNext being the first field in TMK_DpcElement, which allows us
	// to use the contents of P5 yet again as &dpc.tim ???
	
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
	// All three of these interpretations are in use below.
	//
	DM(_tmk_timeQueue->dpc->pNext)			= R0;		  // tmk.timeQueue.dpc.pNext = &tmk.timeQueue.dpc (list end stop)
	DM(_tmk_dpcQueue->ppTail)			    = R0;		  // tmk.dpcQueue.ppTail = &tmk.timeQueue.dpc.pNext
	DM(OFFSETOF(TMK_DpcElement, pNext), I2) = R0;		  // *(previous)tmk.dpcQueue.ppTail = &tmk.timeQueue.dpc

	// Now we raise the reschedule interrupt to have the DPC serviced.
	//
	BIT SET IRPTL SFT3I;

.HeadElementNotExpired:
	// Reload R0 and R1 via the PX register
	R1 = PX;
	PX1 = DM(.TimerR0L);
	PX2 = DM(.TimerR0H);
	R0 = PX;
	PX1 = DM(.TimerPX1);
	PX2 = DM(.TimerPX2);

    /* Cannot use a DB modifier with an RTI instruction due to Si Anomalies 
       02-00-0069, 04-00-0068, 06-00-0028, 07-00-0021, 08-00-0026, 09-00-0015 
       and 15-00-0002 */
	I2 = DM(.TimerI2);
	RTI;

._tmk_TimerISR.end:


/*
TMK_GetUptime
Returns the uptime ticks from the globals struct.

C-callable, but preserves *all* registers except the return
value (R0).

Return type: unsigned int
*/
.global TMK_GetUptime;
TMK_GetUptime:
	RTS(DB);
	R0 = DM(_tmk_timeQueue->uptimeTicks);
	NOP;
.TMK_GetUptime.end:
