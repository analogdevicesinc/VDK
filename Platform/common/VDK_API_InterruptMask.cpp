/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The interrupt mask handling routines
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "TMK.h"

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "VDK_AsmMacros.h"
#include "BitOps.h"
#pragma file_attr("System")


namespace VDK
{
    extern IMASKStruct  g_InitialISRMask;
	extern IMASKStruct  g_InitialISRMask2;
    extern int g_CriticalRegionCount;
    extern int g_SchedulerRegionCount;
	extern TMK_Lock      g_CriticalRegionLock;
	extern TMK_LockState g_CriticalRegionState;


IMASKStruct
GetInterruptMask()
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	TMK_IMaskUInt result = TMK_GetInterruptMaskBits(&g_CriticalRegionLock, g_CriticalRegionState);

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);

	return static_cast<IMASKStruct>(result);
}


void
SetInterruptMaskBits(const IMASKStruct inValue)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	TMK_ChangeInterruptMaskBits(&g_CriticalRegionLock, &g_CriticalRegionState, inValue, 0);

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}


void
ClearInterruptMaskBits(const IMASKStruct inValue)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	TMK_ChangeInterruptMaskBits(&g_CriticalRegionLock, &g_CriticalRegionState, 0, inValue);

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}

#if defined(__2116x__) || defined(__2126x__) || defined(__2136x__) || defined(__2137x__) || defined (__214xx__)
#include <sysreg.h>

void
SetInterruptMaskBitsEx(const IMASKStruct inValue1, const IMASKStruct inValue2)
{
	unsigned int lmask = inValue2;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	// Hammerheads have a different LIRPTL layout from SHARC EX
	//
#if defined(__2116x__) // Hammerhead
	lmask |= lmask << 16;
	lmask &= 0x00FF0000;
#else
	lmask |= lmask << 10;
	lmask &= 0x000FFC00;
#endif
	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	TMK_ChangeInterruptMaskBits(&g_CriticalRegionLock, &g_CriticalRegionState, inValue1, 0);

	//    g_InitialISRMask  |= inValue1;
    g_InitialISRMask2 |= lmask;
    //add the mask bits we require
	while (lmask) {
	  unsigned int nextBit = FindFirstSet(lmask);
#pragma diag(push)
#pragma diag(suppress:127: "suppress warning expected a statement in no error check libraries")
	  switch (nextBit) {
#if defined(__2116x__) // Hammerhead
	  case 16:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x10000);
	    break;
	  case 17:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x20000);
	    break;
	  case 18:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x40000);
	    break;
	  case 19:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x80000);
	    break;
	  case 20:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x100000);
	    break;
	  case 21:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x200000);
	    break;
#else
	  case 10:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x400);
	    break;
	  case 11:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x800);
	    break;
	  case 12:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x1000);
	    break;
	  case 13:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x2000);
	    break;
	  case 14:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x4000);
	    break;
	  case 16:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x10000);
	    break;
	  case 18:
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x40000);
	    break;
	  case 19:		
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x80000);
	    break;
#if defined (__2136x__) || defined (__2137x__) || defined (__214xx__)
	  case 15:		// available in 2136x
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x8000);
	    break;
	  case 17:		// available in 2136x
	    __builtin_sysreg_bit_set(sysreg_LIRPTL, 0x20000);
	    break;
#endif
#endif
	  default:
#if VDK_INSTRUMENTATION_LEVEL_ > 0
	    DispatchThreadError(kInvalidMaskBit, nextBit);
#endif
	  }
#pragma diag(pop)
	  lmask &= ~(1 << nextBit);//clear the bit from the mask to do
	}

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}


void
ClearInterruptMaskBitsEx(const IMASKStruct inValue1, const IMASKStruct inValue2)
{
	unsigned int lmask = inValue2;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	// Hammerheads have a different LIRPTL layout from SHARC EX
	//
#if defined(__2116x__) // Hammerhead
	lmask |= lmask << 16;
	lmask &= 0x00FF0000;
#else                  // SHARC EXn
	lmask |= lmask << 10;
	lmask &= 0x000FFC00;
#endif

	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	TMK_ChangeInterruptMaskBits(&g_CriticalRegionLock, &g_CriticalRegionState, 0, inValue1);

	//    g_InitialISRMask  &= ~inValue1;
    g_InitialISRMask2 &= ~lmask;
	while (lmask) {
	  unsigned int nextBit = FindFirstSet(lmask);
#pragma diag(push)
#pragma diag(suppress:127: "suppress warning expected a statement in no error check libraries")
	  switch (nextBit) {
#if defined(__2116x__) // Hammerhead
	  case 16:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x10000);
	    break;
	  case 17:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x20000);
	    break;
	  case 18:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x40000);
	    break;
	  case 19:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x80000);
	    break;
	  case 20:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x100000);
	    break;
	  case 21:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x200000);
	    break;
#else
	  case 10:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x400);
	    break;
	  case 11:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x800);
	    break;
	  case 12:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x1000);
	    break;
	  case 13:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x2000);
	    break;
	  case 14:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x4000);
	    break;
	  case 16:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x10000);
	    break;
	  case 18:
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x40000);
	    break;
	  case 19:		
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x80000);
	    break;
#if defined (__2136x__) || defined (__2137x__)  || defined (__214xx__)
	  case 15:		// available in 2136x
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x8000);
	    break;
	  case 17:		// available in 2136x
	    __builtin_sysreg_bit_clr(sysreg_LIRPTL, 0x20000);
	    break;
#endif
#endif
	  default:
#if VDK_INSTRUMENTATION_LEVEL_ > 0
	    DispatchThreadError(kInvalidMaskBit, nextBit);
#endif
	  }
#pragma diag(pop)
	  lmask &= ~(1 << nextBit);//clear the bit from the mask to do
	}

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}

void
GetInterruptMaskEx(IMASKStruct* outValue1, IMASKStruct* outValue2)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

	if (0 == g_CriticalRegionCount)
		g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);

	*outValue1 = TMK_GetInterruptMaskBits(&g_CriticalRegionLock, g_CriticalRegionState);
	*outValue2 = g_InitialISRMask2;

	if (0 == VDK::g_CriticalRegionCount)
		TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}
#endif


} // namespace VDK


