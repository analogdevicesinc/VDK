// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

// before we include any headers we need to say that this file may have both
// short and normal word encoding in Phoenix (it has Interrupt vector table)

#define __ADI_MIXED_ENCODING__

#include <sys/anomaly_macros_rtl.h>
#include "Trinity.h"

.file_attr ISR;
.file_attr OS_Internals;

.IMPORT "Trinity.h";

#include <platform_include.h>

#include <asm_sprt.h>

.IMPORT "SHCtxtRec.h";

.EXTERN MainStackPtr;
.EXTERN ___lib_stack_space;
.EXTERN ___lib_stack_length;

#ifdef __2116x__
#define GLITCHNOP NOP
#else
#define GLITCHNOP
#endif

#ifdef __214xx__
// Hardware Anomaly 09000015/15000002 rti(db) in IRQ or timer can create 
// problems does not apply here because there are no IRQ or timers in this file.
.message/suppress 2522 ;
#endif

/* Effect latency of some System Registers may be 2 cycles instead of 1 for
 * External data accesses
  */
  /* This anomaly is 09000022 for other processors */
  #if WA_15000004
  #define WA_15000004_1NOP NOP;
  #else
  #define WA_15000004_1NOP
  #endif


.EXTERN STRUCT tmk_Globals _tmk;
//.EXTERN _tmk_pCurrentThread;

.EXTERN ldf_stack_end;

//
// Global variables that are used by (just about) every routine in here
//
.SECTION/DM seg_dmda;

// Global variables, accessed from header

// Static variables, only used in this file
.VAR	.ContextI4;
.VAR	.ContextASTAT, .ContextMODE1;
.VAR	.pOldThread, .pNewThread;

#ifndef __2106x__
// The following are used for double-word loads of fixed constants
.SECTION/DM internal_memory_data;
.ALIGN 2;
.VAR .DoubleZero[2] = 0, 0;
.VAR .Plus1Minus1[2] = 1, -1;

// The assembler gives a warning for non-hammerhead processors if we add
// the following directive

.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases

#endif

#if defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#elif defined (__2137x__)
/* the code needs to be in internal memory to avoid anomaly 09000011 */
.SECTION/PM seg_int_code;
#else
.SECTION/PM seg_pmco;
#endif

// Functions used by this module
.EXTERN _TMK_ReleaseMasterKernelLock;
.EXTERN _TMK_ReleaseMasterKernelLockFromKernelLevel;
.EXTERN _tmk_MaskMasterKernelLockForThreadLevel;
.EXTERN _tmk_Dispatcher;
.EXTERN UserExceptionHandler;

// Functions exported by this module
.GLOBAL _tmk_RescheduleISR;
.GLOBAL _tmk_SwitchContext_Runtime;
.GLOBAL _tmk_SwitchContext_FirstThread;
.GLOBAL _tmk_SwitchContext_ISR;
.GLOBAL _tmk_RunOnSystemStack;
.GLOBAL _tmk_SwitchContext_KillThread;

#define LOC(X) OFFSETOF(SHContext, X)

/*
tmk_SwitchContext_Runtime
C-callable entrypoint which is called, via a function pointer,
from tmk_ScheduleThread() only. UNTRUE

When we come in here we know that:
- we're running at thread level
- we're in a C runtime environment
- we're holding the kernel lock, acquired by thread code
- we have the primary registers selected
- the old thread ptr is in R4 (first argument)
- the new thread ptr is in R8 (second argument)

Return type: void
*/
_tmk_SwitchContext_Runtime:
	I4 = R4;	 // scratch register, OK to trample

	// Save the type of context saved. Do this now while we still
	// have the thread pointer in I4.
	R0 = RUNTIME;
	DM(OFFSETOF(TMK_Thread,contextLevel), I4) = R0;	  // safe wrt. SWF

	// Get the thread context pointer into I4. We re-use I4 here
	// because it's the only 'scratch' I register in DAG1.
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I4);
	GLITCHNOP;

	// Save the preserved registers
	DM(LOC(MODE1), I4) = MODE1; // save this before we change it
	// STALL

	// We must always restore the multiplier result registers (MR's) with the SRCU bit in
	// the same state as it was when they were saved, otherwise they'll get swapped over.
	// We choose to achieve this by always having SRCU *clear* during both the save and
	// the restore process. If we come in via the RescheduleISR then that will already have
	// cleared SRCU (amongst others), but if (as here) we entered from user-level runtime
	// then we need to clear the bit explicitly. SRCU will be set back to the thread's
	// original state when MODE1 is restored at the very end of the context switch.
	BIT CLR MODE1 SRCU;
#ifdef __2106x__
	// Save the preserved ALU registers
	DM(LOC(R3),	 I4) = R3;
	DM(LOC(R5),	 I4) = R5;
	DM(LOC(R6),	 I4) = R6;
	DM(LOC(R7),	 I4) = R7;
	DM(LOC(R9),	 I4) = R9;
	DM(LOC(R10), I4) = R10;
	DM(LOC(R11), I4) = R11;
	DM(LOC(R13), I4) = R13;
	DM(LOC(R14), I4) = R14;
	DM(LOC(R15), I4) = R15;

	// Save the preserved DAG1 registers, can't do it directly as we're
	// using I4 as the pointer so we have to shovel everything
	// through R0, at the cost of extra instructions.

	// Save I Registers
	R0 = I0; DM(LOC(I0), I4) = R0;
	R0 = I1; DM(LOC(I1), I4) = R0;
	R0 = I2; DM(LOC(I2), I4) = R0;
	R0 = I3; DM(LOC(I3), I4) = R0;
	R0 = I5; DM(LOC(I5), I4) = R0;
	R0 = I6; DM(LOC(I6), I4) = R0;
	R0 = I7; DM(LOC(I7), I4) = R0;
	// Save M Registers
	R0 = M0; DM(LOC(M0), I4) = R0;
	R0 = M1; DM(LOC(M1), I4) = R0;
	R0 = M2; DM(LOC(M2), I4) = R0;
	R0 = M3; DM(LOC(M3), I4) = R0;
	// Save L Registers
	// Don't save L6 as it is same as L7 (stack length)
	R0 = L7; DM(LOC(L7), I4) = R0;
	// Save B Registers
	R0 = B0; DM(LOC(B0), I4) = R0;
	R0 = B1; DM(LOC(B1), I4) = R0;
	R0 = B2; DM(LOC(B2), I4) = R0;
	R0 = B3; DM(LOC(B3), I4) = R0;
	R0 = B5; DM(LOC(B5), I4) = R0;
	// Don't save B6 as it is same as B7 (stack base)
	R0 = B7; DM(LOC(B7), I4) = R0;

	// Save the preserved DAG2 registers
	DM(LOC(I8),	 I4) = I8;
	DM(LOC(I9),	 I4) = I9;
	DM(LOC(I10), I4) = I10;
	DM(LOC(I11), I4) = I11;
	DM(LOC(I14), I4) = I14;
	DM(LOC(I15), I4) = I15;
	DM(LOC(M8),	 I4) = M8;
	DM(LOC(M9),	 I4) = M9;
	DM(LOC(M10), I4) = M10;
	DM(LOC(M11), I4) = M11;
	DM(LOC(B8),	 I4) = B8;
	DM(LOC(B9),	 I4) = B9;
	DM(LOC(B10), I4) = B10;
	DM(LOC(B11), I4) = B11;
	DM(LOC(B14), I4) = B14;
	DM(LOC(B15), I4) = B15;
#else
	I12 = I4; // we use I12 later for the PM loads

	// Save the preserved PEx registers (in pairs where possible)

    // safe wrt. anomaly 09000022/15000004 as only SRCU changing
.message/suppress 2547 for 1 lines;
	DM(LOC(R3),	 I4) = R3;
	DM(LOC(R5),	 I4) = R5;
	DM(LOC(R6),	 I4) = R6 (LW);
	DM(LOC(R9),	 I4) = R9;
	DM(LOC(R10), I4) = R10 (LW);
	DM(LOC(R13), I4) = R13;
	DM(LOC(R14), I4) = R14 (LW);

	// Save the preserved DAG1 registers, 2 at a time.
	// Since we can't do it through DAG1's I4 register,
	// we use DAG2, taking advantage of the flat addressing
	// model and the fact that both DAGs are 32-bit on 211xx.
	// Unfortunately this causes a stall on each store due
	// to PM bus contention, but since we get to store 2 regs
	// at a time it's still only 1 clock per register so
	// we can't complain too much.

	// Save I Registers
	PM(LOC(I0), I12) = I0 (LW); // STALL
	PM(LOC(I2), I12) = I2 (LW); // STALL
	PM(LOC(I5), I12) = I5;		// STALL
	PM(LOC(I6), I12) = I6 (LW); // STALL
	// Save M Registers
	PM(LOC(M0), I12) = M0 (LW); // STALL
	PM(LOC(M2), I12) = M2 (LW); // STALL
	// Save L Registers
	PM(LOC(L6), I12) = L6 (LW); // STALL
	// Save B Registers
	PM(LOC(B0), I12) = B0 (LW); // STALL
	PM(LOC(B2), I12) = B2 (LW); // STALL
	PM(LOC(B5), I12) = B5;		// STALL
	PM(LOC(B6), I12) = B6 (LW); // STALL

	// Save the preserved DAG2 registers, 2 at a time.
	DM(LOC(I8),	 I4) = I8  (LW);
	DM(LOC(I10), I4) = I10 (LW);
	DM(LOC(I14), I4) = I14 (LW);
	DM(LOC(M8),	 I4) = M8  (LW);
	DM(LOC(M10), I4) = M10 (LW);
	DM(LOC(B8),	 I4) = B8  (LW);
	DM(LOC(B10), I4) = B10 (LW);
	DM(LOC(B14), I4) = B14 (LW);
#endif

#ifdef __214xx__ /* Phoenix only */
	// Save the bit fifo
	R0 = BFFWRP;
	BFFWRP = 64;
	R1 = BITEXT 32;
	R2 = BITEXT 32;
	DM(LOC(_BFFWRP),    I4) = R0;
	DM(LOC(BitFIFO_0), I4) = R1;
	DM(LOC(BitFIFO_1), I4) = R2;
#endif

	// Save the multiplier result registers, note that we
	// save both foreground and background sets (unlike
	// the general and DAG registers where we don't save
	// the alternates) and that we both save and restore
	// these with the SRCU bit clear.
	//
	// We have to move via the general regs because the
	// MRs are *not* universal registers, but since moves
	// between the MR regs and the dregs are 'compute'
	// operations, we can combine them with data memory
	// moves so as to 'pipeline' the MR-to-memory move,
	// provided that the memory addressing stays
	// within the restrictions for this instruction type
	// (6-bit offset field in this case).
	M3 = LOC(PC_STACK);
	R0 = MR0F, I2 = I4; // moved up from below
	R0 = MR1F, DM(LOC(MR0F), I4) = R0;
	R0 = MR2F, DM(LOC(MR1F), I4) = R0;
	R0 = MR0B, DM(LOC(MR2F), I4) = R0;
	R0 = MR1B, DM(LOC(MR0B), I4) = R0;
	R0 = MR2B, DM(LOC(MR1B), I4) = R0;
			   DM(LOC(MR2B), I4) = R0;

	// Assume the following, as we are in a C runtime
	// M6 = 1;	  - used for post-increment
	// L2 = 0;	  - so we can use I2 as the store pointer

	// Save off the PC stack
//	I2 = I4; -- moved above
//	M3 = LOC(PC_STACK); -- moved above
	MODIFY(I2, M3);
	JUMP (PC, .SaveContext_SAVE_PC_STACK_TEST)(DB);	// safe wrt. SWF
		R0 = PCSTKP;
		R0 = PASS R0, DM(LOC(PC_STACK_COUNT), I4) = R0;	 // Save the stack count off

.SaveContext_SAVE_PC_STACK:
	GLITCHNOP;
	R0 = R0 - 1, DM(I2, M6) = PCSTK;		// *no* automatic pop
	POP PCSTK;
.SaveContext_SAVE_PC_STACK_TEST:
	IF NE JUMP (PC, .SaveContext_SAVE_PC_STACK); // can't use the delay slots on this one

.RestoreAfterKill:
    // First we do a three-way branch according to the
    // ContextLevel of the thread:
    // - NOTRUN: goes to .firstEntryFromThreadLevel
    // - INTERRUPTED: goes to .RestoreISRContextFromThreadLevel
    // - RUNTIME: falls though to .RestoreRuntimeContextFromThreadLevel
    //
    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP.
	I3 = R8;       // get new thread ptr into I3
	R1 = RUNTIME;
    // Get the thread's variable that says what type of context is stored
    R0 = DM(OFFSETOF(TMK_Thread,contextLevel), I3);

	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

    // Now we fan-out to the different sorts of context
	COMP(R0, R1);  // compare new contextLevel against RUNTIME
	IF NE JUMP .notRuntime_Thread(DB);
		R1 = NOTRUN;
		COMP(R0, R1);

.RestoreRuntimeContextFromThreadLevel: // label not used
	// If we're here then we're doing a voluntary runtime-to-
	// -runtime context switch, e.g. a thread yield or posting
	// a message or semaphore to a higher-priority pending
	// thread. This is one of the cases that we want to run
	// as fast as possible so we want it on the shortest code path.
    //
	// If we're here then we're returning to a C runtime,
	// i.e. Schedule() called from thread (rather than ISR) level, so
	// we know that the return PC is at addr I6-1 on the thread's
	// stack *and* that when we reload the caller's MODE1 we'll stay
	// in the primary regs.
	//
	// I3 and I4 currently contain the thread structure and thread context
	// pointers, respectively.

	// We are executing with the master kernel lock held, and will
	// continue to do so until after we return from the context switch,
	// therefore we can't be pre-empted and it is safe to set the
	// contextLevel to NOCONTEXT here.
	R0 = NOCONTEXT;
	DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R0;

	// Set up the fixed registers mandated by the C runtime.
	// Do these early so we can use I0 and M6 in the PC stack restore.
#ifdef __2106x__
	L0	= 0;	L1	= 0;	L2	= 0;	L3	= 0;
	L4	= 0;	L5	= 0;	L8	= 0;	L9	= 0;
	L10 = 0;	L11 = 0;	L12 = 0;	L13 = 0;
	L14 = 0;	L15 = 0;
	M5	= 0;	M6	= 1;	M7	= -1;
	M13 = 0;	M14 = 1;	M15 = -1;
#else
	L0	= DM(.DoubleZero) (LW);	L2	= DM(.DoubleZero) (LW);
	L4	= DM(.DoubleZero) (LW);	L8	= DM(.DoubleZero) (LW);
	L10 = DM(.DoubleZero) (LW);	L12 = DM(.DoubleZero) (LW);
	L14 = DM(.DoubleZero) (LW);
	M5	= 0;					M6	= DM(.Plus1Minus1) (LW);
	M13 = 0;					M14 = DM(.Plus1Minus1) (LW);
#endif
	// Restore the PC Stack (must be empty at this point)
	// Setup some variables to restore the PC stack
#ifdef __2116x__
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	M2 = R0;				// get stack count into M2
	IF EQ JUMP (PC, .RestoreContext_DONE_PC_STACK_runtime) (DB);	// if stack count is zero then we're done
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
		NOP;
#else
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	IF EQ JUMP (PC, .RestoreContext_DONE_PC_STACK_runtime) (DB);	// if stack count is zero then we're done
		M2 = R0;				// get stack count into M2
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
#endif

.RestoreContext_PC_STACK_runtime:		// top of loop
	PUSH PCSTK;				// current PC goes on stack here...
	R0 = R0 - 1, PCSTK = DM(I2, M7);	// ...and gets replaced here
	IF NE JUMP (PC, .RestoreContext_PC_STACK_runtime); // pity we can't use the delay slots on this one!

.RestoreContext_DONE_PC_STACK_runtime:

#ifdef __214xx__ /* Phoenix only */
	// Restore the bit fifo
	R0 = DM(LOC(_BFFWRP),    I4);
	R1 = DM(LOC(BitFIFO_0), I4);
	R2 = DM(LOC(BitFIFO_1), I4);
	BFFWRP = 0;
	BITDEP R2 BY 32;
	BITDEP R1 BY 32;
	BFFWRP = R0;
#endif

	// We restore the multiplier result registers now, before MODE1
	// has been restored, i.e. while the SRCU bit is still clear.
#ifdef __2137x__
			   R0 = DM(LOC(MR0F), I4);
	MR0F = R0; R0 = DM(LOC(MR1F), I4);	  // NOTE: semicolons instead of commas!
	MR1F = R0; R0 = DM(LOC(MR2F), I4);	  // Anomaly 09000018 means that we can't
	MR2F = R0; R0 = DM(LOC(MR0B), I4);	  // move to a multiplier register in
	MR0B = R0; R0 = DM(LOC(MR1B), I4);	  // parallel with a memory access, in case
	MR1B = R0; R0 = DM(LOC(MR2B), I4);	  // context record is in external mem.
	MR2B = R0; R3 = DM(LOC(R3),   I4);
#else
	GLITCHNOP;
			   R0 = DM(LOC(MR0F), I4);
	MR0F = R0, R0 = DM(LOC(MR1F), I4);
	MR1F = R0, R0 = DM(LOC(MR2F), I4);
	MR2F = R0, R0 = DM(LOC(MR0B), I4);
	MR0B = R0, R0 = DM(LOC(MR1B), I4);
	MR1B = R0, R0 = DM(LOC(MR2B), I4);
	MR2B = R0, R3 = DM(LOC(R3),   I4);
#endif  /* __2137x__ */

#ifdef __2106x__
	R5  = DM(LOC(R5),  I4);	  // Restore the preserved ALU registers (R3 done above)
	R6  = DM(LOC(R6),  I4);
	R7  = DM(LOC(R7),  I4);
	R9  = DM(LOC(R9),  I4);
	R10 = DM(LOC(R10), I4);
	R11 = DM(LOC(R11), I4);
	R13 = DM(LOC(R13), I4);
	R14 = DM(LOC(R14), I4);
	R15 = DM(LOC(R15), I4);

	// Restore the preserved DAG1 and 2 registers. We do the B Registers first as
	// loading them also loads the corresponding I registers, which
	// then need to be overwritten, below.
	B0	= DM(LOC(B0),  I4);
	B1	= DM(LOC(B1),  I4);
	B2	= DM(LOC(B2),  I4);
	B3	= DM(LOC(B3),  I4);
	B5	= DM(LOC(B5),  I4);
	// B6 & B7 are restored below
	B8	= DM(LOC(B8),  I4);
	B9	= DM(LOC(B9),  I4);
	B10 = DM(LOC(B10), I4);
	B11 = DM(LOC(B11), I4);
	B14 = DM(LOC(B14), I4);
	B15 = DM(LOC(B15), I4);
	// L6 & L7 are restored below
	// Restore M Registers
	M0	= DM(LOC(M0),  I4);
	M1	= DM(LOC(M1),  I4);
	M2	= DM(LOC(M2),  I4);
	M3	= DM(LOC(M3),  I4);
	M8	= DM(LOC(M8),  I4);
	M9	= DM(LOC(M9),  I4);
	M10 = DM(LOC(M10), I4);
	M11 = DM(LOC(M11), I4);
	// Restore I Registers
	I0	= DM(LOC(I0),  I4);
	I1	= DM(LOC(I1),  I4);
	I2	= DM(LOC(I2),  I4);
	I3	= DM(LOC(I3),  I4);
	I5	= DM(LOC(I5),  I4);
	// I6 & I7 are restored below
	I8	= DM(LOC(I8),  I4);
	I9	= DM(LOC(I9),  I4);
	I10 = DM(LOC(I10), I4);
	I11 = DM(LOC(I11), I4);
	I14 = DM(LOC(I14), I4);
	I15 = DM(LOC(I15), I4);

	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_1)(DB);
		BIT CLR MODE1 IRPTEN;
		L6	= DM(LOC(L6),  I4);	 // stack length ****same as L7****
.SH_INT_DISABLED_1:
	L7	= DM(LOC(L7),  I4);	 // stack length
	B6	= DM(LOC(B6),  I4);	 // stack base ****same as B7****
	B7	= DM(LOC(B7),  I4);	 // stack base (clobbers I7)
	I6	= DM(LOC(I6),  I4);	 // frame ptr
	I7	= DM(LOC(I7),  I4);	 // Stack pointer
	BIT SET MODE1 IRPTEN;
#else
	// Set I12 equal to I4 so we can use DAG2 for some of the restores
	I12 = I4;

	R5  = DM(LOC(R5),  I4);
	R6  = DM(LOC(R6),  I4) (LW);
	R9  = DM(LOC(R9),  I4);
	R10 = DM(LOC(R10), I4) (LW);
	R13 = DM(LOC(R13), I4);
	R14 = DM(LOC(R14), I4) (LW);

	// Restore the DAG1 registers, using DAG2. Using DAG1 (or using DAG2 to load DAG2
	// registers) doesn't work for LW loads as the second register doesn't get loaded.
	// This appears to be an undocumented hardware anomaly (the simulator loads both
	// registers). As in the save code, above, using PM access does cost us a stall
	// cycle on each instruction, but with LW we still manage 1 register per cycle.

	// We do the B Registers first as loading them also loads the
	// corresponding I registers, which then need to be overwritten,
	// below.
	B0	= PM(LOC(B0),  I12) (LW);
	B2	= PM(LOC(B2),  I12) (LW);
	B5	= PM(LOC(B5),  I12);
	// B6 & B7 are restored below
	// Restore L Registers
	// L6 & L7 are restored below
	// Restore M Registers
	M0	= PM(LOC(M0),  I12) (LW);
	M2	= PM(LOC(M2),  I12) (LW);
	// Restore I Registers
	I0	= PM(LOC(I0),  I12) (LW);
	I2	= PM(LOC(I2),  I12) (LW);
	I5	= PM(LOC(I5),  I12);
	// I6 & I7 are restored below

	// Load L6:7,B6:7, and I6:7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC,.SH_INT_DISABLED_2)(DB);
	BIT CLR MODE1 IRPTEN;
	NOP;
.SH_INT_DISABLED_2:
	L6	= PM(LOC(L6),  I12) (LW);	// stack length
	B6	= PM(LOC(B6),  I12) (LW);	// clobbers I6 and I7
	I6	= PM(LOC(I6),  I12) (LW);	// frame ptr and stack ptr
	BIT SET MODE1 IRPTEN;

	// Restore DAG2 preserved B registers
	B8	= DM(LOC(B8),  I4) (LW);	// stack base
	B10 = DM(LOC(B10), I4) (LW);
	B14 = DM(LOC(B14), I4) (LW);
	// Restore DAG2 preserved M registers
	M8	= DM(LOC(M8),  I4) (LW);
	M10 = DM(LOC(M10), I4) (LW);
	// Restore DAG2 preserved I registers
	I8	= DM(LOC(I8),  I4) (LW);
	I10 = DM(LOC(I10), I4) (LW);
	I14 = DM(LOC(I14), I4) (LW);
#endif

	// ...and do a C-style function return to get back to Schedule().
#if defined(__2116x__)
	I12=DM(-1,I6);
	R2=I6;
	NOP;					// Glitch workaround
	I6=DM(0,I6);			// daren't use RFRAME on 21160
	JUMP (M14,I12) (DB);
		MODE1 = DM(LOC(MODE1), I4);		// MODE1 restored
		I7=R2;
#else
	I12=DM(-1,I6);
	R0 = DM(LOC(MODE1), I4);			// ok to trash scratch R0
	JUMP (M14,I12) (DB);
		MODE1 = R0;						// MODE1 restored
		RFRAME;
#endif
    
    
.notRuntime_Thread:
    // We could (possibly) release the master kernel lock here,
    // but we don't at the moment. Instead it gets released
    // separately on each of the two code paths (interrupt and
    // first-time).
	
	// We did the COMP against NOTRUN in the delay slots of the jump to here	
    IF EQ JUMP .firstEntryFromThreadLevel;

.RestoreISRContextFromThreadLevel: // label not used

	// Whereas all the other (five) context-restore scenarios execute
	// with the master kernel lock held, and hence are not pre-emptible,
	// this case (restoring an interrupted thread from runtime level)
	// releases the master kernel lock early, and depends on the
	// restore sequence being *restartable* in the event of being pre-
	// -empted.
	//
	// Once we've released the master kernel lock the reschedule
	// interrupt may take us to kernel level, in which case we won't
	// return to the point of interruption. Instead the 

    // Release the kernel lock before running the thread. We must
	// have finished accessing the thread struct by this point.
    ccall(_TMK_ReleaseMasterKernelLock);

	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	// If we are here then we are restoring an ISR (i.e full) context and have the
	// primary registers selected. Now we need to restore all the registers
	// in the primary set, i.e. the data regs and the DAGS.

	// Use subroutine to restore primary data and DAG regs
	CALL .RestoreAllPrimaryRegs;

	// Now we're running in the secondary regs, but we aren't holding the
	// master kernel lock. This is OK because:
	// - if we take an interrupt and go to kernel mode we will *not*
	//   return here, instead the restore will restart. Therefore:
	// - it doesn't matter that any changes that we make to secondaries
	//   here will get trashed by kernel level.

	// need to reload these now we're in the secondary regs
	I3 = DM(_tmk->pCurrentThread);		// thread structure pointer
	GLITCHNOP;
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	// Restore the PC Stack (must be empty at this point)
	// Setup some variables to restore the PC stack
#ifndef __2116x__
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	IF EQ JUMP (PC, .RestoreContextISR_DONE_PC_STACK) (DB);  // if stack count is zero then we're done
		M2 = R0;				// get stack count into M2
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
#else
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	M2 = R0;				// get stack count into M2
	IF EQ JUMP (PC, .RestoreContextISR_DONE_PC_STACK) (DB);  // if stack count is zero then we're done
	   	MODIFY(I2, M2);			// and add it to I2 (the load pointer)
		NOP;
#endif

.RestoreContextISR_PC_STACK_runtime:		// top of loop
	PUSH PCSTK;				// current PC goes on stack here...
	R0 = R0 - 1, PCSTK = DM(I2, M7);	// ...and gets replaced here
	IF NE JUMP (PC, .RestoreContextISR_PC_STACK_runtime); // pity we can't use the delay slots on this one!

.RestoreContextISR_DONE_PC_STACK_runtime:
#ifdef __2116x__
	// Restore the loop stack (must be empty at this point)
	// Setup some variables to restore the LOOP stack
	M2 = LOC(LOOP_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(LOOP_STACK_COUNT), I4); // get the saved stack count
	R0 = R0 + R0, MODIFY(I2, M2);	// R0 = 2 * R0, move load pointer to stack region
	M2 = R0;			// get stack count into M2
	IF EQ JUMP (PC, .RestoreContext_DONE_LOOP_STACK_runtime) (DB);  // if stack count is zero then we're done
		R0 = R0 - 1;		// pre-decrement count
		R0 = R0 - 1, MODIFY(I2, M2);	// continue pre-decrement, load ptr += (2 * LOOP_STACK_COUNT)(STALL)
#else
	// Restore the loop stack (must be empty at this point)
	// Setup some variables to restore the loop stack
	M2 = LOC(LOOP_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(LOOP_STACK_COUNT), I4); // get the saved stack count
	R0 = R0 + R0, MODIFY(I2, M2);	// R0 = 2 * R0, move load pointer to stack region
	IF EQ JUMP (PC, .RestoreContext_DONE_LOOP_STACK_runtime) (DB);  // if stack count is zero then we're done
		R0 = R0 - 1, M2 = R0;		// pre-decrement count, get stack count into M2
		R0 = R0 - 1, MODIFY(I2, M2);	// continue pre-decrement, load ptr += (2 * LOOP_STACK_COUNT)(STALL)
#endif

.RestoreContext_LOOP_STACK_runtime:	// top of loop

// Anomaly workaround for 2137x and 214xx. Can't do direct load from memory to
// the Loop Registers. If the context area memory bank is also being written to
// by the DMA engine, then the loop register values can get corrupted when they
// are being restored. 
// This is anomaly 09000023/15000005
#if defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__214xx__)
	// Restore Loop Counters directly from the context area
	PUSH LOOP;
	
	// Get the value of the CURLCNTR from the context area
	R2 = DM(I2,M7);
	CURLCNTR = R2;
	
	// Get the value of the LADDR from the context area
	R2 = DM(I2,M7);
	LADDR = R2;

	IF EQ JUMP	(PC, .RestoreContext_DONE_LOOP_STACK_runtime);
	
		JUMP (PC, .RestoreContext_LOOP_STACK_runtime) (db);
		// Decrement count	
		R0 = R0 - 1;
		R0 = R0 - 1;
#else
	PUSH LOOP;
	IF NE JUMP (PC, .RestoreContext_LOOP_STACK_runtime)(DB);	// R0 == 0 on final iteration
		R0 = R0 - 1, CURLCNTR = DM(I2, M7); // post-decrement count, load CURLCNTR from saved context
		R0 = R0 - 1, LADDR	  = DM(I2, M7); // cont. post-decrement count, load LADDR from saved context
#endif		

.RestoreContext_DONE_LOOP_STACK_runtime:

#ifdef __214xx__ /* Phoenix only */
	// Restore the bit fifo
	R0 = DM(LOC(_BFFWRP),    I4);
	R1 = DM(LOC(BitFIFO_0), I4);
	R2 = DM(LOC(BitFIFO_1), I4);
	BFFWRP = 0;
	BITDEP R2 BY 32;
	BITDEP R1 BY 32;
	BFFWRP = R0;
#endif

#ifndef __2106x__
	// Enter SIMD mode to restore PE registers in parallel
	BIT SET MODE1 PEYEN;
#endif

// Anomaly workaround for 2137x and 214xx. Can't do direct load from memory to
// the Loop Registers. If the context area memory bank is also being written to
// by the DMA engine, then the loop register values can get corrupted when they
// are being restored.
// This is anomaly 09000023/15000005
#if defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__214xx__)
	R0 = DM(LOC(LCNTR), I4);
	LCNTR = R0;		// always restore LCNTR (same in SIMD or SISD)
#else
	LCNTR = DM(LOC(LCNTR), I4);		// always restore LCNTR (same in SIMD or SISD)
#endif

	// We restore the multiplier result registers now, before MODE1
	// has been restored, i.e. while the SRCU bit is still clear.
	// ASTAT is restored later, so it doesn't matter that loading
	// the MR registers will clear the M* flag bits.
			   R0 = DM(LOC(MR0F), I4);
#ifdef __2137x__
	MR0F = R0; R0 = DM(LOC(MR1F), I4);	  // NOTE: semicolons instead of commas!
	MR1F = R0; R0 = DM(LOC(MR2F), I4);	  // Anomaly 09000018 means that we can't
	MR2F = R0; R0 = DM(LOC(MR0B), I4);	  // move to a multiplier register in
	MR0B = R0; R0 = DM(LOC(MR1B), I4);	  // parallel with a memory access, in case
	MR1B = R0; R0 = DM(LOC(MR2B), I4);	  // context record is in external mem.
#else
	MR0F = R0, R0 = DM(LOC(MR1F), I4);
	MR1F = R0, R0 = DM(LOC(MR2F), I4);
	MR2F = R0, R0 = DM(LOC(MR0B), I4);
	MR0B = R0, R0 = DM(LOC(MR1B), I4);
	MR1B = R0, R0 = DM(LOC(MR2B), I4);
#endif
	MR2B = R0;

	// Restore the STKY flags, nothing that we do from
	// here on out should change these
	STKY = DM(LOC(STKY), I4);

//.RestoreContext_ISR_Return:
	// We are (back) in the secondary registers, but
	// the processor has *not* been completely restored at this time
	// (ASTAT and MODE1 are still in their respective globals).
	// Restore the remaining registers before returning to the thread domain.
#ifdef __2106x__
	// Restore the bus exchange registers
	PX1 = DM(LOC(PX1), I4);
	PX2 = DM(LOC(PX2), I4);

	// Restore the User status registers
	USTAT1 = DM(LOC(USTAT1), I4);
	USTAT2 = DM(LOC(USTAT2), I4);

	ASTAT = DM(.ContextASTAT);
	
	// Disable interrupts before modifying the incoming thread's contextLevel
	JUMP (PC, .SH_INT_DISABLED_3) (DB);
		BIT CLR MODE1 IRPTEN;
		R1 = NOCONTEXT;
.SH_INT_DISABLED_3:
#else 
	// Restore ASTATx and ASTATy. The only way of getting at ASTATy is to
	// do this while in SIMD mode.
	ASTAT = DM(LOC(ASTAT), I4);

	// Restore the bus exchange registers (PX2 implicit in SIMD)
	PX1 = DM(LOC(PX1), I4);

	// Restore the User status registers
	USTAT1 = DM(LOC(USTAT1), I4);
	USTAT3 = DM(LOC(USTAT3), I4);
	
	// Disable interrupts before modifying the incoming thread's contextLevel
	JUMP (PC, .SH_INT_DISABLED_3) (DB);
		BIT CLR MODE1 ( PEYEN | IRPTEN );
		R1 = NOCONTEXT; 
.SH_INT_DISABLED_3:
#endif
	
	// Interrupts are now disabled
	// Set the the ContextLevel to NOCONTEXT. Should an interrupt
	// be latched after this point, and prompt another context switch, then 
	// the context for this thread will be saved and restored rather than 
	// restarting.
	//
	// I3 is still unaltered from (far) above, it should still point to the
	// thread structure of the incoming thread.
	NOP; // Anomaly 09000022/15000004, we could be in SIMD here
	DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R1;

	// Restoring MODE1 will complete the context switch. We should return
	// to using the primary registers and it will also re-enable 
	// interrupts.
	R0 = DM(.ContextMODE1);   // ok to trash secondary R0
#ifdef __2136x__
	RTI (DB);
#else
	RTS (DB, LR);
#endif
	MODE1 = R0;   // return to original register set
	NOP;	

._tmk_SwitchContext_Runtime.end:

_tmk_SwitchContext_FirstThread:    // new thread ptr passed as 2nd argument, 1st is dummy

.firstEntryFromThreadLevel:
	// We *must* be finished accessing the thread struct and the context struct
	// before we release the master kernel lock, because once we release it we
	// could be pre-empted, which would overwrite the context before we're finished
	// loading it. This would be bad.

	I3 = R8;
	// Get the pointers to the context records
	GLITCHNOP;
    I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

    // Get the address of the new thread's run function into
    // a preserved register, so it doesn't get trashed by the
    // call to TMK_ReleaseMasterKernelLock().
    R3 = DM(OFFSETOF(TMK_Thread,pStuff), I3);

	// Load stack and frame pointers from the context record.
	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_4)(DB);
		BIT CLR MODE1 IRPTEN;
		NOP;
.SH_INT_DISABLED_4:
	L6	= DM(LOC(L6),  I4);	 // stack length ****same as L7****
	L7	= DM(LOC(L7),  I4);	 // stack length
	B6	= DM(LOC(B6),  I4);	 // stack base ****same as B7****
	B7	= DM(LOC(B7),  I4);	 // stack base (clobbers I7)
	I6	= DM(LOC(I6),  I4);	 // frame ptr
	I7	= DM(LOC(I7),  I4);	 // Stack pointer
	BIT SET MODE1 IRPTEN;

    // Mark in the thread object that there's no context saved for this thread.
	R0 = NOCONTEXT;
    DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R0;

    // Release the kernel lock before running the thread.
	// This call runs on the new thread's stack. After this
	// call we can be pre-empted.
	//
    ccall(_TMK_ReleaseMasterKernelLock);

	R4 = I3;  // pass thread pointer as first argument to run function
	I12 = R3; // get the run funtion address into I12

	// C/C++ "call though function pointer" sequence.
	r2=i6;
	i6=i7;
	jump (m13,i12) (db);
	dm(i7,m7)=r2;
	dm(i7,m7)= pc;
._tmk_SwitchContext_FirstThread.end:


/*
tmk_SwitchContext_ISR
C-callable entrypoint which is called, via a function pointer,
from tmk_Dispatcher() only.

When we come in here we know that:
- we're running at kernel level, still at reschedule interrupt level
- we're holding the kernel lock, acquired by tmk_Dispatcher()
- the old thread ptr is in R4 (first argument)
- the new thread ptr is in R8 (second argument)

Return type: void
*/
_tmk_SwitchContext_ISR:
	// We're at interrupt level, so we're executing in the secondary
	// registers and the rest of the state has already been saved
	// (except for MODE1 and ASTAT which are still in their respective
	// globals where they were saved on entry to RescheduleISR).
	DM(.pOldThread) = R4;
	DM(.pNewThread) = R8;

	I3 = R4;
	R5 = INTERRUPTED;
	R0 = DM(OFFSETOF(TMK_Thread, contextLevel), I3);
	COMP(R0, R5);  // compare contextLevel to INTERRUPTED

	// Now we just have to switch to the primaries and save them.
	BIT CLR MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL); // select primaries
	NOP;

	// If the contextLevel of the interrupted thread is INTERRUPTED (rather
	// than NOCONTEXT) then we have pre-empted the restore - from thread level -
	// of an interrupted thread. In this case we will (eventually) restart
	// the context restore rather than resuming it, so we don't want to save anything
	// to the context record now.
	//
	IF EQ JUMP .NoSaveNeeded2;

	// Now we *cannot* assume a C runtime as we've no idea what
	// is in the primary registers.
 
	// We use I4 as the thread structure pointer throughout, as opposed to I0 as in the
	// 219x code, because it's a scratch register in the SHARC C runtime environment
	// (which doesn't help us here but does elsewhere).
	DM(.ContextI4) = I4;	 // safe wrt. SWF
	I4 = DM(.pOldThread);
	GLITCHNOP;
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I4);
	GLITCHNOP;
#ifdef __2106x__
	// Save off the primary ALU registers
	DM(LOC(R0),	 I4) = R0;
	DM(LOC(R1),	 I4) = R1;
	DM(LOC(R2),	 I4) = R2;
	DM(LOC(R3),	 I4) = R3;
	DM(LOC(R4),	 I4) = R4;
	DM(LOC(R5),	 I4) = R5;
	DM(LOC(R6),	 I4) = R6;
	DM(LOC(R7),	 I4) = R7;
	DM(LOC(R8),	 I4) = R8;
	DM(LOC(R9),	 I4) = R9;
	DM(LOC(R10), I4) = R10;
	DM(LOC(R11), I4) = R11;
	DM(LOC(R12), I4) = R12;
	DM(LOC(R13), I4) = R13;
	DM(LOC(R14), I4) = R14;
	DM(LOC(R15), I4) = R15;

	PX = R0;			  // capture the contents of R0L before we trash it
	R0 = DM(.ContextMODE1);
	BTST R0 BY 16;		  // bit 16 is the RND32 flag
	IF NOT SZ JUMP .Save_32bit_only; // could use delayed here

// Macro to pack the low 8 bits of 4 40-bit R registers into
// a single 32-bit word in Rb. Assumes that Ra has already been
// copied to PX.
//
#define GLOM(Ra,Rb,Rc,Rd) \
	Ra = PX1; \
	PX = Rb;  \
	Rb = PX1; \
	Rb = Rb OR LSHIFT Ra BY -8; \
	PX = Rc;  \
	Rc = PX1; \
	PX = Rd;  \
	Rd = PX1; \
	Rd = Rd OR LSHIFT Rc BY -8; \
	Rb = Rb OR LSHIFT Rd BY 16;

	// Pack and store the low bits of the R registers,
	// in batches of 4.
	//
	// R0 already copied to PX, above
	GLOM(R0,  R1,  R2,	R3);
	DM(LOC(R0_3L), I4) = R1;
	PX = R4;
	GLOM(R4,  R5,  R6,	R7);
	DM(LOC(R4_7L), I4) = R5;
	PX = R8;
	GLOM(R8,  R9,  R10, R11);
	DM(LOC(R8_11L), I4) = R9;
	PX = R12;
	GLOM(R12, R13, R14, R15);
	DM(LOC(R12_15L), I4) = R13;

#undef GLOM

.Save_32bit_only:
	GLITCHNOP;

	// Save the DAG2 registers, we do these first simply because
	// that's the order we do them in the 2116x version.
	DM(LOC(I8),	 I4) = I8;
	DM(LOC(I9),	 I4) = I9;
	DM(LOC(I10), I4) = I10;
	DM(LOC(I11), I4) = I11;
	DM(LOC(I12), I4) = I12;
	DM(LOC(I13), I4) = I13;
	DM(LOC(I14), I4) = I14;
	DM(LOC(I15), I4) = I15;
	DM(LOC(M8),	 I4) = M8;
	DM(LOC(M9),	 I4) = M9;
	DM(LOC(M10), I4) = M10;
	DM(LOC(M11), I4) = M11;
	DM(LOC(M12), I4) = M12;
	DM(LOC(M13), I4) = M13;
	DM(LOC(M14), I4) = M14;
	DM(LOC(M15), I4) = M15;
	DM(LOC(L8),	 I4) = L8;
	DM(LOC(L9),	 I4) = L9;
	DM(LOC(L10), I4) = L10;
	DM(LOC(L11), I4) = L11;
	DM(LOC(L12), I4) = L12;
	DM(LOC(L13), I4) = L13;
	DM(LOC(L14), I4) = L14;
	DM(LOC(L15), I4) = L15;
	DM(LOC(B8),	 I4) = B8;
	DM(LOC(B9),	 I4) = B9;
	DM(LOC(B10), I4) = B10;
	DM(LOC(B11), I4) = B11;
	DM(LOC(B12), I4) = B12;
	DM(LOC(B13), I4) = B13;
	DM(LOC(B14), I4) = B14;
	DM(LOC(B15), I4) = B15;

	// Save the DAG1 registers, can't do it directly as we're
	// using I4 as the pointer so we have to shovel everything
	// through R0, at the cost of extra instructions.

	// Save I Registers
	R0 = I0; DM(LOC(I0), I4) = R0;
	R0 = I1; DM(LOC(I1), I4) = R0;
	R0 = I2; DM(LOC(I2), I4) = R0;
	R0 = I3; DM(LOC(I3), I4) = R0;
	R0 = DM(.ContextI4);	  // I4 is already saved here
			 DM(LOC(I4), I4) = R0;
	R0 = I5; DM(LOC(I5), I4) = R0;
	R0 = I6; DM(LOC(I6), I4) = R0;
	R0 = I7; DM(LOC(I7), I4) = R0;
	// Save M Registers
	R0 = M0; DM(LOC(M0), I4) = R0;
	R0 = M1; DM(LOC(M1), I4) = R0;
	R0 = M2; DM(LOC(M2), I4) = R0;
	R0 = M3; DM(LOC(M3), I4) = R0;
	R0 = M4; DM(LOC(M4), I4) = R0;
	R0 = M5; DM(LOC(M5), I4) = R0;
	R0 = M6; DM(LOC(M6), I4) = R0;
	R0 = M7; DM(LOC(M7), I4) = R0;
	// Save L Registers
	R0 = L0; DM(LOC(L0), I4) = R0;
	R0 = L1; DM(LOC(L1), I4) = R0;
	R0 = L2; DM(LOC(L2), I4) = R0;
	R0 = L3; DM(LOC(L3), I4) = R0;
	R0 = L4; DM(LOC(L4), I4) = R0;
	R0 = L5; DM(LOC(L5), I4) = R0;
	R0 = L6; DM(LOC(L6), I4) = R0;
	R0 = L7; DM(LOC(L7), I4) = R0;
	// Save B Registers
	R0 = B0; DM(LOC(B0), I4) = R0;
	R0 = B1; DM(LOC(B1), I4) = R0;
	R0 = B2; DM(LOC(B2), I4) = R0;
	R0 = B3; DM(LOC(B3), I4) = R0;
	R0 = B4; DM(LOC(B4), I4) = R0;
	R0 = B5; DM(LOC(B5), I4) = R0;
	R0 = B6; DM(LOC(B6), I4) = R0;
	R0 = B7; DM(LOC(B7), I4) = R0;
#else
	// Save off the primary ALU registers, 2 at a time
	DM(LOC(R0),	 I4) = R0  (LW);
	DM(LOC(R2),	 I4) = R2  (LW);
	DM(LOC(R4),	 I4) = R4  (LW);
	DM(LOC(R6),	 I4) = R6  (LW);
	DM(LOC(R8),	 I4) = R8  (LW);
	DM(LOC(R10), I4) = R10 (LW);
	DM(LOC(R12), I4) = R12 (LW);
	DM(LOC(R14), I4) = R14 (LW);
	DM(LOC(S0),	 I4) = S0  (LW);
	DM(LOC(S2),	 I4) = S2  (LW);
	DM(LOC(S4),	 I4) = S4  (LW);
	DM(LOC(S6),	 I4) = S6  (LW);
	DM(LOC(S8),	 I4) = S8  (LW);
	DM(LOC(S10), I4) = S10 (LW);
	DM(LOC(S12), I4) = S12 (LW);
	DM(LOC(S14), I4) = S14 (LW);

	PX = R0;			  // capture the contents of R0L before we trash it
	R0 = DM(.ContextMODE1);
	BTST R0 BY 16;		  // bit 16 is the RND32 flag
	IF NOT SZ JUMP .Save_32bit_only; // could use delayed here

// Macro to pack the low 8 bits of 8 40-bit S and R registers into
// a single pair of 32-bit words in Ra:Rb. Assumes that Ra has
// already been copied to PX.
//
#define GLOM(Sa,Sb,Sc,Sd,Ra,Rb,Rc,Rd) \
	Ra = PX1; \
	PX = Rb;  \
	Rb = PX1; \
	Rb = Rb OR LSHIFT Ra BY -8; \
	PX = Rc;  \
	Rc = PX1; \
	PX = Rd;  \
	Rd = PX1; \
	Rd = Rd OR LSHIFT Rc BY -8; \
	Rb = Rb OR LSHIFT Rd BY -16; \
	PX = Sb; \
	Ra = PX1; \
	PX = Sa;  \
	Rc = Px1; \
	Ra = Ra OR LSHIFT Rc BY -8; \
	PX = Sc;  \
	Rc = PX1; \
	PX = Sd;  \
	Rd = PX1; \
	Rd = Rd OR LSHIFT Rc BY -8; \
	Ra = Ra OR LSHIFT Rd BY -16;

	// Pack and store the low bits of the S and R registers,
	// in batches of 8.
	//
	// R0 already copied to PX, above
	GLOM(S0, S1, S2, S3, R0, R1, R2, R3);
	DM(LOC(SR0_3L), I4) = R0(LW);
	PX = R4;
	GLOM(S4, S5, S6, S7, R4, R5, R6, R7);
	DM(LOC(SR4_7L), I4) = R4(LW);
	PX = R8;
	GLOM(S8, S9, S10, S11, R8, R9, R10, R11);
	DM(LOC(SR8_11L), I4) = R8(LW);
	PX = R12;
	GLOM(S12, S13, S14, S15, R12, R13, R14, R15);
	DM(LOC(SR12_15L), I4) = R12(LW);

#undef GLOM
.Save_32bit_only:
	GLITCHNOP;

	// Save the DAG2 registers, 2 at a time.
	// Do these first so we can re-use I9, below.
	DM(LOC(I8),	 I4) = I8  (LW);
	I9 = I4;
	DM(LOC(I10), I4) = I10 (LW);
	DM(LOC(I12), I4) = I12 (LW);
	DM(LOC(I14), I4) = I14 (LW);
	DM(LOC(M8),	 I4) = M8  (LW);
	DM(LOC(M10), I4) = M10 (LW);
	DM(LOC(M12), I4) = M12 (LW);
	DM(LOC(M14), I4) = M14 (LW);
	DM(LOC(L8),	 I4) = L8  (LW);
	DM(LOC(L10), I4) = L10 (LW);
	DM(LOC(L12), I4) = L12 (LW);
	DM(LOC(L14), I4) = L14 (LW);
	DM(LOC(B8),	 I4) = B8  (LW);
	DM(LOC(B10), I4) = B10 (LW);
	DM(LOC(B12), I4) = B12 (LW);
	DM(LOC(B14), I4) = B14 (LW);

	// Save the DAG1 registers, 2 at a time.
	// Since we can't do it through DAG1's I4 register,
	// we use DAG2, taking advantage of the flat addressing
	// model and the fact that both DAGs are 32-bit on 211xx.
	// Unfortunately this causes a stall on each store due
	// to PM bus contention, but since we get to store 2 regs
	// at a time it's still only 1 clock per register so
	// we can't complain too much.

	// Save I Registers
	PM(LOC(I0), I9) = I0 (LW); // STALL, safe wrt. SWF
	PM(LOC(I2), I9) = I2 (LW); // STALL, safe wrt. SWF
	// I4 is already saved to its global
	R0 = DM(.ContextI4);
	DM(LOC(I4), I4) = R0;
	PM(LOC(I5), I9) = I5;	   // STALL
	PM(LOC(I6), I9) = I6 (LW); // STALL
	// Save M Registers
	PM(LOC(M0), I9) = M0 (LW); // STALL
	PM(LOC(M2), I9) = M2 (LW); // STALL
	PM(LOC(M4), I9) = M4 (LW); // STALL
	PM(LOC(M6), I9) = M6 (LW); // STALL
	// Save L Registers
	PM(LOC(L0), I9) = L0 (LW); // STALL
	PM(LOC(L2), I9) = L2 (LW); // STALL
	PM(LOC(L4), I9) = L4 (LW); // STALL
	PM(LOC(L6), I9) = L6 (LW); // STALL
	// Save B Registers
	PM(LOC(B0), I9) = B0 (LW); // STALL
	PM(LOC(B2), I9) = B2 (LW); // STALL
	PM(LOC(B4), I9) = B4 (LW); // STALL, safe wrt. SWF
	PM(LOC(B6), I9) = B6 (LW); // STALL, safe wrt. SWF
#endif

	R0 = DM(.ContextMODE1);		 // moved up to fill stall slot
	R1 = DM(.ContextASTAT);		 // moved up to fill stall slot

	// Move the already-saved MODE1 and ASTAT register values to the context record
	DM(LOC(MODE1), I4) = R0;
	DM(LOC(ASTAT), I4) = R1;

.NoSaveNeeded2:
	BIT SET MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL); // select secondaries
	NOP;

	R0 = NOCONTEXT;
	R3 = RUNTIME;
//    R5 = INTERRUPTED;
    
    // The entire register set of the old thread has now been saved
	// so now we mark it as containing the context of an interrupted
	// (i.e. pre-empted) thread.
    DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R5; // R5 == INTERRUPTED

	I3 = R8;       // get new thread ptr into I3

    // Get the thread's variable that says what type of context is stored
    GLITCHNOP;
    R6 = DM(OFFSETOF(TMK_Thread,contextLevel), I3);

    // In all three cases the thread's contextLevel field should
    // be set to NOCONTEXT ASAP, so we do that here.

    // Next we do a three-way branch according to the contextLevel of the new thread:
    COMP(R6,R3); // compare contextLevel against RUNTIME
    IF NE JUMP .notRuntime_ISR(DB);
	   COMP(R6,R5); // compare contextLevel against INTERRUPTED
	   DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R0;

	// the contextLevel was RUNTIME
	//
.RestoreRuntimeContextFromInterruptLevel:  // label not used
    ccall(_tmk_MaskMasterKernelLockForThreadLevel);

	// Switch to the primary registers
	BIT CLR MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL); // select primaries
	NOP;
	NOP; // Anomaly 09000022/15000004, we could be in either reg set here
	I3 = DM(.pNewThread);
	GLITCHNOP;
    I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	// If we're here then we're returning to a C runtime,
	// i.e. Schedule() called from thread (rather than ISR) level, so
	// we know that the return PC is at addr I6-1 on the thread's
	// stack *and* that when we reload the caller's MODE1 we'll stay
	// in the primary regs.
	//
	// I3 and I4 currently contain the thread structure and thread context
	// pointers, respectively.

	// Set up the fixed registers mandated by the C runtime.
	// Do these early so we can use I0 and M6 in the PC stack restore.
#ifdef __2106x__
	L0	= 0;	L1	= 0;	L2	= 0;	L3	= 0;
	L4	= 0;	L5	= 0;	L8	= 0;	L9	= 0;
	L10 = 0;	L11 = 0;	L12 = 0;	L13 = 0;
	L14 = 0;	L15 = 0;
	M5	= 0;				M6	= 1;	M7	= -1;
	M13 = 0;				M14 = 1;	M15 = -1;
#else
	L0	= DM(.DoubleZero) (LW);	L2	= DM(.DoubleZero) (LW);
	L4	= DM(.DoubleZero) (LW);	L8	= DM(.DoubleZero) (LW);
	L10 = DM(.DoubleZero) (LW);	L12 = DM(.DoubleZero) (LW);
	L14 = DM(.DoubleZero) (LW);
	M5	= 0;					M6	= DM(.Plus1Minus1) (LW);
	M13 = 0;					M14 = DM(.Plus1Minus1) (LW);
#endif
	// Restore the PC Stack (must be empty at this point)
	// Setup some variables to restore the PC stack
#ifndef __2116x__
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	IF EQ JUMP (PC, .RestoreContext_DONE_PC_STACK_ISR) (DB);	// if stack count is zero then we're done
		M2 = R0;				// get stack count into M2
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
#else
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	M2 = R0;				// get stack count into M2
	IF EQ JUMP (PC, .RestoreContext_DONE_PC_STACK_ISR) (DB);	// if stack count is zero then we're done
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
		NOP;
#endif

.RestoreContext_PC_STACK_ISR:		// top of loop
	PUSH PCSTK;				// current PC goes on stack here...
	R0 = R0 - 1, PCSTK = DM(I2, M7);	// ...and gets replaced here
	IF NE JUMP (PC, .RestoreContext_PC_STACK_ISR); // pity we can't use the delay slots on this one!

.RestoreContext_DONE_PC_STACK_ISR:
#ifndef __2106x__
	// Set I12 equal to I4 so we can use DAG2 for some of the restores
	I12 = I4;
#endif

#ifdef __214xx__ /* Phoenix only */
	// Restore the bit fifo
	R0 = DM(LOC(_BFFWRP),    I4);
	R1 = DM(LOC(BitFIFO_0), I4);
	R2 = DM(LOC(BitFIFO_1), I4);
	BFFWRP = 0;
	BITDEP R2 BY 32;
	BITDEP R1 BY 32;
	BFFWRP = R0;
#endif

	// We restore the multiplier result registers now, before MODE1
	// has been restored, i.e. while the SRCU bit is still clear.
#ifdef __2137x__
			   R0 = DM(LOC(MR0F), I4);
	MR0F = R0; R0 = DM(LOC(MR1F), I4);	  // NOTE: semicolons instead of commas!
	MR1F = R0; R0 = DM(LOC(MR2F), I4);	  // Anomaly 09000018 means that we can't
	MR2F = R0; R0 = DM(LOC(MR0B), I4);	  // move to a multiplier register in
	MR0B = R0; R0 = DM(LOC(MR1B), I4);	  // parallel with a memory access, in case
	MR1B = R0; R0 = DM(LOC(MR2B), I4);	  // context record is in external mem.
	MR2B = R0; R3 = DM(LOC(R3),   I4);
#else
			   R0 = DM(LOC(MR0F), I4);
	MR0F = R0, R0 = DM(LOC(MR1F), I4);
	MR1F = R0, R0 = DM(LOC(MR2F), I4);
	MR2F = R0, R0 = DM(LOC(MR0B), I4);
	MR0B = R0, R0 = DM(LOC(MR1B), I4);
	MR1B = R0, R0 = DM(LOC(MR2B), I4);
	MR2B = R0, R3  = DM(LOC(R3),  I4);
#endif

#ifdef __2106x__
	R5  = DM(LOC(R5),  I4);   // Restore the preserved ALU registers (R3 done above)
	R6  = DM(LOC(R6),  I4);
	R7  = DM(LOC(R7),  I4);
	R9  = DM(LOC(R9),  I4);
	R10 = DM(LOC(R10), I4);
	R11 = DM(LOC(R11), I4);
	R13 = DM(LOC(R13), I4);
	R14 = DM(LOC(R14), I4);
	R15 = DM(LOC(R15), I4);

	// Restore the preserved DAG1 and 2 registers. We do the B Registers first as
	// loading them also loads the corresponding I registers, which
	// then need to be overwritten, below.
	B0	= DM(LOC(B0),  I4);
	B1	= DM(LOC(B1),  I4);
	B2	= DM(LOC(B2),  I4);
	B3	= DM(LOC(B3),  I4);
	B5	= DM(LOC(B5),  I4);
	// B6 & B7 are restored below
	B8	= DM(LOC(B8),  I4);
	B9	= DM(LOC(B9),  I4);
	B10 = DM(LOC(B10), I4);
	B11 = DM(LOC(B11), I4);
	B14 = DM(LOC(B14), I4);
	B15 = DM(LOC(B15), I4);
	// L6 & L7 are restored below
	// Restore M Registers
	M0	= DM(LOC(M0),  I4);
	M1	= DM(LOC(M1),  I4);
	M2	= DM(LOC(M2),  I4);
	M3	= DM(LOC(M3),  I4);
	M8	= DM(LOC(M8),  I4);
	M9	= DM(LOC(M9),  I4);
	M10 = DM(LOC(M10), I4);
	M11 = DM(LOC(M11), I4);
	// Restore I Registers
	I0	= DM(LOC(I0),  I4);
	I1	= DM(LOC(I1),  I4);
	I2	= DM(LOC(I2),  I4);
	I3	= DM(LOC(I3),  I4);
	I5	= DM(LOC(I5),  I4);
	// I6 & I7 are restored below
	I8	= DM(LOC(I8),  I4);
	I9	= DM(LOC(I9),  I4);
	I10 = DM(LOC(I10), I4);
	I11 = DM(LOC(I11), I4);
	I14 = DM(LOC(I14), I4);
	I15 = DM(LOC(I15), I4);

	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_5)(DB);
		BIT CLR MODE1 IRPTEN;
		L6	= DM(LOC(L6),  I4);	 // stack length ****same as L7****
.SH_INT_DISABLED_5:
	L7	= DM(LOC(L7),  I4);	 // stack length
	B6	= DM(LOC(B6),  I4);	 // stack base ****same as B7****
	B7	= DM(LOC(B7),  I4);	 // stack base (clobbers I7)
	I6	= DM(LOC(I6),  I4);	 // frame ptr
	I7	= DM(LOC(I7),  I4);	 // Stack pointer
	BIT SET MODE1 IRPTEN;
#else
	R5  = DM(LOC(R5),  I4);	  // Restore the preserved ALU registers (R3 done above)
	R6  = DM(LOC(R6),  I4) (LW);
	R9  = DM(LOC(R9),  I4);
	R10 = DM(LOC(R10), I4) (LW);
	R13 = DM(LOC(R13), I4);
	R14 = DM(LOC(R14), I4) (LW);

	// Restore the DAG1 registers, using DAG2. Using DAG1 (or using DAG2 to load DAG2
	// registers) doesn't work for LW loads as the second register doesn't get loaded.
	// This appears to be an undocumented hardware anomaly (the simulator loads both
	// registers). As in the save code, above, using PM access does cost us a stall
	// cycle on each instruction, but with LW we still manage 1 register per cycle.

	// We do the B Registers first as loading them also loads the
	// corresponding I registers, which then need to be overwritten,
	// below.
	B0	= PM(LOC(B0),  I12) (LW);
	B2	= PM(LOC(B2),  I12) (LW);
	B5	= PM(LOC(B5),  I12);
	// B6 & B7 are restored below
	// Restore L Registers
	// L6 & L7 are restored below
	// Restore M Registers
	M0	= PM(LOC(M0),  I12) (LW);
	M2	= PM(LOC(M2),  I12) (LW);
	// Restore I Registers
	I0	= PM(LOC(I0),  I12) (LW);
	I2	= PM(LOC(I2),  I12) (LW);
	I5	= PM(LOC(I5),  I12);
	// I6 & I7 are restored below

	// Load L6:7,B6:7, and I6:7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_6)(DB);
		BIT CLR MODE1 IRPTEN;
		NOP;
.SH_INT_DISABLED_6:
	L6	= PM(LOC(L6),  I12) (LW);	// stack length
	B6	= PM(LOC(B6),  I12) (LW);	// clobbers I6 and I7
	I6	= PM(LOC(I6),  I12) (LW);	// frame ptr and stack ptr
	BIT SET MODE1 IRPTEN;

	// Restore DAG2 preserved B registers
	B8	= DM(LOC(B8),  I4) (LW);	// stack base
	B10 = DM(LOC(B10), I4) (LW);
	B14 = DM(LOC(B14), I4) (LW);
	// Restore DAG2 preserved M registers
	M8	= DM(LOC(M8),  I4) (LW);
	M10 = DM(LOC(M10), I4) (LW);
	// Restore DAG2 preserved I registers
	I8	= DM(LOC(I8),  I4) (LW);
	I10 = DM(LOC(I10), I4) (LW);
	I14 = DM(LOC(I14), I4) (LW);
#endif

	// ...and do an RTI along with C-style function stackframe unwind.
	// R1 is a scratch register so it's ok to trash it.
	//
	R1 = DM(-1,I6);		// get the return PC from the stack frame
	R1 = R1 + 1;		// increment it to point to the return location
	PUSH PCSTK;			// push the PC stack and put the return address
	PCSTK = R1;			// onto the top of the PC stack
	R1 = DM(LOC(MODE1), I4);
#if defined(__2116x__)
	R2 = I6;
	I6 = DM(0,I6);		// we can't use RFRAME on early 21160 revisions
	RTI (DB);
		MODE1 = R1;		// MODE1 restored
		I7 = R2;        // can't do I6 = DM(0,I6) here due to SWF anomaly(?)
#else
	RTI (DB);
		MODE1 = R1;		// MODE1 restored
		RFRAME;
#endif


	// The contextLevel wasn't RUNTIME
	//
.notRuntime_ISR:
	// I3 contains the new thread ptr
	// R3 contains RUNTIME
	// R5 contains INTERRUPTED
	// R6 contains the new thread's contextLevel

	// On SHARC we know that there's no SMP, and hence no spinlock
	// associated with the master kernel lock.
    // This means that we can release the master kernel lock here and
    // still continue to access the thread and context structs, safe
	// in the knowledge that we are still at kernel level and hence
	// cannot be pre-empted.

	// Since there's no SMP, the call to TMK_ReleaseMasterKernelLockFromKernelLevel()
	// is only really needed to manage the error-checking state and can be omitted
	// in non-debug builds.
#if !defined(NDEBUG)
    ccall(_TMK_ReleaseMasterKernelLockFromKernelLevel);

	// Re-do comparison as ALU status will have been clobbered by the call
	COMP(R6,R5);
#endif

	// Switch to the primary registers
	BIT CLR MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL); // select primaries
	NOP;
	NOP; // Anomaly 09000022/15000004, we could be in either reg set here
	// Get the thread ptr into I3 and the context struct ptr into I4
	I3 = DM(.pNewThread);
	GLITCHNOP;
    I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	IF EQ JUMP .RestoreISRContextFromInterruptLevel;
    
	// The contextLevel was NOTRUN
	//
.firstEntryFromInterruptLevel: // unused label
    // The primary registers are selected

    // Get the address of the new thread's run function onto the top
	// of the PC stack.
	PUSH PCSTK;
	PCSTK = DM(OFFSETOF(TMK_Thread, pStuff), I3);
	
	R4 = I3;  // pass thread pointer as first argument to run function

	// Set up the fixed registers mandated by the C runtime.
#ifdef __2106x__
	L0	= 0;	L1	= 0;	L2	= 0;	L3	= 0;
	L4	= 0;	L5	= 0;	L8	= 0;	L9	= 0;
	L10 = 0;	L11 = 0;	L12 = 0;	L13 = 0;
	L14 = 0;	L15 = 0;
	M5	= 0;				M6	= 1;	M7	= -1;
	M13 = 0;				M14 = 1;	M15 = -1;
#else
	L0	= DM(.DoubleZero) (LW);	L2	= DM(.DoubleZero) (LW);
	L4	= DM(.DoubleZero) (LW);	L8	= DM(.DoubleZero) (LW);
	L10 = DM(.DoubleZero) (LW);	L12 = DM(.DoubleZero) (LW);
	L14 = DM(.DoubleZero) (LW);
	M5	= 0;					M6	= DM(.Plus1Minus1) (LW);
	M13 = 0;					M14 = DM(.Plus1Minus1) (LW);
#endif

	// Load stack and frame pointers from the context record.
	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_7)(DB);
		BIT CLR MODE1 IRPTEN;
		NOP;
.SH_INT_DISABLED_7:
	L6	= DM(LOC(L6),  I4);	 // stack length ****same as L7****
	L7	= DM(LOC(L7),  I4);	 // stack length
	B6	= DM(LOC(B6),  I4);	 // stack base ****same as B7****
	B7	= DM(LOC(B7),  I4);	 // stack base (clobbers I7)
	I6	= DM(LOC(I6),  I4);	 // frame ptr
	I7	= DM(LOC(I7),  I4);	 // Stack pointer
	BIT SET MODE1 IRPTEN;
	r2=i6;
	i6=i7;
	RTI(DB);
	dm(i7,m7)=r2;
	dm(i7,m7)=pc;

    // If we got here then the run function returned, this should be a kernel panic.
.loopStop2:
    JUMP .loopStop2;

.RestoreISRContextFromInterruptLevel:
	// If we are here then we are restoring an ISR (i.e full) context and have the
	// primary registers selected. Now we need to restore all the registers
	// in the primary set, i.e. the data regs and the DAGS.

	// Use subroutine to restore primary data and DAG regs
	CALL .RestoreAllPrimaryRegs;

	// need to reload these now we're in the secondary regs
	R1 = NOCONTEXT;  // put this here to avoid DAG stall triggering BR glitch anomaly
	I3 = DM(_tmk->pCurrentThread);		// thread structure pointer
    
	JUMP .ReturnThroughRescheduleISR(DB);
	    I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);
	    DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R1; // Clear the ContextLevel, safe wrt. SWF

.SelectAndRestoreAllPrimaryRegs:
	// Switch to the primary registers
	BIT CLR MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL); 
	NOP;
	NOP; // Anomaly 09000022/15000004, we could be in either reg set here	
	// We need to get the context pointer into I4 before falling through.
	I4 = DM(_tmk->pCurrentThread);		// thread structure pointer
	// I4 is context save area pointer
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I4);
	
	// Fall through
	
.RestoreAllPrimaryRegs:
	// Must be called with the primary registers already selected
	// Expects the context struct ptr to be in I4

#ifdef __2106x__
	// Move ASTAT from the context record into .ContextASTAT,
	// from where it will be restored before we return to user code.
	//
	// In order not to change the state of the 4 flag bits in ASTAT
	// (which may be controlling hardware, for all we know) we mask-
	// -merge those bits from the current ASTAT into the value that
	// we're restoring from the context record.
	//
	// Doing this early means that we can trash registers with
	// abandon and don't have to worry about ASTAT later on.
	R12 = 0xFF87FFFF;			 // load up the flags mask
	R10 = DM(LOC(ASTAT), I4);		 // get the to-be-restored ASTAT value
	R10 = R10 AND R12, R11 = ASTAT;	 // mask out flag bits, get current ASTAT
	R12 = FEXT R11 BY 19:4;		 // extract the flag bits from ASTAT into R12
	R10 = R10 OR FDEP R12 BY 19:4;	 // insert the flags into R10
	DM(.ContextASTAT) = R10;	 // and move it to ASTAT's global

	R0 = DM(LOC(MODE1), I4); // Move MODE1 from the context record into its global
	DM(.ContextMODE1) = R0;	 // safe wrt. SWF

	// Restore the ALU registers
	BTST R0 BY 16;		  // bit 16 is the RND32 flag
	IF NOT SZ JUMP .Restore_32bit_only;

// Macro to unpack the 4x8-bit fields from Rb and load them into
// Ra-Rd as the low 8-bits of the 40-bit word, the
// remaining 32 bits coming from the context record.
//
#define UNGLOM(Ra, Rb, Rc, Rd) \
	Ra  = LSHIFT Rb BY 8;      \
	Rc  = LSHIFT Rb BY -8;     \
	Rd  = LSHIFT Rb BY -16;    \
	PX2 = DM(LOC(Ra), I4);     \
	PX1 = Ra;                  \
	Ra  = PX;                  \
	PX2 = DM(LOC(Rb), I4);     \
	PX1 = Rb;                  \
	Rb  = PX;                  \
	PX2 = DM(LOC(Rc), I4);     \
	PX1 = Rc;                  \
	Rc  = PX;                  \
	PX2 = DM(LOC(Rd), I4);     \
	PX1 = Rd;                  \
	Rd  = PX;

	// Load the 40-bit R registers, in batches of 4
	//
	R1 = DM(LOC(R0_3L), I4);
	UNGLOM(R0, R1, R2, R3);
	R5 = DM(LOC(R4_7L), I4);
	UNGLOM(R4, R5, R6, R7);
	R9 = DM(LOC(R8_11L), I4);
	UNGLOM(R8, R9, R10, R11);
	R13 = DM(LOC(R12_15L), I4);
	UNGLOM(R12, R13, R14, R15);
#undef UNGLOM

	JUMP .DReg_Restore_Done;
.Restore_32bit_only:
	GLITCHNOP;

	R0	= DM(LOC(R0),  I4);
	R1	= DM(LOC(R1),  I4);
	R2	= DM(LOC(R2),  I4);
	R3	= DM(LOC(R3),  I4);
	R4	= DM(LOC(R4),  I4);
	R5	= DM(LOC(R5),  I4);
	R6	= DM(LOC(R6),  I4);
	R7	= DM(LOC(R7),  I4);
	R8	= DM(LOC(R8),  I4);
	R9	= DM(LOC(R9),  I4);
	R10 = DM(LOC(R10), I4);
	R11 = DM(LOC(R11), I4);
	R12 = DM(LOC(R12), I4);
	R13 = DM(LOC(R13), I4);
	R14 = DM(LOC(R14), I4);
	R15 = DM(LOC(R15), I4);
.DReg_Restore_Done:

	// Switch to secondary *data* registers now that we've restored the primaries
	BIT SET MODE1 (SRRFH | SRRFL);

	// Restore the DAG1 and DAG2 registers. We do the B Registers first as
	// loading them also loads the corresponding I registers, which
	// then need to be overwritten, below.
	B0	= DM(LOC(B0),  I4);
	B1	= DM(LOC(B1),  I4);
	B2	= DM(LOC(B2),  I4);
	B3	= DM(LOC(B3),  I4);
	// B4 is restored later to avoid clobbering I4 while it's still in use.
	B5	= DM(LOC(B5),  I4);
	B6	= DM(LOC(B6),  I4);
	// B6 & B7 are restored below
	B8	= DM(LOC(B8),  I4);
	B9	= DM(LOC(B9),  I4);
	B10 = DM(LOC(B10), I4);
	B11 = DM(LOC(B11), I4);
	B12 = DM(LOC(B12), I4);
	B13 = DM(LOC(B13), I4);
	B14 = DM(LOC(B14), I4);
	B15 = DM(LOC(B15), I4);
	// Restore L Registers
	L0	= DM(LOC(L0),  I4);
	L1	= DM(LOC(L1),  I4);
	L2	= DM(LOC(L2),  I4);
	L3	= DM(LOC(L3),  I4);
	L4	= DM(LOC(L4),  I4);
	L5	= DM(LOC(L5),  I4);
	// L6 & L7 are restored below
	L8	= DM(LOC(L8),  I4);
	L9	= DM(LOC(L9),  I4);
	L10 = DM(LOC(L10), I4);
	L11 = DM(LOC(L11), I4);
	L12 = DM(LOC(L12), I4);
	L13 = DM(LOC(L13), I4);
	L14 = DM(LOC(L14), I4);
	L15 = DM(LOC(L15), I4);
	// Restore M Registers
	M0	= DM(LOC(M0),  I4);
	M1	= DM(LOC(M1),  I4);
	M2	= DM(LOC(M2),  I4);
	M3	= DM(LOC(M3),  I4);
	M4	= DM(LOC(M4),  I4);
	M5	= DM(LOC(M5),  I4);
	M6	= DM(LOC(M6),  I4);
	M7	= DM(LOC(M7),  I4);
	M8	= DM(LOC(M8),  I4);
	M9	= DM(LOC(M9),  I4);
	M10 = DM(LOC(M10), I4);
	M11 = DM(LOC(M11), I4);
	M12 = DM(LOC(M12), I4);
	M13 = DM(LOC(M13), I4);
	M14 = DM(LOC(M14), I4);
	M15 = DM(LOC(M15), I4);
	// Restore I Registers
	I0	= DM(LOC(I0),  I4);
	I1	= DM(LOC(I1),  I4);
	I2	= DM(LOC(I2),  I4);
	I3	= DM(LOC(I3),  I4);
	// I4 is restored later as it's still in use here
	I5	= DM(LOC(I5),  I4);
	// I6 & I7 are restored below

	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_8)(DB);
		BIT CLR MODE1 IRPTEN;
		L6	= DM(LOC(L6),  I4);
.SH_INT_DISABLED_8:
	L7	= DM(LOC(L7),  I4);	 // stack length
	B6	= DM(LOC(B6),  I4);	 // stack base (clobbers I7)
	B7	= DM(LOC(B7),  I4);	 // stack base (clobbers I7)
	I6	= DM(LOC(I6),  I4);  // frame pointer
	I7	= DM(LOC(I7),  I4);	 // stack pointer
	BIT SET MODE1 IRPTEN;

	I8	= DM(LOC(I8),  I4);
	I9	= DM(LOC(I9),  I4);
	I10 = DM(LOC(I10), I4);
	I11 = DM(LOC(I11), I4);
	I12 = DM(LOC(I12), I4);
	I13 = DM(LOC(I13), I4);
	I14 = DM(LOC(I14), I4);
	I15 = DM(LOC(I15), I4);
#else
	// Set I12 equal to I4 so we can use DAG2 for some of the restores
	I12 = I4;

	R0 = DM(LOC(MODE1), I4); // Move MODE1 from the context record into its global
	DM(.ContextMODE1) = R0;	 // safe wrt. SWF

	BTST R0 BY 16;		  // bit 16 is the RND32 flag
	IF NOT SZ JUMP .Restore_32bit_only;

// Macro to unpack the 8x8-bit fields from Ra:Rb and load them into
// Sa-Sd and Ra-Rd as the low 8-bits of the 40-bit word, the
// remaining 32 bits coming from the context record.
//
#define UNGLOM(Sa, Sb, Sc, Sd, Ra, Rb, Rc, Rd) \
	PX2 = DM(LOC(Sb), I4);     \
	PX1 = Ra;                  \
	Sb  = PX;                  \
	PX2 = DM(LOC(Sa), I4);     \
	Ra  = LSHIFT Ra by 8;      \
	PX1 = Ra;                  \
	Sa  = PX;                  \
	PX2 = DM(LOC(Sd), I4);     \
	Ra  = LSHIFT Ra by 8;      \
	PX1 = Ra;                  \
	Sd  = PX;                  \
	PX2 = DM(LOC(Sc), I4);     \
	Ra  = LSHIFT Ra by 8;      \
	PX1 = Ra;                  \
	Sc  = PX;                  \
	PX2 = DM(LOC(Rb), I4);     \
	Rc  = LSHIFT Rb BY 8;      \
	PX1 = Rb;                  \
	Rb  = PX;                  \
	PX2 = DM(LOC(Ra), I4);     \
	PX1 = Rc;                  \
	Ra  = PX;                  \
	PX2 = DM(LOC(Rd), I4);     \
	Rc  = LSHIFT Rc BY 8;      \
	PX1 = Rc;                  \
	Rd  = PX;                  \
	PX2 = DM(LOC(Rc), I4);     \
	Rc  = LSHIFT Rc BY 8;      \
	PX1 = Rc;                  \
	Rc  = PX;

	// Load the 40-bit R and S registers, in batches of 8
	//
	R0 = DM(LOC(SR0_3L), I4)(LW);
	UNGLOM(S0, S1, S2, S3, R0, R1, R2, R3);
	R4 = DM(LOC(SR4_7L), I4)(LW);
	UNGLOM(S4, S5, S6, S7, R4, R5, R6, R7);
	R8 = DM(LOC(SR8_11L), I4)(LW);
	UNGLOM(S8, S9, S10, S11, R8, R9, R10, R11);
	R12 = DM(LOC(SR12_15L), I4)(LW);
	UNGLOM(S12, S13, S14, S15, R12, R13, R14, R15);
#undef UNGLOM

	JUMP .DReg_Restore_Done;
.Restore_32bit_only:
	GLITCHNOP;

	// Restore the ALU registers
	R0	= DM(LOC(R0),  I4) (LW);
	R2	= DM(LOC(R2),  I4) (LW);
	R4	= DM(LOC(R4),  I4) (LW);
	R6	= DM(LOC(R6),  I4) (LW);
	R8	= DM(LOC(R8),  I4) (LW);
	R10 = DM(LOC(R10), I4) (LW);
	R12 = DM(LOC(R12), I4) (LW);
	R14 = DM(LOC(R14), I4) (LW);
	S0	= DM(LOC(S0),  I4) (LW);
	S2	= DM(LOC(S2),  I4) (LW);
	S4	= DM(LOC(S4),  I4) (LW);
	S6	= DM(LOC(S6),  I4) (LW);
	S8	= DM(LOC(S8),  I4) (LW);
	S10 = DM(LOC(S10), I4) (LW);
	S12 = DM(LOC(S12), I4) (LW);
	S14 = DM(LOC(S14), I4) (LW);
.DReg_Restore_Done:

	// Switch to secondary *data* registers now that we've restored the primaries
	BIT SET MODE1 (SRRFH | SRRFL);

	// Restore the DAG1 registers, using DAG2. Using DAG1 (or using DAG2 to load DAG2
	// registers) doesn't work for LW loads as the second register doesn't get loaded.
	// This appears to be an undocumented hardware anomaly (the simulator loads both
	// registers). As in the save code, above, using PM access does cost us a stall
	// cycle on each instruction, but with LW we still manage 1 register per cycle.

	// We do the B Registers first as loading them also loads the
	// corresponding I registers, which then need to be overwritten,
	// below.
	B0	= PM(LOC(B0),  I12) (LW);
    // safe wrt. anomaly 09000022/15000004 as only the data reg selection is changing
.message/suppress 2547 for 1 lines;
	B2	= PM(LOC(B2),  I12) (LW);
	// B4 is restored later to avoid clobbering I4 while it's still in use.
	B5	= PM(LOC(B5),  I12);
	// B6 & B7 are restored below
	// Restore L Registers
	L0	= PM(LOC(L0),  I12) (LW);
	L2	= PM(LOC(L2),  I12) (LW);
	L4	= PM(LOC(L4),  I12) (LW);
	// L6 & L7 are restored below
	// Restore M Registers
	M0	= PM(LOC(M0),  I12) (LW);
	M2	= PM(LOC(M2),  I12) (LW);
	M4	= PM(LOC(M4),  I12) (LW);
	M6	= PM(LOC(M6),  I12) (LW);
	// Restore I Registers
	I0	= PM(LOC(I0),  I12) (LW);
	I2	= PM(LOC(I2),  I12) (LW);
	// I4 is restored later as it's still in use here
	I5	= PM(LOC(I5),  I12);
	// L6 & I7 are restored below

	// Load L6:7,B6:7, and I6:7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
	JUMP (PC, .SH_INT_DISABLED_9)(DB);
		BIT CLR MODE1 IRPTEN;
		NOP;
.SH_INT_DISABLED_9:
	L6	= PM(LOC(L6),  I12) (LW);	// stack length
	B6	= PM(LOC(B6),  I12) (LW);	// clobbers I6 and I7
	I6	= PM(LOC(I6),  I12) (LW);	// finished with I12 after this
	BIT SET MODE1 IRPTEN;

	// DAG2 restore
	// Restore the B registers, before the I regs
	B8	= DM(LOC(B8),  I4) (LW);
	B10 = DM(LOC(B10), I4) (LW);
	B12 = DM(LOC(B12), I4) (LW);
	B14 = DM(LOC(B14), I4) (LW);
	// Restore L Registers
	L8	= DM(LOC(L8),  I4) (LW);
	L10 = DM(LOC(L10), I4) (LW);
	L12 = DM(LOC(L12), I4) (LW);
	L14 = DM(LOC(L14), I4) (LW);
	// Restore M Registers
	M8	= DM(LOC(M8),  I4) (LW);
	M10 = DM(LOC(M10), I4) (LW);
	M12 = DM(LOC(M12), I4) (LW);
	M14 = DM(LOC(M14), I4) (LW);
	// Restore I Registers
	I8	= DM(LOC(I8),  I4) (LW);
	I10 = DM(LOC(I10), I4) (LW);
	I12 = DM(LOC(I12), I4) (LW);
	I14 = DM(LOC(I14), I4) (LW);
#endif
	// Restore I4 and B4. After this we can't access the context
	// record any more, only the globals. We have to move I4 through
	// R0 since it is being used as the load pointer.
	R0 = DM(LOC(I4), I4);	// trashes *alternate* R0 
	B4 = DM(LOC(B4), I4);	// B4 restored, I4 trashed (STALL)
	I4 = R0;			// I4 restored

	// Return from subroutine, switching to the secondary data regs
	// (we're already in the secondary DAG registers).
	//
	RTS(DB);
		BIT SET MODE1 (SRD1H | SRD1L | SRD2H | SRD2L);
		NOP;

._tmk_SwitchContext_ISR.end:


.NoSaveNeeded:
	// We have interrupted the restore of an interrupt context. The
	// context-restore will be restarted (rather than resumed) so we
	// need to clear down the PC and loop stacks to that they're
	// empty as expected when the restore restarts.

	// We are in the alternate registers, so it's OK to trash R0.

	// Clear the PC stack
	JUMP (PC, .NoSaveISR_CLEAR_PC_STACK_TEST)(DB);	// safe wrt. SWF
		R0 = PCSTKP;
		R0 = PASS R0;

.NoSaveISR_CLEAR_PC_STACK:
	R0 = R0 - 1;
	POP PCSTK;
.NoSaveISR_CLEAR_PC_STACK_TEST:
	IF NE JUMP (PC, .NoSaveISR_CLEAR_PC_STACK); // can't use the delay slots on this one

	// Clear the loop stack
.NoSave_CLEAR_LOOP_STACK:
	BIT TST STKY LSEM;
	IF TF JUMP .NoSaveDone;
	POP LOOP;					// Pop the loop stack
	JUMP .NoSave_CLEAR_LOOP_STACK;



///////////////////////////////////////////////////////////////////////
#ifdef __214xx__
.SECTION/PM/NW seg_SFT3I;
#else
.SECTION/PM seg_SFT3I;
#endif
.global tmk_RescheduleISRTrampoline;

tmk_RescheduleISRTrampoline:
	// We enter the reschedule ISR indirectly via this lower-priority
	// software interrupt. The reason for this is that if we're *in*
	// the reschedule ISR - and another ISR re-raises the reschedule
	// interrupt - then the hardware will force-clear the IRPTL bit
	// (incredible but true) when it returns to the reschedule ISR.
	// This could cause us to miss a DPC service request.
	//
	RTI (DB);
		BIT SET IRPTL SFT2I;  // raise the "real" reschedule interrupt
		NOP;
	
	/* should never get here... */
	NOP;
.tmk_RescheduleISRTrampoline.end:


///////////////////////////////////////////////////////////////////////
#ifdef __214xx__
.SECTION/PM/NW seg_SFT2I;
#else
.SECTION/PM seg_SFT2I;
#endif
.global tmk_RescheduleISRVector;

tmk_RescheduleISRVector:
	// Jump to the ISR handler while saving the current contents of
	// MODE1 and resetting various bits back to the C runtime state.
	//
	JUMP _tmk_RescheduleISR (DB);
		// Save the state of MODE1 before we change it
		DM(.ContextMODE1) = MODE1;

		// TURN off bit-reversing, SIMD mode, broadcast loads, circular buffering
		// and ALU saturation.
#ifdef __2106x__
	    BIT CLR MODE1 (BR0 | BR8 | ALUSAT | SRCU);
#else
		BIT CLR MODE1 (BR0 | BR8 | ALUSAT | PEYEN | BDCST1 | BDCST9 | CBUFEN | SRCU);
#endif

	/* should never get here... */
    /* we add the symbol so it does not get removed when elimination is on */
	JUMP tmk_RescheduleISRTrampoline;
.tmk_RescheduleISRVector.end:


#if defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#elif defined (__2137x__)
/* the code needs to be in internal memory to avoid anomaly 09000011 */
.SECTION/PM seg_int_code;
#else
.SECTION/PM seg_pmco;
#endif

/*
 * _tmk_RescheduleISR
 *
 * This is the service routine for the kernel interrupt. It is the handler
 * for all pre-emptive activity in the system.
 */
_tmk_RescheduleISR:

	// Select secondary register sets
	BIT SET MODE1 (SRD1H | SRD1L | SRD2H | SRD2L | SRRFH | SRRFL | RND32);

	// We're gonna trample ASTAT here...
	DM(.ContextASTAT) = ASTAT;		   // safe wrt. SWF

	// We have already saved MODE1 and ASTAT to their respective
    // statics. We are in executing in the secondary registers so we
	// can pretty much trash what we like.	We are *not* going to save
	// the (primary) data or DAG registers (until we know that a context
	// switch is happening).
	//
	NOP; // Anomaly 09000022/15000004, we could be in either reg set here
	I3 = DM(_tmk->pCurrentThread);		// I3 is thread structure pointer
	GLITCHNOP;

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
	R0 = DM(OFFSETOF(TMK_Thread,contextLevel), I3);
	R1 = NOCONTEXT;
	COMP(R0,R1);        // ASTAT gets trashed here

	// I4 is context save area pointer
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	IF NE JUMP .NoSaveNeeded;

	// Assume the following, as we are in RescheduleISR's C runtime
	// (in the alternate registers).
	// M6 = 1;	  - used for post-increment
	// L2 = 0;	  - so we can use I2 as the store pointer

	// Save off the PC stack
	I2 = I4;
	M3 = LOC(PC_STACK);
	GLITCHNOP;
	MODIFY(I2, M3);
	JUMP (PC, .SaveContextISR_SAVE_PC_STACK_TEST)(DB);	// safe wrt. SWF
		R0 = PCSTKP;
		R0 = PASS R0, DM(LOC(PC_STACK_COUNT), I4) = R0;	 // Save the stack count off

.SaveContextISR_SAVE_PC_STACK:
	GLITCHNOP;
	R0 = R0 - 1, DM(I2, M6) = PCSTK;		// *no* automatic pop
	POP PCSTK;
.SaveContextISR_SAVE_PC_STACK_TEST:
	IF NE JUMP (PC, .SaveContextISR_SAVE_PC_STACK); // can't use the delay slots on this one

	// Setup to save the Loop stack
	I2 = I4;
	M3 = LOC(LOOP_STACK);
	GLITCHNOP;
	R0 = R0 - R0, MODIFY(I2, M3);

	// Always save LCNTR
	DM(LOC(LCNTR), I4) = LCNTR;
	// Should we save off the loop stack???
.SaveContext_SAVE_LOOP_STACK:
	BIT TST STKY LSEM;
	IF TF JUMP (PC, .SaveContext_DONE_LOOP_STACK);
	R0 = R0 + 1, DM(I2, M6) = LADDR;		// Increment the count & save top of loop address stack
	JUMP (PC, .SaveContext_SAVE_LOOP_STACK) (DB);	   // safe wrt. SWF
		DM(I2, M6) = CURLCNTR;			// save top of loop counter stack
		POP LOOP;					// Pop the loop stack
.SaveContext_DONE_LOOP_STACK:
	GLITCHNOP;
	DM(LOC(LOOP_STACK_COUNT), I4) = R0;			   // Save the stack count off, safe wrt. SWF

#ifdef __214xx__ /* Phoenix only */
	// Save the bit fifo
	R0 = BFFWRP;
	BFFWRP = 64;
	R1 = BITEXT 32;
	R2 = BITEXT 32;
	DM(LOC(_BFFWRP),    I4) = R0;
	DM(LOC(BitFIFO_0), I4) = R1;
	DM(LOC(BitFIFO_1), I4) = R2;
#endif

#ifndef __2106x__
	// Reload ASTAT
	ASTAT = DM(.ContextASTAT);

	// Enter SIMD mode to save PE registers in parallel
	BIT SET MODE1 PEYEN;
	NOP;
    WA_15000004_1NOP

	// Save the computation status regs (ASTATX and ASTATY)
	DM(LOC(ASTAT), I4) = ASTAT;
#endif
	DM(LOC(STKY),  I4) = STKY;

	// Save the multiplier result registers, note that we
	// save both foreground and background sets (unlike
	// the general and DAG registers where we don't save
	// the alternates) and that we both save and restore
	// these with the SRCU bit clear.
	//
	// We have to move via the general regs because the
	// MRs are *not* universal registers, but since moves
	// between the MR regs and the dregs are 'compute'
	// operations, we can combine them with data memory
	// moves so as to 'pipeline' the MR-to-memory move,
	// provided that the memory addressing stays
	// within the restrictions for this instruction type
	// (6-bit offset field in this case).
	//
	// Note also that these moves from the MR registers
	// clear the M* flags in ASTAT, so we must have saved
	// ASTAT first.
	R0 = MR0F;				// also trashes (alt) S0 in SIMD
	R0 = MR1F, DM(LOC(MR0F), I4) = R0;
	R0 = MR2F, DM(LOC(MR1F), I4) = R0;
	R0 = MR0B, DM(LOC(MR2F), I4) = R0;
	R0 = MR1B, DM(LOC(MR0B), I4) = R0;
	R0 = MR2B, DM(LOC(MR1B), I4) = R0;
			   DM(LOC(MR2B), I4) = R0;
#ifdef __2106x__
	// Save the bus exchange registers
	DM(LOC(PX1), I4) = PX1;
	DM(LOC(PX2), I4) = PX2;
	// Save the User status registers
	DM(LOC(USTAT1), I4) = USTAT1;
	DM(LOC(USTAT2), I4) = USTAT2;
#else  // SIMD
	// Save the bus exchange registers (PX2 happens implicitly in SIMD)
	DM(LOC(PX1), I4) = PX1;
	// Save the User status registers
	DM(LOC(USTAT1), I4) = USTAT1;
	DM(LOC(USTAT3), I4) = USTAT3;

	// Finished saving PE registers, so leave SIMD mode
	BIT CLR MODE1 PEYEN;
	WA_15000004_1NOP
#endif

	// We have saved the ISR context subset, i.e. everything except
	// the data and DAG registers, but we don't change the thread's
	// contextLevel field yet. This will only be set to INTERRUPTED
	// (in _tmk_SwitchContext_ISR) if we are actually switching away
	// from this thread.

.NoSaveDone:
	// We have most of a C runtime context already established
	// here in the alternate register set, we just have to set
	// up the stack pointer to the base of the main stack and
	// zero the frame pointer (so that stack walks terminate).
	//
    // Set the stack to the (shared) system area, also setting up the
    // circular buffer base and length registers for I6 & I7.
    //
	PX = PM(___lib_stack_space);
	R0 = PX2;
	PX = PM(___lib_stack_length);
	R2 = PX2;

	R1 = R0 + R2;
	R1 = R1 - 1;
    // Load L7,B7, and I7 with interrupts disabled to prevent being
    // interrupted while we have an invalid stack
    JUMP (PC, .SH_INT_DISABLED_10)(DB);
        BIT CLR MODE1 IRPTEN;
	    L7 = R2;
.SH_INT_DISABLED_10:

	B7 = R0; // also sets I7 = R0
	I7 = R1;

#ifdef __2106x__
	B6 = 0; // also sets I6 = 0
	L6 = 0;
#else
	B6 = B7; // also sets I6 = B7
	L6 = L7;
	// I6 gets set to I7 below, so it doesn't need to be zeroed here
#endif

    // Call the Dispatcher. Since this is the only such call it
    // (initially) looks as if we could consider expanding it out
    // in here as hand-written assembly. We would, however, still
    // have to save all the registers and set up the C runtime (i.e.
    // all the above work) in order to be able to invoke the DPCs,
	// so any time saving would be small. (We should never come in
	// here unless there is at least one DPC queued.)
	//
	I12 = DM(_tmk->pfDispatchHook);
	i6=i7;

    BIT SET MODE1 IRPTEN;

	jump (m13,i12) (db);
	dm(i7,m7)=0; // there is no previous frame pointer
	dm(i7,m7)=pc;
.returnFromDispatch:

    // We have blown through the dispatcher without restoring the
    // running thread's context, so we need to do that now. 
	
	// There is a chance may have interrupted the restore of an interrupted 
	// thread from thread level, in which case we'll need to restart the 
	// restore process.
	
	// I3 still contains the thread pointer.
	R1 = DM(OFFSETOF(TMK_Thread,contextLevel), I3);
	R0 = INTERRUPTED;
	COMP(R0, R1);
	
	// Work around for anomaly 15000011
	// Incorrect Execution of VISA CALL(DB) Instructions under specific 
	// conditions so we no longer use call(db) for any processors (though 
	// the anomaly only affects 214xx)

	R0 = NOCONTEXT;
	DM(OFFSETOF(TMK_Thread,contextLevel), I3) = R0;
	IF EQ CALL .SelectAndRestoreAllPrimaryRegs ;

    // If we've fallen through the dispatcher then we need to restore the context
    // of the thread we've interrupted.
.ReturnThroughRescheduleISR:
	// If we've jumped here from RestoreContext, above, then the current thread's
	// ContextLevel field has already been reset to NoContext. However, if we've
	// fallen through from the RescheduleISR then ContextLevel is still ISRContext
	// and we need to set it to NoContext here.

	// Once we're here we know that the secondary registers are selected and
	// hence that we have access to RescheduleISR's C runtime. So in the loops
	// below we can assume that:
	// M6 = 1;	  - used for post-increment
	// L2 = 0;	  - so we can use I2 as the store pointer

	I3 = DM(_tmk->pCurrentThread);		// thread structure pointer
	GLITCHNOP;
	I4 = DM(OFFSETOF(TMK_Thread, pContextSave), I3);

	// Restore the PC Stack (must be empty at this point)
	// Setup some variables to restore the PC stack
#ifdef __2116x__
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	M2 = R0;				// get stack count into M2
	IF EQ JUMP (PC, .RestoreContextISR_DONE_PC_STACK) (DB);  // if stack count is zero then we're done
	   	MODIFY(I2, M2);			// and add it to I2 (the load pointer)
		NOP;
#else
	M2 = LOC(PC_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(PC_STACK_COUNT), I4);	// get the saved stack count
	R0 = PASS R0, MODIFY(I2, M2);	// test it for zero, move the load pointer to the stack region
	IF EQ JUMP (PC, .RestoreContextISR_DONE_PC_STACK) (DB);  // if stack count is zero then we're done
		M2 = R0;				// get stack count into M2
		MODIFY(I2, M2);			// and add it to I2 (the load pointer)
#endif

.RestoreContextISR_PC_STACK:		// top of loop
	PUSH PCSTK;				// current PC goes on stack here...
	R0 = R0 - 1, PCSTK = DM(I2, M7);	// ...and gets replaced here
	IF NE JUMP (PC, .RestoreContextISR_PC_STACK); // pity we can't use the delay slots on this one!

.RestoreContextISR_DONE_PC_STACK:
#ifdef __2116x__
	// Restore the loop stack (must be empty at this point)
	// Setup some variables to restore the LOOP stack
	M2 = LOC(LOOP_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(LOOP_STACK_COUNT), I4); // get the saved stack count
	R0 = R0 + R0, MODIFY(I2, M2);	// R0 = 2 * R0, move load pointer to stack region
	M2 = R0;			// get stack count into M2
	IF EQ JUMP (PC, .RestoreContext_DONE_LOOP_STACK) (DB);  // if stack count is zero then we're done
		R0 = R0 - 1;		// pre-decrement count
		R0 = R0 - 1, MODIFY(I2, M2);	// continue pre-decrement, load ptr += (2 * LOOP_STACK_COUNT)(STALL)
#else
	// Restore the loop stack (must be empty at this point)
	// Setup some variables to restore the loop stack
	M2 = LOC(LOOP_STACK) - 1;			// offset by -1 to allow for post-decrement
	I2 = I4;					// start to set up I2 as the load pointer
	R0 = DM(LOC(LOOP_STACK_COUNT), I4); // get the saved stack count
	R0 = R0 + R0, MODIFY(I2, M2);	// R0 = 2 * R0, move load pointer to stack region
	IF EQ JUMP (PC, .RestoreContext_DONE_LOOP_STACK) (DB);  // if stack count is zero then we're done
		R0 = R0 - 1, M2 = R0;		// pre-decrement count, get stack count into M2
		R0 = R0 - 1, MODIFY(I2, M2);	// continue pre-decrement, load ptr += (2 * LOOP_STACK_COUNT)(STALL)
#endif

// Anomaly workaround for 2137x and 214xx. Can't do direct load from memory to
// the Loop Registers. If the context area memory bank is also being written to
// by the DMA engine, then the loop register values can get corrupted when they
// are being restored.
// This is anomaly 09000023/15000005
.RestoreContext_LOOP_STACK:	// top of loop
#if defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__214xx__)

	// Restore Loop Counters directly from the context area
	PUSH LOOP;
	
	// Get the value of the CURLCNTR from the context area
	R2 = DM(I2,M7);
	CURLCNTR = R2;
	
	// Get the value of the LADDR from the context area
	R2 = DM(I2,M7);
	LADDR = R2;

	IF EQ JUMP	(PC, .RestoreContext_DONE_LOOP_STACK);

	JUMP (PC, .RestoreContext_LOOP_STACK) (db);
		// Decrement count	
		R0 = R0 - 1;
		R0 = R0 - 1;
#else
	PUSH LOOP;
	IF NE JUMP (PC, .RestoreContext_LOOP_STACK)(DB);	// R0 == 0 on final iteration
		R0 = R0 - 1, CURLCNTR = DM(I2, M7); // post-decrement count, load CURLCNTR from saved context
		R0 = R0 - 1, LADDR	  = DM(I2, M7); // cont. post-decrement count, load LADDR from saved context
#endif		

.RestoreContext_DONE_LOOP_STACK:

#ifdef __214xx__ /* Phoenix only */
	// Restore the bit fifo
	R0 = DM(LOC(_BFFWRP),    I4);
	R1 = DM(LOC(BitFIFO_0), I4);
	R2 = DM(LOC(BitFIFO_1), I4);
	BFFWRP = 0;
	BITDEP R2 BY 32;
	BITDEP R1 BY 32;
	BFFWRP = R0;
#endif

#ifndef __2106x__
	// Enter SIMD mode to restore PE registers in parallel
	BIT SET MODE1 PEYEN;
#endif

// Anomaly workaround for 2137x and 214xx. Can't do direct load from memory to
// the Loop Registers. If the context area memory bank is also being written to
// by the DMA engine, then the loop register values can get corrupted when they
// are being restored.
// This is anomaly 09000023/15000005
#if defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__214xx__)
	R0 = DM(LOC(LCNTR), I4);
	LCNTR = R0;		// always restore LCNTR (same in SIMD or SISD)
#else
	LCNTR = DM(LOC(LCNTR), I4);		// always restore LCNTR (same in SIMD or SISD)
#endif

	// We restore the multiplier result registers now, before MODE1
	// has been restored, i.e. while the SRCU bit is still clear.
	// ASTAT is restored later, so it doesn't matter that loading
	// the MR registers will clear the M* flag bits.
			   R0 = DM(LOC(MR0F), I4);
#ifdef __2137x__
	MR0F = R0; R0 = DM(LOC(MR1F), I4);	  // NOTE: semicolons instead of commas!
	MR1F = R0; R0 = DM(LOC(MR2F), I4);	  // Anomaly 09000018 means that we can't
	MR2F = R0; R0 = DM(LOC(MR0B), I4);	  // move to a multiplier register in
	MR0B = R0; R0 = DM(LOC(MR1B), I4);	  // parallel with a memory access, in case
	MR1B = R0; R0 = DM(LOC(MR2B), I4);	  // context record is in external mem.
#else
	MR0F = R0, R0 = DM(LOC(MR1F), I4);
	MR1F = R0, R0 = DM(LOC(MR2F), I4);
	MR2F = R0, R0 = DM(LOC(MR0B), I4);
	MR0B = R0, R0 = DM(LOC(MR1B), I4);
	MR1B = R0, R0 = DM(LOC(MR2B), I4);
#endif
	MR2B = R0;

	// Restore the STKY flags, nothing that we do from
	// here on out should change these
	STKY = DM(LOC(STKY), I4);

	// We are (still) in the secondary registers, but
	// the processor has *not* been completely restored at this time
	// (ASTAT and MODE1 are still in their respective globals).
	// Restore the remaining registers before returning to the thread domain.
#ifdef __2106x__
	// Restore the bus exchange registers
	PX1 = DM(LOC(PX1), I4);
	PX2 = DM(LOC(PX2), I4);

	// Restore the User status registers
	USTAT1 = DM(LOC(USTAT1), I4);
	USTAT2 = DM(LOC(USTAT2), I4);

	ASTAT = DM(.ContextASTAT);	
#else // SIMD
	// Restore ASTATx and ASTATy. The only way of getting at ASTATy is to
	// do this while in SIMD mode.
	ASTAT = DM(LOC(ASTAT), I4);

	// Restore the bus exchange registers (PX2 implicit in SIMD)
	PX1 = DM(LOC(PX1), I4);

	// Restore the User status registers
	USTAT1 = DM(LOC(USTAT1), I4);
	USTAT3 = DM(LOC(USTAT3), I4);
#endif

	// We have finished restoring PE registers, but we don't need to
	// explicitly leave SIMD mode as this will happen when MODE1 is
	// restored, below (or not, if SIMD mode was in effect at the
	// time of the interrupt).
	//
	R0 = DM(.ContextMODE1);   // ok to trash secondary R0

	RTI (DB);
		MODE1 = R0;   // return to original register set
		NOP;
    
._tmk_RescheduleISR.end:


_tmk_RunOnSystemStack:
	// Set the stack to the (shared) system area, also setting up the
	// circular buffer base and length registers for I6 & I7.
	//
#ifndef __2106x__
    // safe wrt. anomaly 09000022/15000004 as can't be reached from path above
.message/suppress 2547 for 2 lines;
#endif
	PX = PM(___lib_stack_space);
	R0 = PX2;
	PX = PM(___lib_stack_length);
	R2 = PX2;

	R1 = R0 + R2;
    R1 = R1 - 1;

	// Load L7,B7, and I7 with interrupts disabled to prevent being
	// interrupted while we have an invalid stack
    JUMP (PC, .SH_INT_DISABLED_11)(DB);
        BIT CLR MODE1 IRPTEN;
	    L7 = R2;
.SH_INT_DISABLED_11:
	B7 = R0; // also sets I7 = R0
	I7 = R1;

#ifdef __2106x__
	B6 = 0; // also sets I6 = 0
	L6 = 0;
#else
	B6 = B7; // also sets I6 = B7
	L6 = L7;
	I6 = 0;
#endif

	i12 = R12; // third argument is function ptr

	// First and second args (old and new thread ptrs) remain in R4 and R8.
	r2=i6;
	i6=i7;

    BIT SET MODE1 IRPTEN;

	jump (m13,i12) (db);
	dm(i7,m7)=r2;
	dm(i7,m7)= pc;

._tmk_RunOnSystemStack.end:


_tmk_SwitchContext_KillThread:

	JUMP .RestoreAfterKill;

._tmk_SwitchContext_KillThread.end:
