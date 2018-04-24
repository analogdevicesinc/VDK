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
#error No SMP spinlock support on TS
#endif

.IMPORT "Trinity.h";

.EXTERN STRUCT TimeQueue _tmk_timeQueue;
.EXTERN STRUCT DPCQueue  _tmk_dpcQueue;
.EXTERN _tmk_RescheduleISR;

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
.file_attr ISR;

.SECTION data1;
.ALIGN 4;
.VAR .JSAVE[4],.KSAVE[4]	;

.SECTION program;

/*
 * This is the timer ISR for the microkernel. It initiates all time-driven pre-emptive
 * activity in the system. Logically it forms part of the time queue, but it is also a
 * client of the DPC queue. It also updates the global elapsed time count, 'uptimeTicks'.
 */
.global _tmk_TimerISR;
_tmk_TimerISR:
#ifdef __ADSPTS101__
	// TS101 anomaly workaround
	NOP;;
	NOP;;
	NOP;;
#else
	// TS20x anomaly workaround
	NOP;NOP;NOP;NOP;;
#endif

	Q[j31 + .KSAVE] = K31:28;; Q[k31 + .JSAVE] = J31:28;;

    // Load the locations of the two queues (while still saving registers).
    // Since the load will cause a stall of 3 cycles (if we used the addresses)
    // we bubble them up here...
    J28 = [J31 + _tmk_timeQueue->list->pNext];;		// J28 = tmk.timeQueue.pNext
 	J29 = [J31 + _tmk_dpcQueue->ppTail];;			// J29 = tmk.dpcQueue.ppTail_m

    K29 = [K31 +_tmk_timeQueue->uptimeTicks];;     // K29 = tmk.uptimeTicks

    K28 = [J28 + OFFSETOF(TMK_TimeElement, tickNumber)];; // K28 = tmk.timeQueue.list.pNext->tickNumber
    K29 = K29 + 1;;										  // increment uptimeTicks
    [K31 + _tmk_timeQueue->uptimeTicks] = K29;;   // tmk.uptimeTicks = K29
    COMP(K28,K29); J30 = _tmk_timeQueue;;	// J30 = &tmk.timeQueue

#ifdef __ADSPTS101__
    // If the queue head expired then raise the low priority software interrupt by starting timer0.
    IF KEQ; DO, TMRIN0L = 2;;   // hit timer0 (Dominic Vorassi's fix)
	K28 = (1 << ENABLE_TIMER_0_BIT);;
    IF KEQ; DO, SQCTLST = K28;; // start timer0
	K28 = ~(1 << ENABLE_TIMER_0_BIT);; // using NOT inst would change KEQ flag

#else // TS20x
	// Load the kernel interrupt address into K28, ready for writing to IVKERNEL.
	K28 = _tmk_RescheduleISR;;
#endif

	// We only do this when the deltaTicks of the list head object *crosses* zero.
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
	// These contraints allow the same address in J30 to have the following meanings:
	//
	// - &tmk_timeQueue
	// - &tmk_timeQueue.dpc
	// - &tmk_timeQueue.dpc.pNext
	//
	// All three of these interpretations are in use below.
	//
	IF KEQ; DO, [J31 + _tmk_timeQueue->dpc->pNext]      = J30;; // tmk.timeQueue.dpc.pNext = &tmk.timeQueue.dpc (list end stop)
	IF KEQ; DO, [J31 + _tmk_dpcQueue->ppTail]		    = J30;; // tmk.dpcQueue.ppTail = &tmk.timeQueue.dpc.pNext
	IF KEQ; DO, [J29 + OFFSETOF(TMK_DpcElement, pNext)] = J30;; // *(previous)tmk.dpcQueue.ppTail = &tmk.timeQueue.dpc

	// Now we raise the reschedule interrupt to have the DPC serviced.
	//
#ifdef __ADSPTS101__
	// Timer0 will have expired by now, so stop it
    IF KEQ; DO, SQCTLCL = K28;;
#else // TS20x
	// Raise the kernel interrupt (TS20x V1.0 and later onlyy)
    IF KEQ; DO, IVKERNEL = K28;;
#endif

	K31:28 = Q[j31 + .KSAVE];; J31:28 = Q[k31 + .JSAVE];;
	RTI (ABS)(NP);;

._tmk_TimerISR.end:


/*
TMK_GetUptime
Returns the uptime ticks from the globals struct.

C-callable, but preserves *all* registers except the return
value (J8).

Return type: unsigned int
*/
.global TMK_GetUptime;
.global _TMK_GetUptime;
TMK_GetUptime:
_TMK_GetUptime:
	J8 = [J31 + _tmk_timeQueue->uptimeTicks];; // J8 = tmk.uptimeTicks
        CJMP (ABS);;
._TMK_GetUptime.end:
.TMK_GetUptime.end:


