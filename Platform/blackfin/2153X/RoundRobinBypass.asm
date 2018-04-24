// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************


//  Architecture: ADSP-BF53x

.import "TMK.h"			; 

.section/DOUBLEANY program;

.extern _RoundRobinRuntimeHookFn__3VDKFP10TMK_ThreadT1;
.extern _RoundRobinInterruptHookFn__3VDKFP10TMK_ThreadT1;
.extern _g_RoundRobinInitPriority__3VDK;

.global _RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1;
.global _RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1;

.align 2;
_RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1:
	P0 = R0;
	P1 = R1 ;
	P2.L = _g_RoundRobinInitPriority__3VDK;
	P2.H = _g_RoundRobinInitPriority__3VDK;

	R3 = B[P0 + OFFSETOF(TMK_Thread,priority)] (Z);  // R1 = pOldThread->priority
	R1 = 1;
	P0.L = _pfRuntimeRRChain__3VDK;
	P0.H = _pfRuntimeRRChain__3VDK;
	R1 <<= R3;

	R3 = B[P1 + OFFSETOF(TMK_Thread,priority)] (Z);  // R1 = pNewThread->priority
	R2 = 1;
	R2 <<= R3;
	
	R3 = [P2];   // R2 = g_RoundRobinInitPriority

	R2 = R2 | R1;
	R2 = R2 & R3;

	CC = R2;

	P2 = [P0];
	// R0 should be unchanged from entry
	R1 = P1; // but R1 needs to be restored

	IF CC JUMP _RoundRobinRuntimeHookFn__3VDKFP10TMK_ThreadT1;

	JUMP (P2);

._RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1.end:

	.align 2;
_RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1:
	P0 = R0;
	P1 = R1 ;
	P2.L = _g_RoundRobinInitPriority__3VDK;
	P2.H = _g_RoundRobinInitPriority__3VDK;

	R3 = B[P0 + OFFSETOF(TMK_Thread,priority)] (Z);  // R1 = pOldThread->priority
	R1 = 1;
	P0.L = _pfInterruptRRChain__3VDK;
	P0.H = _pfInterruptRRChain__3VDK;
	R1 <<= R3;

	R3 = B[P1 + OFFSETOF(TMK_Thread,priority)] (Z);  // R1 = pNewThread->priority
	R2 = 1;
	R2 <<= R3;
	
	R3 = [P2];   // R2 = g_RoundRobinInitPriority

	R2 = R2 | R1;
	R2 = R2 & R3;

	CC = R2;

	P2 = [P0];
	// R0 should be unchanged from entry
	R1 = P1; // but R1 needs to be restored

	IF CC JUMP _RoundRobinInterruptHookFn__3VDKFP10TMK_ThreadT1;

	JUMP (P2);

._RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1.end:
