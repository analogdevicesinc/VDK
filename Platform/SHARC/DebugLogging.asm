// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include "ADI_attributes.h"
#include "asm_offsets.h"
#include "VDK_ISR_API.h"
.file_attr HistoryAndStatus;

#ifdef __2116x__
#define GLITCHNOP NOP
#else
#define GLITCHNOP
#endif

.EXTERN _VDK_ASM_History;
.EXTERN TMK_GetUptime;

.GLOBAL _History_FirstEntry__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_Interrupt__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_KillThread__3VDKFP10TMK_ThreadT1;
.GLOBAL _History_Runtime__3VDKFP10TMK_ThreadT1;
.GLOBAL _g_pfHistory_FirstTimeChain__3VDK;
.GLOBAL _g_pfHistory_InterruptChain__3VDK;
.GLOBAL _g_pfHistory_KillThreadChain__3VDK;
.GLOBAL _g_pfHistory_RuntimeChain__3VDK;

.SECTION/DM seg_dmda;

.VAR _g_pfHistory_FirstTimeChain__3VDK;
.VAR _g_pfHistory_RuntimeChain__3VDK;
.VAR _g_pfHistory_InterruptChain__3VDK;
.VAR _g_pfHistory_KillThreadChain__3VDK;

.var	debuglogging_R3;
.var	debuglogging_MODE1;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM seg_pmco;
#endif


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
	
    // Get a pointer to DebugInfo into I4
	I4 = R8;		 // I4 = new thread ptr
	GLITCHNOP;

	// -1 adjusts between TMK_Thread and VDK::Thread
    I4 = DM(OFFSET_Thread_m_DebugInfoP_ - 1, I4);   // DebugInfo structure pointer

    // Get the cycle count into R0:R1)
    R2 = EMUCLK2;	// load count MSW
    R1 = EMUCLK;	// load count LSW
    R0 = EMUCLK2;	// load count MSW again
    COMP(R0, R2);	// and check for MSW rollover
    IF NE R1 = EMUCLK;	// count rolled-over, reload LSW

    // and save it
    DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 0, I4) = R0; // (MSW), safe wrt. SWF
    DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 1, I4) = R1; // (LSW), safe wrt. SWF

    // Get the current Tick + Increment the number of times run using the delay slots.
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so 
    // changing code to a simple call
    R1 = 1;
    DM(OFFSET_DebugInfo_m_nNumTimesRun_, I4) = R1;
    CALL TMK_GetUptime ; // will only change r0

    DM(OFFSET_DebugInfo_m_tRunStartTime_, I4) = R0; // save tick count
    
	// Log the thread switch
	// Note that the first passed arg to this function is a dummy (NULL) argument
	// so we don't have to save/restore R4.
	I4 = R8; //The incoming thread structure
	R8 = DM(-1, I4); // Get the VDK ThreadID for the incoming thread
	
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so 
    // changing code to a simple call
	// R1 is *not* saved & restored with this call
	R4 = ENUM_VDK_kThreadSwitched_;
	R12 = VDK_KERNEL_LEVEL_; // Get the VDK ThreadID for the outgoing thread
	CALL _VDK_ASM_History ;
	
	R8 = I4; // Restore the incoming thread to R8
	I12 = DM(_g_pfHistory_FirstTimeChain__3VDK);
	R3 = DM(debuglogging_R3);
	JUMP (M13, I12) (DB);
	NOP;
	NOP;
	
	
._History_FirstEntry__3VDKFP10TMK_ThreadT1.end:


_History_Interrupt__3VDKFP10TMK_ThreadT1:
	I12 = DM(_g_pfHistory_InterruptChain__3VDK);
	//R2 = VDK_KERNEL_LEVEL_;
	JUMP .doLogging;
._History_Interrupt__3VDKFP10TMK_ThreadT1.end:


_History_KillThread__3VDKFP10TMK_ThreadT1:
	I12 = DM(_g_pfHistory_KillThreadChain__3VDK);
	JUMP .doLogging;
._History_KillThread__3VDKFP10TMK_ThreadT1.end:


_History_Runtime__3VDKFP10TMK_ThreadT1:
	I12 = DM(_g_pfHistory_RuntimeChain__3VDK);
	// Fall through

.doLogging:

	// Log the thread switch
	// We need to get the VDK ThreadIDs from the TMK_Thread pointers,
	// and store the ThreadIDs into R8 and R12. As we have registers
	// available we will save-off the TMK_Thread pointers to these regs
	// because we need to reuse R4 and R8 to pass data to the history 
	// subroutine.
	I4 = R4;
	R12 = DM(-1, I4);	// R12 = Outgoing threadID

	// Save-off the thread pointers
	R2 = R4;			// R2 = outgoing thread pointer
	I4 = R8;			// I4 = incoming thread pointer
	
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so 
    // changing code to a simple call
	// R1 is *not* saved & restored with this call
	R8 = DM(-1, I4);	// R8 = incoming threadID
	R4 = ENUM_VDK_kThreadSwitched_;
	CALL _VDK_ASM_History ;
	
	
	// Restore the thread pointers to their original registers.
	R8 = I4; // incoming thread pointer
	R4 = R2; // outgoing thread pointer
    

// --------------------------------------------------------------------------
// Set the Thread Stats for the thread  which is being switched OUT:
//
//	Save the following information:
//		1) Length of time (CYCLES) running since switched in:
//					(pOldThread->m_DebugInfoP->m_llRunLastCycles).
//		2) Total time (CYCLES) running since creation:
//					(pOldThread->m_DebugInfoP->m_llRunTotalCycles).
// 		3) Length of time (TICKS) running since switched in:
//					(pOldThread->m_DebugInfoP->m_tRunLastTime).
//		4) Total time (TICKS) running since creation:
//					(pOldThread->m_DebugInfoP->m_tRunTotalTime).
//
// --------------------------------------------------------------------------

    // Get a pointer to DebugInfo
    I4 = R4;		// old thread pointer
	GLITCHNOP;
	
    // Get a pointer to DebugInfo, -1 adjusts between TMK_Thread and VDK::Thread
    I4 = DM(OFFSET_Thread_m_DebugInfoP_ - 1, I4);   // DebugInfo structure pointer

	// Get the cycle count into R0:R1 (MSW:LSW)
    R2 = EMUCLK2;	// load count MSW
    R1 = EMUCLK;	// load count LSW
    R0 = EMUCLK2;	// load count MSW again
    COMP(R0, R2);	// and check for MSW rollover
    IF NE R1 = EMUCLK;	// count rolled-over, reload LSW

    // Get m_llRunStartCycles into R2:R12 (MSW:LSW)
    R2 = DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 0, I4); // (MSW)
    R12 = DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 1, I4); // (LSW)

    // Find the difference in the two times (into R0:R1)
    // i.e. last - start (R0:R1 = R0:R1 - R2:R12)
    R1 = R1 - R12;		// subtract LSWs
    R0 = R0 - R2 + CI - 1;	// and borrow from MSW

	// Save off the cycles that have elapsed since the thread was switched in
	DM(OFFSET_DebugInfo_m_llRunLastCycles_ + 0, I4) = R0; // (MSW), safe wrt. SWF
    DM(OFFSET_DebugInfo_m_llRunLastCycles_ + 1, I4) = R1; // (LSW), safe wrt. SWF

    // Get m_llRunTotalCycles into R2:R12 (MSW:LSW)
    R2 = DM(OFFSET_DebugInfo_m_llRunTotalCycles_ + 0, I4); // (MSW)
    R12 = DM(OFFSET_DebugInfo_m_llRunTotalCycles_ + 1, I4); // (LSW)

    // Add the difference to the TOTAL time run (in R2:R12)
    // i.e. R2:R12 = R2:R12 + R0:R1
    R12 = R12 + R1;		// add LSWs
    R2 = R2 + R0 + CI;		// and carry into MSW

    // and store the new value of m_llRunTotalCycles (in R2:R12)
    DM(OFFSET_DebugInfo_m_llRunTotalCycles_ + 0, I4) = R2; // (MSW), safe wrt. SWF
    DM(OFFSET_DebugInfo_m_llRunTotalCycles_ + 1, I4) = R12; // (LSW), safe wrt. SWF

    // Get the tick count
	CALL TMK_GetUptime; // will only change r0
    
	// R0 = tick count
	// Calculate how many ticks have elapsed since this thread was switched in
    R1 = DM(OFFSET_DebugInfo_m_tRunStartTime_, I4);
    R2 = R0 - R1;			   // difference last - start
    
    // Save this value off as RunLastTime (Ticks)
    DM(OFFSET_DebugInfo_m_tRunLastTime_, I4) = R2;

    // Add this onto the total time to obtain the new RunTotalTime (ticks)
    R1 = DM(OFFSET_DebugInfo_m_tRunTotalTime_, I4);
    R1 = R1 + R2;			   // total += (last - start)
    DM(OFFSET_DebugInfo_m_tRunTotalTime_, I4) = R1; // safe wrt. SWF

// --------------------------------------------------------------------------
// Set the Thread Stats for the thread which has just been switched IN:
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
    // We can trample all the registers we like (except I3, I4 and R4)
    // as they haven't been restored yet
	I4 = R8;

    // Get a pointer to DebugInfo, -1 adjusts between TMK_Thread and VDK::Thread
    I4 = DM(OFFSET_Thread_m_DebugInfoP_ - 1, I4);   // DebugInfo structure pointer

    // Save the tick count which is still in R0
    DM(OFFSET_DebugInfo_m_tRunStartTime_, I4) = R0;   // safe wrt. SWF

    // Get the cycle count into R0:R1)
    R2 = EMUCLK2;	// load count MSW
    R1 = EMUCLK;	// load count LSW
    R0 = EMUCLK2;	// load count MSW again
    COMP(R0, R2);	// and check for MSW rollover
    IF NE R1 = EMUCLK;	// count rolled-over, reload LSW

    // and save it
    DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 0, I4) = R0; // (MSW), safe wrt. SWF
    DM(OFFSET_DebugInfo_m_llRunStartCycles_ + 1, I4) = R1; // (LSW), safe wrt. SWF

    // Increment the number of times run
    R0 = DM(OFFSET_DebugInfo_m_nNumTimesRun_, I4);
    R0 = R0 + 1;

	JUMP (M13, I12) (DB);
	DM(OFFSET_DebugInfo_m_nNumTimesRun_, I4) = R0;
	NOP;
	
.doLogging.end:
._History_Runtime__3VDKFP10TMK_ThreadT1.end:
