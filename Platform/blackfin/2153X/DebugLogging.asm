// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#if (VDK_INSTRUMENTATION_LEVEL_==2)
#include "asm_offsets.h"
#include "VDK_ISR_API.h"
#include "VDK_AsmMacros.h"
.file_attr HistoryAndStatus;

#if defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__)
#define ALU_CARRY AC
#else
#define ALU_CARRY AC0
#endif

.GLOBAL _History_FirstEntry__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_Interrupt__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_KillThread__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_Runtime__3VDKFP10TMK_ThreadT1;
.GLOBAL _g_pfHistory_FirstTimeChain__3VDK;
.GLOBAL _g_pfHistory_InterruptChain__3VDK;
.GLOBAL _g_pfHistory_KillThreadChain__3VDK;
.GLOBAL _g_pfHistory_RuntimeChain__3VDK;

.SECTION data1;

.VAR _g_pfHistory_FirstTimeChain__3VDK;
.VAR _g_pfHistory_RuntimeChain__3VDK;
.VAR _g_pfHistory_InterruptChain__3VDK;
.VAR _g_pfHistory_KillThreadChain__3VDK;

.SECTION program;
    .align 4;

_History_FirstEntry__3VDKFP10TMK_ThreadT1:

// --------------------------------------------------------------------------
// Set the Thread Stats for the thread which has just been switched IN:
//
//	Save the following information:
// 		1) Start time (TICKS) NOW: 
//				(pNewThread->m_DebugInfoP->m_tRunStartTime).
//		2) Start time (CYCLES) NOW:
//				 (pNewThread->m_DebugInfoP->m_llRunStartCycles).
//		3) Increment the Number of times run:
//				 (pNewThread->m_DebugInfoP->m_nNumTimesRun).
//
// --------------------------------------------------------------------------

	P2 = R1; // Incoming TMK_Thread
	P0 = R0; // Outgoing TMK_Thread
	
	// -4 adjusts between TMK_Thread and VDK::Thread
	P2 = [P2 + OFFSET_Thread_m_DebugInfoP_ - 4];

	R3 = CYCLES;
	[P2 + OFFSET_DebugInfo_m_llRunStartCycles_ + 4] = R3;

	R3 = CYCLES2;
	[P2 + OFFSET_DebugInfo_m_llRunStartCycles_ + 0] = R3;

	// Setup the number of times run.
	//
	R3 = 1(X);
	[P2 + OFFSET_DebugInfo_m_nNumTimesRun_] = R3;
	
	// Get the start time for the incoming thread
	CALL.X TMK_GetUptime; // Returns tick in R0
	[P2 + OFFSET_DebugInfo_m_tRunStartTime_] = R0;	



	//Now prepare to write the history event
	[--SP] = RETS;
	
	// Log the history event. We know that this is only ever called from
	// Kernel Level, so using that ThreadID instead of the one passed in.	
	P2 = R1;

	// Populate the HistoryEnum.
	R0.L = LO(ENUM_VDK_kThreadSwitched_);
	R0.H = HI(ENUM_VDK_kThreadSwitched_);
	R1 = [P2 -4]; //The ThreadID of the first Thread to run
	R2.L = LO(VDK_KERNEL_LEVEL_);
	R2.H = HI(VDK_KERNEL_LEVEL_);
	
	CALL.X _VDK_History;	
	
	R1 = P2; // P2 should still point to the thread structure.
	
	RETS = [SP++];

	// restore first argument (even though it is unused and only ever
	// contains NULL). 
	// Second argument (R1) unchanged (restored above).
	
	P2.L = LO(_g_pfHistory_FirstTimeChain__3VDK);
	P2.H = HI(_g_pfHistory_FirstTimeChain__3VDK);
	P2 = [P2];
	
	R0 = P0; 
	JUMP (P2);

._History_FirstEntry__3VDKFP10TMK_ThreadT1.end:


_History_Interrupt__3VDKFP10TMK_ThreadT1:
	//R2.H = HI(VDK_KERNEL_LEVEL_);
	//R2.L = LO(VDK_KERNEL_LEVEL_);
	//P0 = R0;
	P2.L = LO(_g_pfHistory_InterruptChain__3VDK);
	P2.H = HI(_g_pfHistory_InterruptChain__3VDK);
	JUMP .doLogging1;
._History_Interrupt__3VDKFP10TMK_ThreadT1.end:


_History_KillThread__3VDKFP10TMK_ThreadT1:
	P2.L = LO(_g_pfHistory_KillThreadChain__3VDK);
	P2.H = HI(_g_pfHistory_KillThreadChain__3VDK);
	JUMP .doLogging1;
._History_KillThread__3VDKFP10TMK_ThreadT1.end:


_History_Runtime__3VDKFP10TMK_ThreadT1:
	P2.L = LO(_g_pfHistory_RuntimeChain__3VDK);
	P2.H = HI(_g_pfHistory_RuntimeChain__3VDK);
	// Fall through

.doLogging1:
	P0 = R0;
	// Outgoing ThreadID
	R2 = [P0 + -4]; // -4 is offset of VDK thread ID from TMK_Thread struct

.doLogging2: // if we come in here then R2 has already been set
	P1 = R1;
	P2 = [P2]; // might as well load the jump address early

    [--SP] = R0;
    [--SP] = R1;
	[--SP] = RETS;

	// Now prepare to write the history event, save regs which may be trashed
	[--SP] = P1; // P1 can be trashed with CALLs
	
	R0.L = LO(ENUM_VDK_kThreadSwitched_);
	R0.H = HI(ENUM_VDK_kThreadSwitched_);
	// Incoming ThreadID
	R1 = [P1 + -4]; // -4 is offset of VDK thread ID from TMK_Thread struct

	
	CALL.X _VDK_History; // Call the history logging function.

	// Restore P1
	P1 = [SP++];

// --------------------------------------------------------------------------
// Set the Thread Stats for the thread  which is being switched OUT:
//
//	Save the following information:
//		1) Length of time (CYCLES) running since switched in:
//					(_g_OldThreadPtr__3VDK->m_DebugInfoP->m_llRunLastCycles).
//		2) Total time (CYCLES) running since creation:
//					(_g_OldThreadPtr__3VDK->m_DebugInfoP->m_llRunTotalCycles).
// 		3) Length of time (TICKS) running since switched in:
//					(_g_OldThreadPtr__3VDK->m_DebugInfoP->m_tRunLastTime).
//		4) Total time (TICKS) running since creation:
//					(_g_OldThreadPtr__3VDK->m_DebugInfoP->m_tRunTotalTime).
//
// And set the Thread Stats for the thread which has just been switched IN:
//
//	Save the following information:
// 		1) Start time (TICKS) NOW: 
//				(pNewThread->m_DebugInfoP->m_timeStartRun).
//		2) Start time (CYCLES) NOW:
//				 (pNewThread->m_DebugInfoP->m_llStartCycles).
//		3) Increment the Number of times run:
//				 (pNewThread->m_DebugInfoP->m_nNumTimesRun).
//
// --------------------------------------------------------------------------

	P0 = [P0 + OFFSET_Thread_m_DebugInfoP_ - 4]; // -4 adjusts between TMK_Thread
	P1 = [P1 + OFFSET_Thread_m_DebugInfoP_ - 4]; // and VDK::Thread

	// This thread is switching out so update the last cycle count
	//
	R0 = CYCLES;
	R3 = CYCLES2;
	[P1 + OFFSET_DebugInfo_m_llRunStartCycles_ + 4] = R0; // Update lower count
	[P1 + OFFSET_DebugInfo_m_llRunStartCycles_ + 0] = R3; // Update upper count

    // Compute the differences and update the rest of the timer counters 
	// Find the differences in the two times
	//
	// Subtract the lower words
	//
	R2 = [P0 + OFFSET_DebugInfo_m_llRunStartCycles_ + 4];
	R2 = R0 - R2; 	// This time run LSW

	// ALU Carry is set to 0 if adjustments to CYCLES2 are required
	// due to overflow.
	//
	CC = ALU_CARRY;	 
	CC = !CC;		
	R0 = CC;
	R3 = R3 - R0;	// Subtract one from CYCLES2 if necessary

	// Save off the cycles that have elapsed since the thread was switched in
	[P0 + OFFSET_DebugInfo_m_llRunLastCycles_ + 4] = R2;
	[P0 + OFFSET_DebugInfo_m_llRunLastCycles_ + 0] = R3;

	R0 = [P0 + OFFSET_DebugInfo_m_llRunTotalCycles_ + 4];
	R0 = R0 + R2;	// Add this time run LSW to Total time LSW
	[P0 + OFFSET_DebugInfo_m_llRunTotalCycles_ + 4] = R0;
	
	CC = ALU_CARRY;	// ALU Carry is 1 if we have to carry a bit onto CYCLES2
	R0 = CC;
	R3 = R3 + R0;	// Add one to CYCLES2 if necessary

	R0 = [P0 + OFFSET_DebugInfo_m_llRunStartCycles_ + 0];
	R3 = R3 - R0; 	// This time run MSW

	R0 = [P0 + OFFSET_DebugInfo_m_llRunTotalCycles_ + 0];
	R0 = R0 + R3;	// Add this time run MSW to Total time MSW
	[P0 + OFFSET_DebugInfo_m_llRunTotalCycles_ + 0] = R0;

	// Update the cycle count, the call preserves all regs except R0 and RETS,
	// so we have to save RETS here.
	// We need to safe and restore P1 because call.x can trash that register
	[--SP] = P1;
	CALL.X  TMK_GetUptime;	// get the current UPTIME_TICKS
	P1 = [SP++];

	[P1 + OFFSET_DebugInfo_m_tRunStartTime_] = R0; // Store it in starttime

	// Calculate ticks elapsed since the old thread was switched in
	R3 = [P0 + OFFSET_DebugInfo_m_tRunStartTime_];
	R1 = R0 - R3;

	// Save off this value
    [P0 + OFFSET_DebugInfo_m_tRunLastTime_] = R1;

    // Add this onto the total time to obtain the new RunTotalTime (ticks)
	R3 = [P0 + OFFSET_DebugInfo_m_tRunTotalTime_];
	R3 = R3 + R1;
	[P0 + OFFSET_DebugInfo_m_tRunTotalTime_] = R3;

	// Increment the number of times run
	//
	R1 = [P1 + OFFSET_DebugInfo_m_nNumTimesRun_];
	R1 += 1;
	[P1 + OFFSET_DebugInfo_m_nNumTimesRun_] = R1;

	RETS = [SP++];
	R1 = [SP++];
	R0 = [SP++];
	JUMP (P2);

.doLogging2.end:
.doLogging1.end:
._History_Runtime__3VDKFP10TMK_ThreadT1.end:

#endif
