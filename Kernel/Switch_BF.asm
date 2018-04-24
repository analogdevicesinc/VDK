// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include "Trinity.h"
#include "TMK_warning_removal.h"

.file_attr ISR;
.file_attr OS_Internals;

.IMPORT "Trinity.h";

// Disable warning for anomaly 05000312 when we deal with LC0 and LC1. Our code
// is OK but TAR40686 prevents us from disabling the warning in the location
// where it is needed (we need to do it globally)
.message/suppress 5515; 

// Disable warning for anomaly 05000165 for BF535 
// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
// we have not checked the code yet so we make it informational for now
.message/info 5504; 

#if defined (__ADSPBF561__)
    // This bogus MMR read to work around 05000283 will cause a 05000428
    // warning. The warning is not valid as the read is from MMR space
    // and not from L2 memory.
	// Ideally we would only do this in the line in question but asm TAR40686
	// prevents us from doing that
.MESSAGE/SUPPRESS 5517 ;  //Suppress 05000428 warning
#endif
.EXTERN STRUCT tmk_Globals _tmk;

#if defined (BUILD_ROM_IMAGE)
// in delta we do not want to reference the stack end directly but via a 
// variable that can be resolved to a particular address
.EXTERN _tmk_SystemStackEnd;
#else
.EXTERN ldf_stack_end;
.EXTERN ldf_stack_space;
.EXTERN ___adi_stack_bounds;
#endif

.SECTION program;
.ALIGN 2;

// Functions used by this module
.EXTERN _TMK_ReleaseMasterKernelLock;
.EXTERN _TMK_ReleaseMasterKernelLockFromKernelLevel;
.EXTERN _tmk_MaskMasterKernelLockForThreadLevel;
.EXTERN _tmk_Dispatcher;

#if defined (BUILD_ROM_IMAGE) 
.EXTERN _tmk_pUserExceptionHandler;
#else
.EXTERN UserExceptionHandler;
#endif

// Globals used by this module

// On delta (both with utility ROM or not) because of the lack of external 
// memory there is no point on calling the cplb manager and pull in extra code
// that is not required
#ifndef __ADSPBF592A__
.EXTERN _cplb_hdr;
#endif

// Functions exported by this module
.GLOBAL _tmk_RescheduleISR;
.GLOBAL _tmk_SwitchContext_Runtime;
.GLOBAL _tmk_SwitchContext_FirstThread;
.GLOBAL _tmk_SwitchContext_ISR;
.GLOBAL _tmk_ExceptionHandler;
.GLOBAL _tmk_RunOnSystemStack;
.GLOBAL _tmk_SwitchContext_KillThread;
.GLOBAL TMK_ExceptionRestoreEntry;

#define EXCEPTION_NUM 0

// Enable the support for self-nested interrupts
#define SELF_NESTING

_tmk_SwitchContext_FirstThread:
    P5 = R1;  // new thread ptr passed as 2nd argument, 1st is dummy
    R3 = NOCONTEXT;
	// Disable warning for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	.message/suppress 5504 for 1 lines; 
    B[P5 + OFFSETOF(TMK_Thread,contextLevel)] = R3; // set contextLevel to NOCONTEXT
 
	// Load the stack pointer. We do this before .firstEntryFromThreadLevel since
	// if we jump directly to that symbol (from _tmk_SwitchContext_Runtime) then
	// the SP will already have been loaded.
	//
#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Store the location of '___adi_stack_bounds'
    P0.L = ___adi_stack_bounds;
    P0.H = ___adi_stack_bounds;

    // Obtain the address of the new thread's stack base
    R0 = [P5 + OFFSETOF(TMK_Thread,pStackBase)];

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    [P0] = R0;

    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    STI R1;
#else
    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)]; // load stack pointer
    NOP; // to avoid anomaly 05-00-0227
#endif
 
.firstEntryFromThreadLevel:
    // Get the address of the new thread's run function into
    // a preserved register, so it doesn't get trashed by the
    // call to TMK_ReleaseMasterKernelLock().
    P3 = [P5 + OFFSETOF(TMK_Thread,pStuff)];       // get run function ptr
    FP = 0;                              // terminate debugger stack traces

	// Reserve space on stack for argument saves by the called function.
	// We move the stack pointer by two words even though we're reserving
	// three words on the stack. This is because the initial SP that we get
	// passed in pContextSave, above, is the address of the highest word
	// *within* the stack region, whereas BlackFin uses an "empty" stack (pre-
	// decrement, post-increment) so that there is in effect already one word
	// reserved on the stack.
	//
	SP += -8;
    
    // Release the kernel lock before running the thread. We must
	// have finished accessing the thread struct by this point.
    CALL.X _TMK_ReleaseMasterKernelLock;

    R0 = P5;   // pass thread pointer as first argument to run function
    CALL(P3);

    // If we got here then the run function returned, this will be a kernel panic.
.loopStop1:
    JUMP .loopStop1;


/*
tmk_SwitchContext_Runtime
C-callable entrypoint which is called, via a function pointer,
from tmk_ScheduleThread() only. UNTRUE

When we come in here we know that:
- we're running at thread level
- we're holding the kernel lock, acquired by thread code
- the old thread ptr is in R0 (first argument)
- the new thread ptr is in R1 (second argument)

Return type: void
*/
_tmk_SwitchContext_Runtime:
    // Save the (nine) preserved registers
	// Disable warning for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	.message/suppress 5504 for 3 lines; 
    [--SP] = (R7:4, P5:3);  // 7 registers
    [--SP] = FP;
    [--SP] = RETS;
    
    // Work in scratch registers only until we've saved the preserved regs
    P4 = R0;    // old thread ptr (first argument)
    P5 = R1;	// new thread ptr (second argument)
    R2 = RUNTIME;
    R3 = NOCONTEXT;

    // Save the stack pointer in the pContextSave field
    // of the thread structure.
    [P4 + OFFSETOF(TMK_Thread,pContextSave)] = SP;

    // Mark the old thread as containing the context of
    // a "runtime" (i.e. voluntarily context-switched) thread.
    B[P4 + OFFSETOF(TMK_Thread,contextLevel)] = R2;

.RestoreAfterKill:
    // Before we load the new thread's stack pointer, we will do a dummy read
    // from this stack.This will force the CPLB handler to cover the stack memory
    // with a valid CPLB before switching it in if this is required. This can
    // prevent an unrecoverable double exception.
    // This is a fix for TAR32922
    P0 = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    P0 = [P0];
	NOP; // to avoid anomaly 05-00-0227

#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Store the location of '___adi_stack_bounds'
    P0.L = ___adi_stack_bounds;
    P0.H = ___adi_stack_bounds;

    // Obtain the address of the new thread's stack base
    R0 = [P5 + OFFSETOF(TMK_Thread,pStackBase)];

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    [P0] = R0;

    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    STI R1;
#else
    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    NOP; // to avoid anomaly 05-00-0227
#endif

    // First we do a three-way branch according to the
    // ContextLevel of the thread:
    // - NOTRUN: goes to .firstEntryFromThreadLevel
    // - INTERRUPTED: goes to .RestoreISRContextFromThreadLevel
    // - RUNTIME: falls though to .RestoreRuntimeContextFromThreadLevel
    //
    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP.

    // Get the thread's variable that says what type of context is stored...
    R2 = B[P5 + OFFSETOF(TMK_Thread,contextLevel)](Z);
	// Disable warning for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	.message/suppress 5504 for 1 lines; 
    B[P5 + OFFSETOF(TMK_Thread,contextLevel)] = R3;     // ...and set it to NOCONTEXT

    // Now we fan-out to the different sorts of context
    CC = R2 == RUNTIME;
    IF !CC JUMP .notRuntime_Thread;
    ANOM05000428_3NOPS
    
.RestoreRuntimeContextFromThreadLevel: // label not used
	// If we're here then we're doing a voluntary runtime-to-
	// -runtime context switch, e.g. a thread yield or posting
	// a message or semaphore to a higher-priority pending
	// thread. This is one of the cases that we want to run
	// as fast as possible so we want it on the shortest code path.
    //
	// We know that we're returning to a C runtime, and hence that
    // the return PC is in the (saved) RETS register
    // on the top of the stack.
    RETS         = [SP++];
    FP           = [SP++];
    (R7:4, P5:3) = [SP++];
    RTS;                      // back to the (new) thread
    
    
.notRuntime_Thread:
    // We could (possibly) release the master kernel lock here,
    // but we don't at the moment. Instead it gets released
    // separately on each of the two code paths (interrupt and
    // first-time).
    
    CC = R2 == NOTRUN;
    IF CC JUMP .firstEntryFromThreadLevel;
    ANOM05000428_3NOPS
    
.RestoreISRContextFromThreadLevel: // label not used
    // Restore the frame pointer register. We do this now as we have
	// already loaded the destination thread's SP, and  we want both
	// SP and FP to refer to the same stack when we make the call to
	// TMK_ReleaseMasterKernelLock() from the exception handler, below.
	//
    FP = [SP++];

    // Restore the DSP registers
    A1.X = [SP++];
    A1.W = [SP++];
    A0.X = [SP++];
    A0.W = [SP++];
    I0   = [SP++];
    I1   = [SP++];
    I2   = [SP++];
    I3   = [SP++];
    B0   = [SP++];
    B1   = [SP++];
    B2   = [SP++];
    B3   = [SP++];
    L0   = [SP++];
    L1   = [SP++];
    L2   = [SP++];
    L3   = [SP++];
    M0   = [SP++];
    M1   = [SP++];
    M2   = [SP++];
    M3   = [SP++];

    // We're switching to an interrupted thread, so we have to get
    // to a higher level first. We're using exception level for this.
.RaiseException:
    EXCPT EXCEPTION_NUM;

    // If an NMI occurs at the wrong point wrt. the instruction above then
    // we may fail to enter the exception handler and end up here. To deal
    // with this we just loop and retry raising the exception.
    JUMP .RaiseException;

._tmk_SwitchContext_FirstThread.end:
._tmk_SwitchContext_Runtime.end:

.section L1_code;
.align 2;
_tmk_ExceptionHandler:    
    // Is the exception a USER DEFINED EXCPT exception? And if so, is it
    // the VDK exception?
#if defined (__SILICON_REVISION__) && defined(__ADSPBF535__)
	// Add SSYNC as first line of interrupts for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	SSYNC;
#endif
    [--SP] = ASTAT;  // save off ASTAT and R0 so we can look at the
    [--SP] = R0;     // cause of the exception.

#if defined (__SILICON_REVISION__) && !(defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
	[--SP] = P3;
	// Workaround for anomaly 05-00-0283. 
    // A system MMR write is stalled indefinitely when killed in a particular 
    // stage.
    // The workaround is to reset the MMR logic with another killed MMR
    // access that does not have other side effects on the application
    // No breakpoints should be placed before the wa_05000283_skip2 label
    // This anomaly affects all silicon revisions to date (0.1 0.2 0.3 0.4).
	//
	// P3, R0, and ASTAT must have been saved prior to this point.
	//
	CC = R0 == R0; // always true
	P3.H = 0XFFC0; // MMR space CHIPID
	P3.L = 0X0014;
	IF CC JUMP .wa_05000283_skip2; // always skip MMR access, however, MMR access will be fetched and killed
	ANOM05000428_SUPPRESS_MMR_READ  //MMR is not in L2 so will not hit 05-00-0428 
	R0 = [P3]; // bogus MMR read to work around the anomaly
	ANOM05000428_2NOPS // Only two NOPS required to avoid 05-00-0428
	
.wa_05000283_skip2:  // continue with ISR
	P3 = [SP++];
#endif

#if defined (__SILICON_REVISION__) && !(defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
    // Workaround for anomaly 05-00-0257. Reload the loop counter registers to force the clearing
    // of the loop buffers, and hence prevent the wrong fetch address from being sent to the
    // instruction fetch unit if returning into a short hardware loop.
    R0=LC0;
    LC0=R0;
    R0=LC1;
    LC1=R0;
#endif

    R0 = SEQSTAT; // Get the sequencer status
    R0 <<= 26;    // and extract the exception cause field(s).
    R0 >>= 26;    // >> is logical shift, >>> is arithmetic
    CC = R0 == EXCEPTION_NUM;
    IF !CC JUMP .notReturnFromRestore;

.returnFromRestore:  // label not used
    SP += 8;  // discard the 2 register values that were pushed on
	          // entry to the exception handler (might be faster just to pop).

	// This entrypoint can be used if the user application implements its own
	// exception handler. 
TMK_ExceptionRestoreEntry:

	// Release the kernel lock before returning to the thread.
    // This lock was acquired by the (previous) thread code prior
    // to calling the scheduler. We must have finished accessing
	// the thread struct by this point.
	//
	// We cannot permit the reschedule ISR to be entered again until
	// we have popped *all* of the saved context off the thread's stack,
	// otherwise we would face theoretically unbounded stack usage in
	// the pathological case.
	//
	// So we have to wait until we're inside the exception handler before releasing
	// the kernel lock (i.e. unmasking the reschedule interrupt) so that exception
	// level will hold off any pre-emption until RTX is executed.
	//
	// We also cannot restore any of the scratch registers until after we have made
	// the call, so all of the restores have to be done inside the exception handler
	// too. This also means that the 8-byte stack usage of the exception handler is
	// in addition to the space needed for the context save. There is a a partial
	// solution for this which involves using #pragma regs_clobbered to restrict the
	// clobber set of TMK_ReleaseMasterKernelLock() while still being compatible
	// with the C/C++ calling conventions. This fix is in the AOS variant of TMK
	// and should be merged into the mainline as soon as possible.
	//
    CALL.X _TMK_ReleaseMasterKernelLock;
	
    // If we're here then we're restoring an interrupted thread, i.e.
    // the entire register set.
    // - The destination thread's stack has been reloaded
    // - the thread's contextLevel field has been set to NOCONTEXT

    // Restore the subroutine return address register
    RETS = [SP++];
 
    // Load the return-from-interrupt address into the RETX register,
    // as we are going to use the RTX instruction to get back to runtime level.
    RETX = [SP++];

    // Restore the loop registers, we must do this with interrupts disabled,
	// due to anomaly 05-00-0312.
    LB1 = [SP++];
    LT1 = [SP++];
    LC1 = [SP++];
    LB0 = [SP++];
    LT0 = [SP++];
    LC0 = [SP++];

    // Restore ASTAT
    ASTAT = [SP++];

    // Restore the data and pointer registers
    (R7:0, P5:0) = [SP++];

    // We're done, resume the thread
    RTX;
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	CSYNC;
#endif

.notReturnFromRestore:
	// It wasn't the VDK exception, so now we need to check for CPLB miss
	// exceptions. Check instruction CPLB miss code first.
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	SSYNC;
#endif

// On delta we do not call the cplb manager so we don't need to inspect 
// EXCAUSE any further before we go to UserExceptionHandler
#ifndef __ADSPBF592A__
	[--SP] = R1;
	R1 = 0x2C;

	CC = R0 == R1; 
	IF CC JUMP .is_icplb_miss;

	// Check data CPLB miss code next.
	R1 = 0x26;
	CC = R0 == R1;
	IF CC JUMP .is_dcplb_miss;
	R1 = 0x23; // Data access CPLB protection violation
	CC = R0 == R1;
	IF CC JUMP .is_dcplb_violation;
	ANOM05000428_3NOPS 
	R1 = [SP++];
#endif
	R0 = [SP++];
	ASTAT = [SP++];
    // Jump to the user defined exception handler....
#if defined (BUILD_ROM_IMAGE)
// for the delta ROM we want to go to a label which is going to be fixed size 
// rather than the UserExceptionHandler which is likely to change in size. 
// The fixed size thunk restores P5 to its original value so we can trash it
// here
	[--SP] = P5;
    P5.L = LO(_tmk_pUserExceptionHandler);
    P5.H = HI(_tmk_pUserExceptionHandler);
    P5 =[P5];
    JUMP (P5);// we do not do JUMP.X because that trashes P1 
				// and we'd rather have a linker error than
				// weird behaviour in the app.
#else
    JUMP UserExceptionHandler;  // we do not do JUMP.X because that trashes P1 
				// and we'd rather have a linker error than
				// weird behaviour in the app.
#endif

// On delta we do not call the cplb manager so we don't need these labels
#ifndef __ADSPBF592A__
.is_dcplb_violation:
.is_dcplb_miss:
.is_icplb_miss:
// we restore the registers we used and go to the RTL way of handling things
	R1 = [SP++];
	R0 = [SP++];
	ASTAT = [SP++];
// we cannot restore P1 so we cannot do a JUMP.X
	JUMP _cplb_hdr;
#endif

.TMK_ExceptionRestoreEntry.end:
._tmk_ExceptionHandler.end:


.section program;
.align 2;

/*
tmk_SwitchContext_ISR
C-callable entrypoint which is called, via a function pointer,
from tmk_Dispatcher() only.

When we come in here we know that:
- we're running at kernel level, still at reschedule interrupt level
- we're holding the kernel lock, acquired by tmk_Dispatcher()
- the old thread ptr is in R0 (first argument)
- the new thread ptr is in R1 (second argument)

Return type: void
*/
_tmk_SwitchContext_ISR:
    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    P4 = R0;    // old thread ptr (first argument)
    P5 = R1;	// new thread ptr (second argument)
    R2 = INTERRUPTED;
    R3 = NOCONTEXT;
    
    // The entire register set has already been saved on entry
    // to the reschedule ISR, so all we have to do now is
    // mark the old thread as containing the context
    // of an interrupted (i.e. pre-empted) thread.
	// Disable warning for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	//.message/suppress 5504 for 1 lines; 
    B[P4 + OFFSETOF(TMK_Thread,contextLevel)] = R2;

    // First we do a three-way branch according to the
    // ContextLevel of the thread:
    // - NOTRUN: goes to .firstEntry, below
    // - INTERRUPTED: not handled yet
    // - RUNTIME: falls though to RTS, below
    //
    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP.

    // Get the thread's variable that says what type of context is stored...
    // (use a preserved reg so it won't be trashed by the call
    // to TMK_ReleaseMasterKernelLock()).
    R7 = B[P5 + OFFSETOF(TMK_Thread,contextLevel)](Z);
    B[P5 + OFFSETOF(TMK_Thread,contextLevel)] = R3;     // ...and set it to NOCONTEXT

    // Now we fan-out to the different sorts of context
    //
    CC = R7 == RUNTIME;
    IF !CC JUMP .notRuntime_ISR;
    ANOM05000428_3NOPS 
    
.RestoreRuntimeContextFromInterruptLevel:  // label not used
#if !defined(SELF_NESTING) || !defined(NDEBUG)
	// For self-nesting support IVG14 will already be masked,
	// so we don't need to do it here except for checking.
	//
	CALL.X _tmk_MaskMasterKernelLockForThreadLevel;
#endif 

    // Before we load the new thread's stack pointer, we will do a dummy read
    // from this stack.This will force the CPLB handler to cover the stack memory
    // with a valid CPLB before switching it in if this is required. This can
    // prevent an unrecoverable double exception.
    // This is a fix for TAR32922
    P0 = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    P0 = [P0];

#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Load the address of '___adi_stack_bounds'
    P0.L = ___adi_stack_bounds;
    P0.H = ___adi_stack_bounds;

    // Obtain the address of the new thread's stack base
    R0 = [P5 + OFFSETOF(TMK_Thread,pStackBase)];

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    [P0] = R0;

    // Load the stack pointer (other regs are saved on the stack).
    // We don't want to do this until after the last call (above) so
    // unfortunately we can't move the SP load any futher upstream.
    // It's ok to load from the thread struct here as we haven't
    // released the master kernel lock yet.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    STI R1;
#else
	// Load the stack pointer (other regs are saved on the stack).
    // We don't want to do this until after the last call (above) so
	// unfortunately we can't move the SP load any futher upstream.
	// It's ok to load from the thread struct here as we haven't
	// released the master kernel lock yet.
	SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
#endif
		
	// If we're here then we're unblocking a thread which
	// is pending on a signal from an ISR. This is the most
	// time-critical case so we want it on the shortest code path.
    //

#ifdef SELF_NESTING
	// If a DPC has called TMK_AcquireMasterKernelLock() then the previous
	// value of tmk.masterState will have been overwritten with one that
	// has IVG14 unset. This will cause a failure when this value is re-loaded
	// on return to the runtime context. To avoid this we update the saved
	// value prior to returning.
	//
    P5.L = LO(_tmk);
    P5.H = HI(_tmk);
	R7 = [P5 + OFFSETOF(_tmk, masterState)];
	BITSET (R7, 14); // unmask IVG14 in the saved IMASK
// Disable warning for anomaly 05000165 for BF535 
// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
//.message/suppress 5504 for 1 lines; 
	[P5 + OFFSETOF(_tmk, masterState)] = R7;
#endif

	// We know that we're returning to a C runtime, so
    // the return PC is in the (saved) RETS register
    // on the top of the stack.
    RETI         = [SP++];  // get the return PC into RETI
    FP           = [SP++];
    (R7:4, P5:3) = [SP++];
    RTI;                      // back to the (new) thread
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	CSYNC;
#endif


.notRuntime_ISR:
#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Load the address of '___adi_stack_bounds'
    P0.L = ___adi_stack_bounds;
    P0.H = ___adi_stack_bounds;

    // Obtain the address of the new thread's stack base
    R0 = [P5 + OFFSETOF(TMK_Thread,pStackBase)];

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    [P0] = R0;

    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    // Ideally we'd wait until we've released the master kernel lock
    // (so that the release call runs on the system stack) but it's not
    // safe to access the thread struct unless we're still holding the lock.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    STI R1;
#else
    // Load the stack pointer (other regs are saved on the stack).
    // We do this here to avoid stalls when we start to pop the stack.
    // Ideally we'd wait until we've released the master kernel lock
	// (so that the release call runs on the system stack) but it's not
	// safe to access the thread struct unless we're still holding the lock.
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
#endif

	// We have finished accessing the thread structure, all
    // further activity in here will be on the stack.
    // This means that we can release the master kernel lock here,
    // except for the runtime case (which is why we branched above).
	//
	// Only really needed for SMP, and to manage the error-checking state.
	// For non-debug, non-SMP builds this call is a no-op and can be omitted.
	//
#if !defined(NDEBUG) || (defined(NUM_CORES) && (NUM_CORES > 1))
    CALL.X _TMK_ReleaseMasterKernelLockFromKernelLevel; // only needed for SMP
#endif

    CC = R7 == INTERRUPTED;
    IF CC JUMP .RestoreISRContextFromInterruptLevel(BP);
    
// if this label was ever to be used then we need to checking the code
// for anomaly 05000165 as it may no longer be safe
.firstEntryFromInterruptLevel: // unused label
    // Get the address of the new thread's run function into P5
    P3 = [P5 + OFFSETOF(TMK_Thread,pStuff)]; // from thread-specific data

    FP = 0;                              // terminate debugger stack traces

	// Reserve space on stack for argument saves by the called function.
	// We move the stack pointer by two words even though we're reserving
	// three words on the stack. This is because the initial SP that we get
	// passed in pContextSave, above, is the address of the highest word
	// *within* the stack region, whereas BlackFin uses an "empty" stack (pre-
	// decrement, post-increment) so that there is in effect already one word
	// reserved on the stack.
	//
	SP += -8;
    
    R0 = P5;      // pass thread pointer as first argument to run function
// Disable warning for anomaly 05000165 for BF535 
// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
//.message/suppress 5504 for 1 lines; 
	[--SP] = P3;  // push the destination address onto the stack
    CALL .doRTI;  // do this just to get a valid return address into RETS

    // If we got here then the run function returned, this will be a kernel panic.
.loopStop2:
    JUMP .loopStop2;
    
.doRTI:
	RETI = [SP++];  // disables interrupts

#ifdef SELF_NESTING
	// For self-nesting support, IVG14 will have been cleared in IMASK on entry
	// to kernel level, so we need to set it here.
	//
	CLI R7;
	BITSET (R7, 14); // unmask IVG14
	STI R7;
#endif

	RTI; 
#if defined (__SILICON_REVISION__) && defined (__ADSPBF535__)
	// Workaround for anomaly 05000165 for BF535 
	// "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress 
	CSYNC;
#endif


/*
 * _tmk_RescheduleISR
 *
 * This is the service routine for the kernel interrupt. It is the handler
 * for all pre-emptive activity in the system.
 */
_tmk_RescheduleISR:
#if defined (__SILICON_REVISION__) && (defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
    SSYNC;
#endif
    // Save off these registers first so we can do some trampling
    [--SP] = (R7:0, P5:0);
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


	R7 = [P3]; // bogus MMR read to work around the anomaly
	
.wa_05000283_skip:  // continue with ISR
#endif

#ifdef SELF_NESTING
	// To support self-nested interrupts (i.e. SYSGFG->SNEN = 1) we need to
	// explicitly mask out the reschedule interrupt, as the hardware won't hold
	// it off during execution of the ISR. We have to do this before saving RETI.
	//
	CLI R0;
	BITCLR (R0, 14); // mask out IVG14
	STI R0;	
#endif
    
    R0 = 0; // used below

    // Save off the loop registers
    [--SP] = LC0;
    [--SP] = LT0;
    [--SP] = LB0;
    [--SP] = LC1;
    [--SP] = LT1;
    [--SP] = LB1;

    // Reset the loop counters. We have to do this before re-enabling interrupts,
	// due to anomaly 05-00-0312.
    LC0 = R0;
    LC1 = R0;

    // Save off the RETI register, re-enables nested interrupts
    [--SP] = RETI;

    // Get the current thread pointer into P5, used below
    P5.L = LO(_tmk);
    P5.H = HI(_tmk);
#if defined (BUILD_ROM_IMAGE)
    // for delta ROM we need to load a variable which contains the address of 
    //the stack end. For other processors we can use the ldf variable directly
	P2.L = LO(_tmk_SystemStackEnd);
	P2.H = HI(_tmk_SystemStackEnd);
#endif

    P1 = [P5 + OFFSETOF(_tmk, pfDispatchHook)];  // load the address of the dispatch function
    P5 = [P5 + OFFSETOF(_tmk, pCurrentThread)];  // finish loading the current thread pointer

    // Save off system wide registers
    [--SP] = RETS;

    // Save off the DSP registers
    [--SP] = M3  ;
    [--SP] = M2  ;
    [--SP] = M1  ;
    [--SP] = M0  ;
    [--SP] = L3  ;
    [--SP] = L2  ;
    [--SP] = L1  ;
    [--SP] = L0  ;
    [--SP] = B3  ;
    [--SP] = B2  ;
    [--SP] = B1  ;
    [--SP] = B0  ;
    [--SP] = I3  ;
    [--SP] = I2  ;
    [--SP] = I1  ;
    [--SP] = I0  ;
    [--SP] = A0.W;
    [--SP] = A0.X;
    [--SP] = A1.W;
    [--SP] = A1.X;

    // Save off the stack frame pointer
    [--SP] = FP;

	// Store the SP in the thread structure
    [P5 + OFFSETOF(TMK_Thread,pContextSave)] = SP;

    // We've saved off the context, we need to setup the C runtime, and
    // call the scheduler

    // Set the stack to the (shared) system area. Done through an intermediate
	// register so we can be interrupted in the middle of it without corrupting
	// the stack
#if defined (BUILD_ROM_IMAGE)
    // P2 in delta contains the address of a variable which value is 
    // ldf_stack_end - 12
    P0 = [P2];
#else
	P0.L = LO(ldf_stack_end - 12);
	P0.H = HI(ldf_stack_end - 12);
#endif
#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Store the location of '___adi_stack_bounds'
    // Store the location of '___adi_stack_bounds'
    P2.L = ___adi_stack_bounds;
    P2.H = ___adi_stack_bounds;

    // Obtain the system stack limit
    P3.L = ldf_stack_space;
    P3.H = ldf_stack_space;

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    SP = P0;
    [P2] = P3;
    STI R1;
#else
	SP = P0;
#endif

	// L0-L3 define the lengths of the DAG's circular buffers. They must be
	// set to zero when running compiled code, as the compiler uses the DAG
	// registers in non-circular buffer mode. (From C/C++ Compiler Manual).
	// R0 was set to zero above.
	L0 = R0;
	L1 = R0;
	L2 = R0;
	L3 = R0;
    
    FP = R0;  // terminate debugger stack walk

    // Call the Dispatcher. Since this is the only call to it
    // we could - at some point - consider expanding it out
    // in here as hand-written assembly. We would, however, still
    // have to save all the registers and set up the C runtime (i.e.
    // all the above work) in order to be able to invoke the DPCs.
    // We should never come in here unless there is at least one
    // DPC queued.
	CALL (P1); // was CALL.X _tmk_Dispatcher;

    // We have blown through the dispatcher without restoring the
    // running thread's context, so we need to do that now. 
	// Because we had the current thread pointer in P5, which is a
	// preserved register, it will not have been changed by
	// the call to _tmk_Dispatcher. Also, we didn't change the 
	// contextLevel field when we saved the context, so we don't
	// need to change it back now.
	
#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Store the location of '___adi_stack_bounds'
    P0.L = ___adi_stack_bounds;
    P0.H = ___adi_stack_bounds;

    // Obtain the address of the new thread's stack base
    R0 = [P5 + OFFSETOF(TMK_Thread,pStackBase)];

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R1;
    [P0] = R0;

       // Reload the stack pointer (context is saved in stack top)
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
    STI R1;
#else
    // Reload the stack pointer (context is saved in stack top)
    SP = [P5 + OFFSETOF(TMK_Thread,pContextSave)];
#endif

    // There are three stall cycles here, between the load of SP
    // and the start of the pops below, but since all we have to do
    // now is pop the stack there's no other work to be moved in here.
    // We might do the setting of contextLevel to NOCONTEXT here (i.e.
    // after the label below) but that's already done before fanning-
    // -out and is unnecessary (albeit free - maybe) work for the
    // fallthrough case.
 
.RestoreISRContextFromInterruptLevel:
    // If we're here then we're restoring an interrupted thread, i.e.
    // the entire register set. We may have got here either from
	// RescheduleISREntryPoint, or by falling through from RestoreContext
    // above.
    // In either case:
    // - P4 currently contains the thread structure pointer
    // - the thread's m_ContextLevel field has been set to NOCONTEXT
    // - we have a C runtime environment

    // Restore the frame pointer register
    ANOM05000428_3NOPS // The above label is jumped-to with IF CC JUMP xx (BP).
    FP = [SP++];

    // Restore the DSP registers
    A1.X = [SP++];
    A1.W = [SP++];
    A0.X = [SP++];
    A0.W = [SP++];
    I0   = [SP++];
    I1   = [SP++];
    I2   = [SP++];
    I3   = [SP++];
    B0   = [SP++];
    B1   = [SP++];
    B2   = [SP++];
    B3   = [SP++];
    L0   = [SP++];
    L1   = [SP++];
    L2   = [SP++];
    L3   = [SP++];
    M0   = [SP++];
    M1   = [SP++];
    M2   = [SP++];
    M3   = [SP++];

    // Restore the system wide registers
    RETS = [SP++];

    // Load the return-from-interrupt address into the RETI register,
    // as we are going to use the RTI instruction to get back to runtime level.
    RETI = [SP++];   // disables interrupts

#ifdef SELF_NESTING
	// For self-nesting support, we cleared the IVG14 bit in IMASK on entry to
	// the reschedule ISR so we have to set it here. This has to be done between
	// the reload of RETI and the RTI. We know that the IVG14 bit was originally
	// set, since otherwise the reschedule ISR wouldn't have been entered.
	//
	CLI R0;
	BITSET (R0, 14); // unmask IVG14
	STI R0;
#endif

    // Restore the loop registers, we must do this with interrupts disabled,
	// due to anomaly 05-00-0312.
    LB1 = [SP++];
    LT1 = [SP++];
    LC1 = [SP++];
    LB0 = [SP++];
    LT0 = [SP++];
    LC0 = [SP++];

    // Restore ASTAT
    ASTAT = [SP++];

    // Restore the data and address registers
    (R7:0, P5:0) = [SP++];

    // We're done, resume the thread
#if defined (__SILICON_REVISION__) && (defined (__ADSPBF535__) || defined (__AD6532__) || defined (__AD6900__))
    // Workaround for ADSP-BF535 caching anomaly number 05000164 "store to load
    // forwarding in write through mode". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_wt_fix option
    SSYNC;
    RTI;
    NOP;
    NOP;
#else
    RTI;
#endif
    
._tmk_RescheduleISR.end:
._tmk_SwitchContext_ISR.end:


_tmk_RunOnSystemStack:
    // Set the stack to the (shared) system area. Done through an intermediate
	// register so we can be interrupted in the middle of it without corrupting
	// the stack
#if defined (BUILD_ROM_IMAGE)
    // the variable tmk_SystemStackEnd contains ldf_stack_end - 12
    // P2 is a scratch register
	P2.L = LO(_tmk_SystemStackEnd);
	P2.H = HI(_tmk_SystemStackEnd);
    P0 = [P2];
#else
	P0.L = LO(ldf_stack_end - 12);
	P0.H = HI(ldf_stack_end - 12);
#endif
#ifdef STACK_OVERFLOW_DETECTION
    // These changes need to be in the context switch itself and cannot be
    // done in a hook. If they were done in a hook there would be small
    // periods of time when ___adi_stack_bounds would not match the active
    // stack. If an interrupt written in C happened at that point we would
    // get an incorrect stack overflow error.

    // Store the location of '___adi_stack_bounds'
    // Store the location of '___adi_stack_bounds'
    P1.L = ___adi_stack_bounds;
    P1.H = ___adi_stack_bounds;

    // Obtain the system stack limit
    P2.L = ldf_stack_space;
    P2.H = ldf_stack_space;

    // Disable interrupts while updating the stack pointer and stack bounds
    CLI R3;
    SP = P0;
    [P1] = P2;
    STI R3;
#else
    SP = P0;
#endif

    P3 = R2; // third argument is function ptr
    FP = 0;  // terminate debugger stack walk

    JUMP (P3); // 1st & 2nd args still in R0 and R1
        
._tmk_RunOnSystemStack.end:


_tmk_SwitchContext_KillThread:

//  P4 = R0;    // old thread ptr (first argument) - not used
    P5 = R1;	// new thread ptr (second argument)
    R2 = RUNTIME;
    R3 = NOCONTEXT;

	JUMP.X .RestoreAfterKill;

._tmk_SwitchContext_KillThread.end:


