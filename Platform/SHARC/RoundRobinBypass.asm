// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************


//  Architecture: ADSP-TSxxx

#include "ADI_attributes.h"
.import "TMK.h"			; 

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM seg_pmco;
#endif

.extern _RoundRobinRuntimeHookFn__3VDKFP10TMK_ThreadT1;
.extern _RoundRobinInterruptHookFn__3VDKFP10TMK_ThreadT1;
.extern _g_RoundRobinInitPriority__3VDK;

.global _RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1;
.global _RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1;

.align 2;
_RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1:
	I4 = R4;                                       // I4 = pOldThread
	R0 = DM(_g_RoundRobinInitPriority__3VDK);      // R0 = g_RoundRobinInitPriority
	R1 = DM(OFFSETOF(TMK_Thread,priority), I4);    // XR1 = pOldThread->priority
	I4 = R8;                                       // I4 = pNewThread
	BTST R0 BY R1;
	R1 = DM(OFFSETOF(TMK_Thread,priority), I4);    // XR1 = pNewThread->priority
	IF NSZ JUMP _RoundRobinRuntimeHookFn__3VDKFP10TMK_ThreadT1;

	BTST R0 BY R1;
	I4 = DM(_pfInterruptRRChain__3VDK);
	IF NSZ JUMP _RoundRobinRuntimeHookFn__3VDKFP10TMK_ThreadT1;
	
	JUMP(M5,I4);

._RoundRobinRuntimeBypass__3VDKFP10TMK_ThreadT1.end:

	.align 2;
_RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1:
	I4 = R4;                                       // I4 = pOldThread
	R0 = DM(_g_RoundRobinInitPriority__3VDK);      // R0 = g_RoundRobinInitPriority
	R1 = DM(OFFSETOF(TMK_Thread,priority), I4);    // XR1 = pOldThread->priority
	I4 = R8;                                       // I4 = pNewThread
	BTST R0 BY R1;
	R1 = DM(OFFSETOF(TMK_Thread,priority), I4);    // XR1 = pNewThread->priority
	IF NSZ JUMP _RoundRobinInterruptHookFn__3VDKFP10TMK_ThreadT1;

	BTST R0 BY R1;
	I4 = DM(_pfInterruptRRChain__3VDK);
	IF NSZ JUMP _RoundRobinInterruptHookFn__3VDKFP10TMK_ThreadT1;
	
	JUMP(M5,I4);

._RoundRobinInterruptBypass__3VDKFP10TMK_ThreadT1.end:
