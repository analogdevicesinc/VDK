// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include "Trinity.h"

.file_attr ISR;
.file_attr OS_Internals;

.IMPORT "Trinity.h";

#if defined(__ADSPTS101__)  /*******************************************************************/

#include <defts101.h>

#define TS10X(x) x
#define TS20X(x)

.EXTERN _lck_TS10xIMASKHShadow;

#elif defined(__ADSPTS20x__)  /*****************************************************************/

#include <defts201.h>

#ifdef __TS201_V0
#error TS20x silicon versions prior to 1.0 are not supported
#endif

#define TS10X(x)
#define TS20X(x) x

#endif  /***************************************************************************************/

.IMPORT "TSCtxtRec.h";

.EXTERN STRUCT tmk_Globals _tmk;
//.EXTERN _tmk_pCurrentThread;

.EXTERN ldf_stack_end;

.SECTION data1;

.align 4;
.VAR .J_Temp[4];
.VAR .K_Temp[4];
.VAR .Y_Exception_Temp[4];

.SECTION program;

// Functions used by this module
.EXTERN _TMK_ReleaseMasterKernelLock;
.EXTERN _TMK_ReleaseMasterKernelLockFromKernelLevel;
.EXTERN _tmk_MaskMasterKernelLockForThreadLevel;
.EXTERN _tmk_Dispatcher;
.EXTERN UserExceptionHandler;

.EXTERN ldf_jstack_base;
.EXTERN ldf_kstack_base;

// Globals used by this module
.EXTERN _cplb_hdr;
.EXTERN __ivsw_contents_at_startup;

// Functions exported by this module
.GLOBAL _tmk_RescheduleISR;
.GLOBAL _tmk_SwitchContext_Runtime;
.GLOBAL _tmk_SwitchContext_FirstThread;
.GLOBAL _tmk_SwitchContext_ISR;
.GLOBAL _tmk_ExceptionHandler;
.GLOBAL _tmk_RunOnSystemStack;
.GLOBAL _tmk_SwitchContext_KillThread;

// Trap number reserved for software debuggers such as BitWare's
#define SW_DBG_EXCEPTION_NUM_ 31
#define EXCEPTION_NUM 0

#define LOC(X) OFFSETOF(TSContext, X)

/*
tmk_SwitchContext_Runtime
C-callable entrypoint which is called, via a function pointer,
from tmk_ScheduleThread() only. UNTRUE

When we come in here we know that:
- we're running at thread level
- we're holding the kernel lock, acquired by thread code
- the old thread ptr is in J4 (first argument)
- the new thread ptr is in J5 (second argument)

Return type: void
*/
_tmk_SwitchContext_Runtime:

    // Work in scratch registers only until we've saved the preserved regs

    // We are saving a runtime context, and will save this info in
    // the old thread pointer in SaveContext_Common below.
    xR3 = RUNTIME;;
    xR2 = NOCONTEXT;;

	// Get pointers to the context save areas...
    j30 = [j4 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	k30 = [j4 + OFFSETOF(TMK_Thread, pContextSaveK)];;

    // Mark the old thread as containing the context of
    // a "runtime" (i.e. voluntarily context-switched) thread.
	// We can do this now (i.e. before the regs have actually been
	// written) because we're holding the master kernel lock so
	// nothing's going to change under us.
    [j4 + OFFSETOF(TMK_Thread,contextLevel)] = xR3;;

	// Preserved X and Y Compute Block Registers (24-31)
	Q[j30 + LOC(XYR27_24)] = xR27:24;     Q[k30 + LOC(XYR27_24)] = yR27:24;;
	Q[j30 + LOC(XYR31_28)] = xR31:28;     Q[k30 + LOC(XYR31_28)] = yR31:28;;

	// Preserved J and K IALU Registers (16-27)
	Q[J30 + LOC(JK19_16)] = K19:16;       Q[K30 + LOC(JK19_16)] = J19:16;;
	Q[J30 + LOC(JK23_20)] = K23:20;       Q[K30 + LOC(JK23_20)] = J23:20;;
	Q[J30 + LOC(JK27_24)] = K27:24;       Q[K30 + LOC(JK27_24)] = J27:24;;

	// return address register
	[J30 + LOC(CJMP)] = CJMP;;

.RestoreAfterKill:
    // First we do a three-way branch according to the
    // ContextLevel of the thread:
    // - NOTRUN: goes to .firstEntryFromThreadLevel
    // - INTERRUPTED: goes to .RestoreISRContextFromThreadLevel
    // - RUNTIME: falls though to .RestoreRuntimeContextFromThreadLevel
    //
    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP.

    // Get the thread's variable that says what type of context is stored
    J0 = [J5 + OFFSETOF(TMK_Thread,contextLevel)];;

    // Now we fan-out to the different sorts of context
	COMP(J0, RUNTIME);;  // compare prev contextLevel against RUNTIME
	IF NJEQ, JUMP .notRuntime_Thread; COMP(J0, NOTRUN);;

.RestoreRuntimeContextFromThreadLevel: // label not used
	// If we're here then we're doing a voluntary runtime-to-
	// -runtime context switch, e.g. a thread yield or posting
	// a message or semaphore to a higher-priority pending
	// thread. This is one of the cases that we want to run
	// as fast as possible so we want it on the shortest code path.
    //
	// We know that we're returning to a C runtime, and hence that
    // the return PC is in the (saved) CJMP register.

    // J5 currently contains the thread structure pointer
	// xR2 = NOCONTEXT

	// Set contextLevel to NOCONTEXT. It's ok to do this now as we're
	// holding the master kernel lock and so can't be pre-empted.
	//
    [J5 + OFFSETOF(TMK_Thread,contextLevel)] = xR2;;

	// Get pointers to the context save buffers...
    J30 = [J5 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	K30 = [J5 + OFFSETOF(TMK_Thread, pContextSaveK)];;

	// Return address register
	J19  = [j30 + LOC(CJMP)];;
	CJMP  = J19;;

	// Preserved X and Y Compute Block Registers
	xR27:24 = Q[j30 + LOC(XYR27_24)];     yR27:24 = Q[k30 + LOC(XYR27_24)];;
	xR31:28 = Q[j30 + LOC(XYR31_28)];     yR31:28 = Q[k30 + LOC(XYR31_28)];;

	// Preserved J and K IALU Registers
	K19:16 = Q[j30 + LOC(JK19_16)];      J19:16 = Q[k30 + LOC(JK19_16)];;
	K23:20 = Q[j30 + LOC(JK23_20)];      J23:20 = Q[k30 + LOC(JK23_20)];;

	// Note that this *isn't* the usual C return sequence. Instead of
	// loading K27:24 and J27:24 from the stack frame (which we can't do
	// as we don't have the stack or frame pointers yet) we are restoring
	// them directly from the context record(s). Since we didn't establish
	// a stack frame on entry (to _SwitchContext_Runtime__3VDKFv) these
	// regs are exactly as passed in by the caller.
	CJMP (ABS); K27:24 = Q[j30 + LOC(JK27_24)]; J27:24 = Q[k30 + LOC(JK27_24)];;
    
    
.notRuntime_Thread:
    // We could (possibly) release the master kernel lock here,
    // but we don't at the moment. Instead it gets released
    // separately on each of the two code paths (interrupt and
    // first-time).
	
	// We did the XCOMP in the same line as the jump to here	
.align_code 4;
    IF JEQ, JUMP .firstEntryFromThreadLevel;;

.RestoreISRContextFromThreadLevel: // label not used

	// Get the new thread pointer and the pointers to the
	// context save records into preserved regs (so they
	// won't get trashed by the call below).
	//
    J20 = J5;;
    J22 = [J5 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	K22 = [J5 + OFFSETOF(TMK_Thread, pContextSaveK)];;

	// Whereas all the other (five) context-restore scenarios execute
	// with the master kernel lock held, and hence are not pre-emptible,
	// this case (restoring an interrupted thread from runtime level)
	// releases the master kernel lock early, and depends on the
	// restore sequence being *restartable* in the event of being pre-
	// -empted.
	//
	// Once we've released the master kernel lock the reschedule
	// interrupt may take us to kernel level, in which case we won't
	// return to the point of interruption. Instead the context-restore
	// will be re-started from scratch the next time we try to schedule
	// this thread.

    // Release the kernel lock before running the thread. We must
	// have finished accessing the thread struct by this point.
    CALL _TMK_ReleaseMasterKernelLock(ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;

    // If we're here then we're restoring an interrupted thread, i.e.
    // the entire register set.

	// Start reloading the registers from the context record
	//
	xR0 = [j22 + LOC(SFREG)];;
	SFREG = xR0;;

#ifdef __ADSPTS101__
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	xR7:4 = Q[J22 + LOC(TR3_0)];     yR7:4 = Q[K22 + LOC(TR3_0)];;
	xR3:0 = Q[J22 + LOC(TR7_4)];     yR3:0 = Q[K22 + LOC(TR7_4)];	TR3:0   = R7:4;;
	xR7:4 = Q[J22 + LOC(TR11_8)];    yR7:4 = Q[K22 + LOC(TR11_8)];	TR7:4   = R3:0;;
	xR3:0 = Q[J22 + LOC(TR15_12)];   yR3:0 = Q[K22 + LOC(TR15_12)];	TR11:8  = R7:4;;
	xR7:4 = Q[J22 + LOC(PR1_0_THR1_0)]; yR7:4 = Q[K22 + LOC(PR1_0_THR1_0)];	TR15:12 = R3:0;;

	// The Parallel Results and ??? registers (SIMD)
	PR1:0  = R5:4;;
	THR1:0 = R7:6;;
	
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	R7:4 = TR3:0;		Q[J22 + LOC(PR1_0_THR1_0)] = xR3:0;	Q[K22 + LOC(PR1_0_THR1_0)]  = yR3:0;;
	R3:0 = TR7:4;		Q[J22 + LOC(TR3_0)]  = xR7:4;		Q[K22 + LOC(TR3_0)]   = yR7:4;;	
	R7:4 = TR11:8;		Q[J22 + LOC(TR7_4)]  = xR3:0;		Q[K22 + LOC(TR7_4)]   = yR3:0;;
	R3:0 = TR15:12;		Q[J22 + LOC(TR11_8)]  = xR7:4;		Q[K22 + LOC(TR11_8)]  = yR7:4;;	
						Q[J22 + LOC(TR15_12)] = xR3:0;		Q[K22 + LOC(TR15_12)] = yR3:0;;
#else  // TS20x
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	xR3:0 = Q[J22 + LOC(THR3_0)];	yR3:0 = Q[K22 + LOC(THR3_0)];;
	xR7:4 = Q[J22 + LOC(TR3_0)];     yR7:4 = Q[K22 + LOC(TR3_0)];		THR3:0  = R3:0;;
	xR3:0 = Q[J22 + LOC(TR7_4)];     yR3:0 = Q[K22 + LOC(TR7_4)];	    TR3:0   = R7:4;;
	xR7:4 = Q[J22 + LOC(TR11_8)];    yR7:4 = Q[K22 + LOC(TR11_8)];	TR7:4   = R3:0;;
	xR3:0 = Q[J22 + LOC(TR15_12)];   yR3:0 = Q[K22 + LOC(TR15_12)];	TR11:8  = R7:4;;
	xR7:4 = Q[J22 + LOC(TR19_16)];   yR7:4 = Q[K22 + LOC(TR19_16)];	TR15:12 = R3:0;;
	xR3:0 = Q[J22 + LOC(TR23_20)];   yR3:0 = Q[K22 + LOC(TR23_20)];	TR19:16 = R7:4;;
	xR7:4 = Q[J22 + LOC(TR27_24)];   yR7:4 = Q[K22 + LOC(TR27_24)];	TR23:20 = R3:0;;
	xR3:0 = Q[J22 + LOC(TR31_28)];   yR3:0 = Q[K22 + LOC(TR31_28)];	TR27:24 = R7:4;;
	xR7:4 = Q[J22 + LOC(PR1_0_BFOTMP)]; yR7:4 = Q[K22 + LOC(PR1_0_BFOTMP)];	TR31:28 = R3:0;;

	// The Parallel Results, Bit FIFO Overflow registers and
	// communication Control registers.
	PR1:0  = R5:4;;
	BFOTMP = R7:6;	xR0 = [J22 + LOC(CMCTL)];        yR0 = [K22 + LOC(CMCTL)];;
	CMCTL = R0;;
#endif
	// DABs
	J0 = J22 + LOC(DAB);					 K0 = K22 + LOC(DAB);;
	XR3:0 = DAB Q[J0 += 4];				 YR3:0 = DAB Q[K0 += 4];;

	// Accumulators, circular buffer regs and return addresses
	xR3:0 = Q[j22 + LOC(MR3_0)];          yR3:0 = Q[k22 + LOC(MR3_0)];;
	xMR3:2 = R3:2;                       yMR3:2 = R3:2;;
	xMR1:0 = R1:0;                       yMR1:0 = R1:0;;
	xR0   = [j22 + LOC(MR4)];            yR0   = [k22 + LOC(MR4)];;
	xMR4 = R0;                           yMR4 = R0;;
	JL3:0 = Q[j22 + LOC(L3_0 )];          KL3:0 = Q[k22 + LOC(L3_0 )];;
	JB3:0 = Q[j22 + LOC(B3_0 )];          KB3:0 = Q[k22 + LOC(B3_0 )];;

	XR0   = [j22 + LOC(CJMP)];;
	CJMP  = XR0;;

	// Compute Block Status registers
	xR0 = [j22 + LOC(STAT)];             yR0 = [k22 + LOC(STAT)];;
	XSTAT = R0;                          YSTAT = R0;;

	// Zero-overhead Loop Registers
	xR0 = [j22 + LOC(LC)];				  yR0 = [k22 + LOC(LC)];;
	LC0 = xR0;;
	LC1 = yR0;;

	// X and Y Compute Block Registers
	xR3:0   = Q[j22 + LOC(XYR3_0  )];     yR3:0   = Q[k22 + LOC(XYR3_0  )];;
	xR7:4   = Q[j22 + LOC(XYR7_4  )];     yR7:4   = Q[k22 + LOC(XYR7_4  )];;
	xR11:8  = Q[j22 + LOC(XYR11_8 )];     yR11:8  = Q[k22 + LOC(XYR11_8 )];;
	xR15:12 = Q[j22 + LOC(XYR15_12)];     yR15:12 = Q[k22 + LOC(XYR15_12)];;
	xR19:16 = Q[j22 + LOC(XYR19_16)];     yR19:16 = Q[k22 + LOC(XYR19_16)];;
	xR23:20 = Q[j22 + LOC(XYR23_20)];     yR23:20 = Q[k22 + LOC(XYR23_20)];;
	xR27:24 = Q[j22 + LOC(XYR27_24)];     yR27:24 = Q[k22 + LOC(XYR27_24)];;
	xR31:28 = Q[j22 + LOC(XYR31_28)];     yR31:28 = Q[k22 + LOC(XYR31_28)];;

	// J and K IALU Registers
	K3:0   = Q[j22 + LOC(JK3_0  )];      J3:0   = Q[k22 + LOC(JK3_0  )];;
	K7:4   = Q[j22 + LOC(JK7_4  )];      J7:4   = Q[k22 + LOC(JK7_4  )];;
	K11:8  = Q[j22 + LOC(JK11_8 )];      J11:8  = Q[k22 + LOC(JK11_8 )];;
	K15:12 = Q[j22 + LOC(JK15_12)];      J15:12 = Q[k22 + LOC(JK15_12)];;
	K19:16 = Q[j22 + LOC(JK19_16)];      J19:16 = Q[k22 + LOC(JK19_16)];;
	// J20-23 and K20-23 are restored below, in the exception handler
	K27:24 = Q[j22 + LOC(JK27_24)];      J27:24 = Q[k22 + LOC(JK27_24)];;
	K31:28 = Q[j22 + LOC(JK31_28)];      J31:28 = Q[k22 + LOC(JK31_28)];;

    // We're switching to an interrupted thread, so we have to get
    // to a higher level first. We're using exception level for this.
    TRAP EXCEPTION_NUM;;

.looptohere:
    JUMP .looptohere(NP);;
._tmk_SwitchContext_Runtime.end:


_tmk_ExceptionHandler:    
	// Anomaly workarounds (which???)
	TS10X(NOP;;NOP;;NOP;;)
	TS20X(NOP;NOP;NOP;NOP;;)
	
     // Save off some registers
    Q[J31 + .Y_Exception_Temp] = YR7:4;;

    // Figure out why we're here
// Anomaly workaround:
// in version 0.0 and 1.0 of the silicon we need to load SQSTAT twice on a row 
// to get the right result
// Anomaly 03-00-0216
    YR5 = SQSTAT;;
// end of Anomaly workaround
    YR4 = SQSTAT;;
    YR5 = EXCEPTION_NUM << SQSTAT_SPVCMD_P;;
    YR6 = SQSTAT_EXCAUSE | SQSTAT_SPVCMD;;
    YR7 = YSTAT;;

    YR6 = R4 AND R6;;
    YCOMP(R6, R5);;
    IF NYAEQ, JUMP .notReturnFromRestore;;

.return_From_Restore:  // label not used
    // We're going to return to the point where the thread (i.e. the one
	// being restored) was originally interrupted. Therefore, we are
	// not interested in the contents of RETS and instead set RETI to
	// the value from the thread's context record (through an intermediate
	// reg).
    yR5 = [j22 + LOC(RETI)];;
#ifdef __ADSPTS20x__
	yR4 = NOCONTEXT; RETI = yR5;;
#else	// TS101
	yR4 = NOCONTEXT;;
	RETI = yR5;;

	// Test the old state of IMASKH for anomaly 03000268 workaround
	YR5 = [J22 + LOC(IMASK)];;
	YBITEST R5 BY INT_GIE_P;; // OK to trash YSTAT as saved in YR7
	IF NYSEQ, JUMP .skip;;  // if GIE bit set then do nothing

	// The GIE bit was clear at the time this thread was pre-empted, so
	// we need to reload the saved value of IMASKH.
	IMASKH = YR5;;

.skip:
#endif

	// After we change contextLevel to NOCONTEXT any pre-emption will overwrite
	// the context record rather than preserving it. However, no pre-emption
	// can occur until we return from the exception handler.
	//
	[J20 + OFFSETOF(TMK_Thread,contextLevel)] = yR4;;	// NOCONTEXT

	// Now that we have finished with the thread pointer and the context
	// record pointers we can restore the last of the J and K registers.
	// Restoring YSTAT here seems safer than doing it in the same line as
	// RTI below, in case there's an effect delay on changes to YSTAT.
	//
	K23:20 = Q[j22 + LOC(JK23_20)];      J23:20 = Q[k22 + LOC(JK23_20)]; YSTAT = R7;;

	// Restore the registers saved on entry to _tmk_ExceptionHandler (YSTAT
    // done on line above) and jump back to the thread.
	//
    RTI (NP)(ABS); YR7:4 = Q[J31 + .Y_Exception_Temp];;

.notReturnFromRestore:
    // We're not here for a VDK exception
	// Workaround to support software debuggers (e.g. BitWare)
#ifdef __ADSPTS20x__
	YR5 = SW_DBG_EXCEPTION_NUM_ << SQSTAT_SPVCMD_P;
        YR4 = CJMP;;     // store CJMP to restore it later
    YCOMP(R6, R5); YR5 = UserExceptionHandler;;
	IF yaeq; DO, YR5 = [J31 + __ivsw_contents_at_startup];; // the address of the monitor...
	CJMP = YR5; YSTAT = R7;;
	CJMP (ABS); CJMP = YR4; YR7:4 = Q[J31 + .Y_Exception_Temp];;   // jump to the monitor code and reinstate cjmp as we branch
#else  // TS101
	YR5 = SW_DBG_EXCEPTION_NUM_ << SQSTAT_SPVCMD_P;;
    RETI = CJMP;;     // store CJMP to restore it later
    YCOMP(R6, R5); YR5 = UserExceptionHandler;;
	IF yaeq; DO, YR5 = [J31 + __ivsw_contents_at_startup];; // the address of the monitor...
	CJMP = YR5; YSTAT = R7;;
	CJMP (ABS); CJMP = RETI; YR7:4 = Q[J31 + .Y_Exception_Temp];;   // jump to the monitor code and reinstate cjmp as we branch
#endif

._tmk_ExceptionHandler.end:


_tmk_SwitchContext_FirstThread:    // new thread ptr passed as 2nd argument, 1st is dummy

	xR2 = NOCONTEXT;;  // we'll be wanting this later

.firstEntryFromThreadLevel:
	// We *must* be finished accessing the thread struct and the context struct
	// before we release the master kernel lock, because once we release it we
	// could be pre-empted, which would overwrite the context before we're finished
	// loading it. This would be bad.

	// Get the pointers to the context records
    j30 = [j5 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	k30 = [j5 + OFFSETOF(TMK_Thread, pContextSaveK)];;

    // Get the address of the new thread's run function into
    // a preserved register, so it doesn't get trashed by the
    // call to TMK_ReleaseMasterKernelLock().
    j20 = [j5 + OFFSETOF(TMK_Thread,pStuff)];;       // get run function ptr
	j21 = j5;; // copy the thread ptr to a preserved register

	// Load stack and frame pointers from the context record.
	K27:24 = Q[j30 + LOC(JK27_24)];      J27:24 = Q[k30 + LOC(JK27_24)];;

    // Mark in the thread object that there's no context saved for this thread.
    [J5 + OFFSETOF(TMK_Thread,contextLevel)] = xR2;; // here's the one I prepared earlier

    // There are two calls below that will require that the stack pointer be in a position
    // where there are words available at a positive offset.
    // Currently both the J and K stacks point to the first free quad word. We need
    // some more space prior to calling into a C environment.
    // J-stack we require space for the linkage info at +4 and outgoing args
    // at +8. So we offset the stack pointer by another 8 words.
    // K-stack just requires extra space for the linkage info at +4.
    J27 = J27 - 8; K27 = K27 - 4;;
    
    // There's an argument that we can do the above in ThreadMemoryAlloc, but it
    // would require extra code for the FreeThreadMemory to calculate the start of
    // memory allocation for a call to heap_free.
    
    // Release the kernel lock before running the thread.
	// This call runs on the new thread's stack.
	//
    CALL _TMK_ReleaseMasterKernelLock(ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;

	CJMP = j20;;
	CJMP (ABS); j4 = j21;;  // pass thread pointer as first argument to run function

    // If we got here then the run function returned, this will be a kernel panic.
.align_code 4;
.loopStop1:
    JUMP .loopStop1;;
._tmk_SwitchContext_FirstThread.end:


/*
tmk_SwitchContext_ISR
C-callable entrypoint which is called, via a function pointer,
from tmk_Dispatcher() only.

When we come in here we know that:
- we're running at kernel level, still at reschedule interrupt level
- we're holding the kernel lock, acquired by tmk_Dispatcher()
- the old thread ptr is in J4 (first argument)
- the new thread ptr is in J5 (second argument)

Return type: void
*/
_tmk_SwitchContext_ISR:
    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    xR2 = INTERRUPTED;;
    xR3 = NOCONTEXT;;
    
    // Get the new thread's variable that says what type of context is stored...
    J21 = [J5 + OFFSETOF(TMK_Thread,contextLevel)];;

    J22 = [J5 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	K22 = [J5 + OFFSETOF(TMK_Thread, pContextSaveK)];;

    // The entire register set of the old thread has already been saved on entry
    // to the reschedule ISR, so all we have to do now is
    // mark the old thread as containing the context
    // of an interrupted (i.e. pre-empted) thread.
    [J4 + OFFSETOF(TMK_Thread,contextLevel)] = xR2;;

    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP, so we do that here.
    [J5 + OFFSETOF(TMK_Thread,contextLevel)] = xR3; J20 = J5;;

    // Next we do a three-way branch according to the contextLevel of the new thread:
    COMP(J21,RUNTIME);;
    IF NJEQ, JUMP .notRuntime_ISR; COMP(J21,INTERRUPTED);;

	// the contextLevel was RUNTIME
	//
.RestoreRuntimeContextFromInterruptLevel:  // label not used
    CALL _tmk_MaskMasterKernelLockForThreadLevel(ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;

	// Get the Return address (stored from CJMP) into RETI. CJMP is scratch wrt. the caller
	// so we don't need to restore it.
	//
	xR24  = [j22 + LOC(CJMP)];;
	RETIB  = xR24;;
	TS10X(RETIB  = xR24;;)  // anomaly 03000136 workaround

	// Preserved X and Y Compute Block Registers
	xR27:24 = Q[j22 + LOC(XYR27_24)];     yR27:24 = Q[k22 + LOC(XYR27_24)];;
	xR31:28 = Q[j22 + LOC(XYR31_28)];     yR31:28 = Q[k22 + LOC(XYR31_28)];;

	// Preserved J and K IALU Registers
	              K19:16 = Q[j22 + LOC(JK19_16)]; J19:16 = Q[k22 + LOC(JK19_16)];;
	              K27:24 = Q[j22 + LOC(JK27_24)]; J27:24 = Q[k22 + LOC(JK27_24)];;
    RTI(ABS)(NP); K23:20 = Q[j22 + LOC(JK23_20)]; J23:20 = Q[k22 + LOC(JK23_20)];;


	// The contextLevel wasn't RUNTIME
	//
.notRuntime_ISR:
	// On TigerSHARC we know that there's no SMP, and hence no spinlock
	// associated with the master kernel lock.
    // This means that we can release the master kernel lock here and
    // still continue to access the thread and context structs, safe
	// in the knowledge that we are still at kernel level and hence
	// cannot be interrupted.

	// Since there's no SMP, the call to TMK_ReleaseMasterKernelLockFromKernelLevel()
	// is only really needed to manage the error-checking state and can be omitted
	// in non-debug builds.
#if !defined(NDEBUG)
    CALL _TMK_ReleaseMasterKernelLockFromKernelLevel(ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;

	// Re-do comparison as JALU status will have been clobbered by the call
	COMP(J21,INTERRUPTED);;
	.align_code 4;
#endif

	IF JEQ, JUMP .RestoreISRContextFromInterruptLevel;; // jump back to reschedule ISR
    
	// The contextLevel was NOTRUN
	//
.firstEntryFromInterruptLevel: // unused label

    // Reset the loop registers
    LC0 = 0;;	
    LC1 = 0;;

    // Get the address of the new thread's run function into RETI
	J0 = [j20 + OFFSETOF(TMK_Thread, pStuff)];;
#ifdef __ADSPTS20x__
	CJMP  = .loopStop2; j4 = j20;;    // pass thread pointer as first argument to run function
#else // TS101
	CJMP  = .loopStop2;;
	j4 = j20;;    // pass thread pointer as first argument to run function
	RETIB = J0;; // anomaly 03000136 workaround
#endif
	RETIB = J0;;

	NOP;;
	NOP;;
	NOP;;
	NOP;;
	
	// Load stack and frame pointers from the context record.
	RTI(ABS)(NP); K27:24 = Q[j22 + LOC(JK27_24)]; J27:24 = Q[k22 + LOC(JK27_24)];;

    // If we got here then the run function returned, this should be a kernel panic.
.loopStop2:
    JUMP .loopStop2;;
    
._tmk_SwitchContext_ISR.end:

#if 0
.align_code 4;
.NoSaveNeeded1:
	JUMP .NoSaveNeeded;;
.align_code 4;
#endif

/*
 * _tmk_RescheduleISR
 *
 * This is the service routine for the kernel interrupt. It is the handler
 * for all pre-emptive activity in the system.
 */
_tmk_RescheduleISR:
	// Anomaly workarounds (which???)
	TS10X(NOP;;NOP;;NOP;;)
	TS20X(NOP;NOP;NOP;NOP;;)

	// We remain at reschedule interrupt level, so we cannot be pre-empted
	// during the context-save sequence.
		
    // Save off some registers so we can have some room to work
	Q[k31 + .J_Temp] = j23:20;;
	Q[j31 + .K_Temp] = k23:20; j23 = j31;;
	k23 = k31;;

	// Get the thread pointer.
	// Read the interrupt return address, clears the global interrupt
	// disable (i.e. the misleadingly-named GIE in SQSTAT) and so
	// re-enables nested interrupts.
	//
    j20 = [k31 + _tmk->pCurrentThread];	k21 = RETIB;;

	// Test that the current thread's contextLevel is NOCONTEXT before
	// saving any context. The only way that contextLevel will *not* be
	// NOCONTEXT is if we are pre-empting a context restore from thread
	// level, after the master kernel lock has been released. This can
	// only happen when switching to a first-time or an interrupted
	// thread, as for a runtime thread the kernel lock will be held until
	// after the thread has been resumed. Similarly, we can never pre-empt
	// a context restore that is happening at kernel level.
	//
	// If we do pre-empt a context restore then that restore sequence is
	// never resumed, instead it is re-started whenever it once again
	// becomes time to return to that thread. In which case the register
	// contents saved into .J_Temp and .K_Temp (and the interrupt return
	// address in k21) are discarded.
	//
	j21 = [j20 + OFFSETOF(TMK_Thread,contextLevel)];;
	COMP(j21,NOCONTEXT);;

	// Get pointers to the context save areas
    j22 = [j20 + OFFSETOF(TMK_Thread, pContextSaveJ)];;
	k22 = [j20 + OFFSETOF(TMK_Thread, pContextSaveK)];;

	IF NJEQ, JUMP .NoSaveNeeded;;

	///////////////////////////////////////////////////////////
	//
	// The original values of the clobbered regs are in: 
	//		.J_Temp	(20-23)
	//		.K_Temp	(20-23)
	//
	///////////////////////////////////////////////////////////

	// Save the interrupt return address
	[j22 + LOC(RETI)] = k21;;

#ifdef __ADSPTS101__
    // Anomaly 03000268 workaround for Nile. This anomaly makes it
	// possible for us to come in here with the GIE bit in IMASKH
	// clear. The only point at which this should be able to happen
	// is in CLI() in Locking.c, after the write to IMASKH but before
	// the reads of IMASKL and the IMASKH shadow. This is the only
	// interruptible point where IMASKH != tmk_TS10xIMASKHShadow.
	// Before we proceed we need to re-normalize things by loading
	// tmk_TS10xIMASKHShadow into IMASKH, which should re-enable
	// interrupts since GIE should be 1 in the IMASKH shadow
	// at this time (if it isn't there's no way we ought to have
	// been interrupted).
	//
	k21 = [j31 + _lck_TS10xIMASKHShadow];;
	[j22 + LOC(IMASK)] = IMASKH;;
	IMASKH = k21;;
#endif

	// X and Y Compute Block Registers
	Q[j22 + LOC(XYR3_0  )] = xR3:0  ;     Q[k22 + LOC(XYR3_0  )] = yR3:0  ;;
	Q[j22 + LOC(XYR7_4  )] = xR7:4  ;     Q[k22 + LOC(XYR7_4  )] = yR7:4  ;;
	Q[j22 + LOC(XYR11_8 )] = xR11:8 ;     Q[k22 + LOC(XYR11_8 )] = yR11:8 ;;
	Q[j22 + LOC(XYR15_12)] = xR15:12;     Q[k22 + LOC(XYR15_12)] = yR15:12;;
	Q[j22 + LOC(XYR19_16)] = xR19:16;     Q[k22 + LOC(XYR19_16)] = yR19:16;;
	Q[j22 + LOC(XYR23_20)] = xR23:20;     Q[k22 + LOC(XYR23_20)] = yR23:20;;
	Q[j22 + LOC(XYR27_24)] = xR27:24;     Q[k22 + LOC(XYR27_24)] = yR27:24;;
	Q[j22 + LOC(XYR31_28)] = xR31:28;     Q[k22 + LOC(XYR31_28)] = yR31:28;;


	// J and K IALU Registers
	Q[J22 + LOC(JK3_0  )] = K3:0  ;       Q[K22 + LOC(JK3_0  )] = J3:0  ;;
	Q[J22 + LOC(JK7_4  )] = K7:4  ;       Q[K22 + LOC(JK7_4  )] = J7:4  ;;
	Q[J22 + LOC(JK11_8 )] = K11:8 ;       Q[K22 + LOC(JK11_8 )] = J11:8 ;;
	Q[J22 + LOC(JK15_12)] = K15:12;       Q[K22 + LOC(JK15_12)] = J15:12;;
	Q[J22 + LOC(JK19_16)] = K19:16;       Q[K22 + LOC(JK19_16)] = J19:16;;
	yR3:0 = Q[J31 + .J_Temp];			  J31 = J23;; // J/KSTAT mustn't now be
	xR3:0 = Q[K31 + .K_Temp];             K31 = K23;; // changed before being saved
	Q[J22 + LOC(JK23_20)] = xR3:0;        Q[K22 + LOC(JK23_20)] = yR3:0;;
	Q[J22 + LOC(JK27_24)] = K27:24;       Q[K22 + LOC(JK27_24)] = J27:24;;
	Q[J22 + LOC(JK31_28)] = K31:28;       Q[K22 + LOC(JK31_28)] = J31:28;;

	// Loop count registers and comp unit status registers
	[j22 + LOC(LC)] = LC0; xR0 = XSTAT;;
	[k22 + LOC(LC)] = LC1; yR0 = YSTAT;;
	[j22 + LOC(STAT)] = xR0;             [K22 + LOC(STAT)] = yR0;;

	// Accumulators, circular buffer regs and return addresses
	xR3:0 = MR3:0;                       yR3:0 = MR3:0;;
	Q[J22 + LOC(L3_0 )] = JL3:0 ;         Q[K22 + LOC(L3_0 )] = KL3:0;;
	Q[J22 + LOC(MR3_0)] = xR3:0 ;         Q[K22 + LOC(MR3_0)] = yR3:0;;
	xR0 = MR4;                           yR0 = MR4;;
	Q[J22 + LOC(B3_0 )] = JB3:0 ;         Q[K22 + LOC(B3_0 )] = KB3:0 ;;
	[J22 + LOC(MR4   )] = xR0;           [K22 + LOC(MR4  )] = yR0;;

	// Return addresses, zero J0 and K0 prior to use saving DABs below
	[J22 + LOC(CJMP)] = CJMP;             K0 = K0 XOR K0;;
	J0 = J0 XOR J0;;

	// DABs
	XR3:0 = DAB Q[J0 += 4];				 YR3:0 = DAB Q[K0 += 4];;
	Q[J22 + LOC(DAB)] = XR3:0;			 Q[K22 + LOC(DAB)] = YR3:0;;					

#ifdef __ADSPTS101__
	// The Parallel Results and ??? registers (SIMD)
	R1:0 = PR1:0;;
	R3:2 = THR1:0;;
	
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	R7:4 = TR3:0;		Q[J22 + LOC(PR1_0_THR1_0)] = xR3:0;	Q[K22 + LOC(PR1_0_THR1_0)]  = yR3:0;;
	R3:0 = TR7:4;		Q[J22 + LOC(TR3_0)]  = xR7:4;		Q[K22 + LOC(TR3_0)]   = yR7:4;;	
	R7:4 = TR11:8;		Q[J22 + LOC(TR7_4)]  = xR3:0;		Q[K22 + LOC(TR7_4)]   = yR3:0;;
	R3:0 = TR15:12;		Q[J22 + LOC(TR11_8)]  = xR7:4;		Q[K22 + LOC(TR11_8)]  = yR7:4;;	
						Q[J22 + LOC(TR15_12)] = xR3:0;		Q[K22 + LOC(TR15_12)] = yR3:0;;
#else  // TS20x
	// Communication Control registers
	xR0 = CMCTL;                    yR0 = CMCTL;;
	[J22 + LOC(CMCTL)] = xR0;        [K22 + LOC(CMCTL)] = yR0;;

	// The Parallel Results and Bit FIFO Overflow registers (SIMD)
	R5:4 = PR1:0;;
	R7:6 = BFOTMP;;
	
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	R3:0 = THR3:0;	    Q[J22 + LOC(PR1_0_BFOTMP)] = xR7:4;	Q[K22 + LOC(PR1_0_BFOTMP)] = yR7:4;;
	R7:4 = TR3:0;		Q[J22 + LOC(THR3_0)] = xR3:0;		Q[K22 + LOC(THR3_0)]  = yR3:0;;
	R3:0 = TR7:4;		Q[J22 + LOC(TR3_0)]  = xR7:4;		Q[K22 + LOC(TR3_0)]   = yR7:4;;	
	R7:4 = TR11:8;		Q[J22 + LOC(TR7_4)]  = xR3:0;		Q[K22 + LOC(TR7_4)]   = yR3:0;;
	R3:0 = TR15:12;		Q[J22 + LOC(TR11_8)]  = xR7:4;		Q[K22 + LOC(TR11_8)]  = yR7:4;;	
	R7:4 = TR19:16;		Q[J22 + LOC(TR15_12)] = xR3:0;		Q[K22 + LOC(TR15_12)] = yR3:0;;
	R3:0 = TR23:20;		Q[J22 + LOC(TR19_16)] = xR7:4;		Q[K22 + LOC(TR19_16)] = yR7:4;;
	R7:4 = TR27:24;		Q[J22 + LOC(TR23_20)] = xR3:0;		Q[K22 + LOC(TR23_20)] = yR3:0;;
	R3:0 = TR31:28;		Q[J22 + LOC(TR27_24)] = xR7:4;		Q[K22 + LOC(TR27_24)] = yR7:4;;
						Q[J22 + LOC(TR31_28)] = xR3:0;		Q[K22 + LOC(TR31_28)] = yR3:0;;
#endif
	[j22 + LOC(SFREG)] = SFREG;;

.NoSaveNeeded:
    // We are not forcing tmk to be in internal memory and CJMP can only
    // be loaded directly if the variable is in internal memory so we use
    // an intermediate register to do the load
	XR0 = [J31 + _tmk->pfDispatchHook];;
	CJMP = XR0;;

    // Setup the runtime model
    // The stack pointers point to the first free quad-word. In addition,
    // each stack must have another 4 words for storing linkage info, and
    // the J stack has 4 more words for copying the 4 argument registers.
    J27 = J31 + (ldf_jstack_base - 12); K26 = 0;;		// 2-word inst
    K27 = K31 + (ldf_kstack_base - 8);  J26 = 0;;		// 2-word inst

    // Reset the loop registers
    LC0 = 0;;	
    LC1 = 0;;

	// Load this constant into a preserved reg here to avoid stalls when we
	// need it after the call, below.
    xR24 = NOCONTEXT;;

    // Call the Dispatcher. Since this is the only such call it
    // (initially) looks as if we could consider expanding it out
    // in here as hand-written assembly. We would, however, still
    // have to save all the registers and set up the C runtime (i.e.
    // all the above work) in order to be able to invoke the DPCs,
	// so any time saving would be small. (We should never come in
	// here unless there is at least one DPC queued.)
	//
	// was    CALL _tmk_Dispatcher(ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;
    CJMP_CALL (ABS); Q[J27 + 4] = J27:24; Q[k27 + 4] = K27:24;;

    // We have blown through the dispatcher without restoring the
    // running thread's context, so we need to do that now. 
	// We have the current thread pointer and the context pointers in
	// preserved registers so they have not been changed by
	// the call to _tmk_Dispatcher. Also, we didn't change the 
	// contextLevel field when we saved the context but we *do*
	// need to set it to NOCONTEXT now, because we may have
	// pre-empted the restore of an interrupted thread from runtime level.
	//
    [J20 + OFFSETOF(TMK_Thread,contextLevel)] = xR24;;

    // If we've fallen through the dispatcher then we need to restore the context
    // of the thread we've interrupted.
.RestoreISRContextFromInterruptLevel:
    // If we're here then we're restoring an interrupted thread, i.e.
    // the entire register set. We may have got here either from
	// RescheduleISREntryPoint, or by falling through from RestoreContext
    // above.
    // In either case:
    // - J20 currently contains the thread structure pointer
    // - J22 and K22 currently contain the thread context pointers
    // - the thread's m_ContextLevel field has been set to NOCONTEXT
    // - we have a C runtime environment

	xR0 = [J22 + LOC(SFREG)];;
    SFREG = xR0;;

#ifdef __ADSPTS101__
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	xR7:4 = Q[J22 + LOC(TR3_0)];     yR7:4 = Q[K22 + LOC(TR3_0)];;
	xR3:0 = Q[J22 + LOC(TR7_4)];     yR3:0 = Q[K22 + LOC(TR7_4)];	TR3:0   = R7:4;;
	xR7:4 = Q[J22 + LOC(TR11_8)];    yR7:4 = Q[K22 + LOC(TR11_8)];	TR7:4   = R3:0;;
	xR3:0 = Q[J22 + LOC(TR15_12)];   yR3:0 = Q[K22 + LOC(TR15_12)];	TR11:8  = R7:4;;
	xR7:4 = Q[J22 + LOC(PR1_0_THR1_0)]; yR7:4 = Q[K22 + LOC(PR1_0_THR1_0)];	TR15:12 = R3:0;;

	// The Parallel Results and ??? registers (SIMD)
	PR1:0  = R5:4;;
	THR1:0 = R7:6;;
#else  // TS20x
	// "Enhanced Communication" registers (a.k.a. Viterbi/Trellis accelerators)								  
	// The register-register transfers are SIMD and execute on both X and Y together
	xR3:0 = Q[J22 + LOC(THR3_0)];	 yR3:0 = Q[K22 + LOC(THR3_0)];;
	xR7:4 = Q[J22 + LOC(TR3_0)];     yR7:4 = Q[K22 + LOC(TR3_0)];	THR3:0  = R3:0;;
	xR3:0 = Q[J22 + LOC(TR7_4)];     yR3:0 = Q[K22 + LOC(TR7_4)];	TR3:0   = R7:4;;
	xR7:4 = Q[J22 + LOC(TR11_8)];    yR7:4 = Q[K22 + LOC(TR11_8)];	TR7:4   = R3:0;;
	xR3:0 = Q[J22 + LOC(TR15_12)];   yR3:0 = Q[K22 + LOC(TR15_12)];	TR11:8  = R7:4;;
	xR7:4 = Q[J22 + LOC(TR19_16)];   yR7:4 = Q[K22 + LOC(TR19_16)];	TR15:12 = R3:0;;
	xR3:0 = Q[J22 + LOC(TR23_20)];   yR3:0 = Q[K22 + LOC(TR23_20)];	TR19:16 = R7:4;;
	xR7:4 = Q[J22 + LOC(TR27_24)];   yR7:4 = Q[K22 + LOC(TR27_24)];	TR23:20 = R3:0;;
	xR3:0 = Q[J22 + LOC(TR31_28)];   yR3:0 = Q[K22 + LOC(TR31_28)];	TR27:24 = R7:4;;
	xR7:4 = Q[J22 + LOC(PR1_0_BFOTMP)]; yR7:4 = Q[K22 + LOC(PR1_0_BFOTMP)];	TR31:28 = R3:0;;

	// The Parallel Results, Bit FIFO Overflow registers and
	// communication Control registers.
	PR1:0  = R5:4;;
	BFOTMP = R7:6;	xR0 = [J22 + LOC(CMCTL)];        yR0 = [K22 + LOC(CMCTL)];;
	CMCTL = R0;;
#endif
	// DABs
	J0 = J22 + LOC(DAB);					 K0 = K22 + LOC(DAB);;
	XR3:0 = DAB Q[J0 += 4];				 YR3:0 = DAB Q[K0 += 4];;

	// Accumulators, circular buffer regs and return addresses
	xR3:0 = Q[j22 + LOC(MR3_0)];          yR3:0 = Q[k22 + LOC(MR3_0)];;
	xMR3:2 = R3:2;                       yMR3:2 = R3:2;;
	xMR1:0 = R1:0;                       yMR1:0 = R1:0;;
	xR0   = [j22 + LOC(MR4)];            yR0   = [k22 + LOC(MR4)];;
	xMR4 = R0;                           yMR4 = R0;;
	JL3:0 = Q[j22 + LOC(L3_0 )];          KL3:0 = Q[k22 + LOC(L3_0 )];;
	JB3:0 = Q[j22 + LOC(B3_0 )];          KB3:0 = Q[k22 + LOC(B3_0 )];;

	xR0  = [j22 + LOC(CJMP)];;
	CJMP  = xR0;;
	
#ifdef __ADSPTS101__
    // Anomaly 03000268 workaround for Nile. If the GIE bit in
	// the saved IMASKH is zero then this indicates that the thread
	// we are returning to was interrupted just *after* clearing
	// GIE. In this case (only) we need to restore what was in
	// IMASKH at the time of the interrupt.
	//
	YR5 = [J22 + LOC(IMASK)];;
	YBITEST R5 BY INT_GIE_P;;
	IF NYSEQ, JUMP .skip2;;  // if GIE bit set then do nothing

	// The GIE bit was clear at the time this thread was pre-empted, so
	// we need to reload the saved value of IMASKH.
	IMASKH = YR5;;

.skip2:
#endif

	// Compute Block Status registers
	xR0 = [j22 + LOC(STAT)];             yR0 = [k22 + LOC(STAT)];;
	XSTAT = R0;                          YSTAT = R0;;

	// Zero-overhead Loop Registers
	xR0 = [j22 + LOC(LC)];				  yR0 = [k22 + LOC(LC)];;
	LC0 = xR0;;
	LC1 = yR0;;

	// X and Y Compute Block Registers
	xR3:0   = Q[j22 + LOC(XYR3_0  )];     yR3:0   = Q[k22 + LOC(XYR3_0  )];;
	xR7:4   = Q[j22 + LOC(XYR7_4  )];     yR7:4   = Q[k22 + LOC(XYR7_4  )];;
	xR11:8  = Q[j22 + LOC(XYR11_8 )];     yR11:8  = Q[k22 + LOC(XYR11_8 )];;
	xR15:12 = Q[j22 + LOC(XYR15_12)];     yR15:12 = Q[k22 + LOC(XYR15_12)];;
	xR19:16 = Q[j22 + LOC(XYR19_16)];     yR19:16 = Q[k22 + LOC(XYR19_16)];;
	xR23:20 = Q[j22 + LOC(XYR23_20)];     yR23:20 = Q[k22 + LOC(XYR23_20)];;
	xR27:24 = Q[j22 + LOC(XYR27_24)];     yR27:24 = Q[k22 + LOC(XYR27_24)];;
	xR31:28 = Q[j22 + LOC(XYR31_28)];     yR31:28 = Q[k22 + LOC(XYR31_28)];;

	// J and K IALU Registers
	K3:0   = Q[j22 + LOC(JK3_0  )];      J3:0   = Q[k22 + LOC(JK3_0  )];;
	K7:4   = Q[j22 + LOC(JK7_4  )];      J7:4   = Q[k22 + LOC(JK7_4  )];;
	K11:8  = Q[j22 + LOC(JK11_8 )];      J11:8  = Q[k22 + LOC(JK11_8 )];;
	K15:12 = Q[j22 + LOC(JK15_12)];      J15:12 = Q[k22 + LOC(JK15_12)];;
	K19:16 = Q[j22 + LOC(JK19_16)];      J19:16 = Q[k22 + LOC(JK19_16)];;

	K27:24 = Q[j22 + LOC(JK27_24)];      J27:24 = Q[k22 + LOC(JK27_24)];;
	K31:28 = Q[j22 + LOC(JK31_28)];      J31:28 = Q[k22 + LOC(JK31_28)];;

    // Load the return-from-interrupt address into the RETI register (using the
	// RETIB alias to globally disable interrupts) as we are going to use the
	// RTI intruction to get back to runtime level.
    K21 = [j22 + LOC(RETI)];;
    RETIB = K21;;
    TS10X(RETIB = K21;;) // anomaly 03000136 workaround

	NOP;;
	NOP;;
	NOP;;
	NOP;;

    // We're done, resume the thread
    RTI (NP)(ABS); K23:20 = Q[j22 + LOC(JK23_20)]; J23:20 = Q[k22 + LOC(JK23_20)];;
    
._tmk_RescheduleISR.end:


_tmk_RunOnSystemStack:
    // Set the stack to the (shared) system area.
    // The stack pointers point to the first free quad-word. In addition,
    // each stack must have another 4 words for storing linkage info, and
    // the J stack has 4 more words for copying the 4 argument registers.
	//
    J27 = J31 + (ldf_jstack_base - 12); K26 = 0;;		// 2-word inst
    K27 = K31 + (ldf_kstack_base - 8);  J26 = 0;;		// 2-word inst

    CJMP = J6;; // third argument is function ptr

	// First and second args (old and new thread ptrs) remain in J4 and J5.
	CJMP (ABS);;
        
._tmk_RunOnSystemStack.end:


_tmk_SwitchContext_KillThread:

    xR3 = RUNTIME;;
    xR2 = NOCONTEXT;;
	xR1 = NOTRUN;;

	JUMP .RestoreAfterKill;;

._tmk_SwitchContext_KillThread.end:


