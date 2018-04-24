/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The thread-local storage API
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The implementations for thread-local storage.  This API allows 
 * 			libraries to store information on a per-thread basis.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_Thread.h"
#include "VDK_API.h"
#include "VDK_Macros.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


namespace VDK
{
	bool
	AllocateThreadSlot(int *ioSlotNum)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		return VDK::Thread::AllocateTLSSlot(ioSlotNum, NULL);
	}

	bool
	AllocateThreadSlotEx(int *ioSlotNum, void (*pfCleanup)(void *))
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		return VDK::Thread::AllocateTLSSlot(ioSlotNum, pfCleanup);
	}

	bool
	SetThreadSlotValue(int inSlotNum, void *inValue)
	{
		if NERRCHK(IS_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

	    TMK_Thread *pThread = TMK_GetCurrentThread();

	    if (NULL == pThread)
			return false;

#pragma suppress_null_check
	    Thread *pVdkThread = static_cast<Thread*>(pThread);
		return pVdkThread->SetTLSSlotNum(inSlotNum, inValue);
	}

	void*
	GetThreadSlotValue(int inSlotNum)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	    TMK_Thread *pThread = TMK_GetCurrentThread();

	    if (NULL == pThread)
			return NULL;

#pragma suppress_null_check
	    Thread *pVdkThread = static_cast<Thread*>(pThread);
		return pVdkThread->GetTLSSlotValue(inSlotNum);
	}

	bool
	FreeThreadSlot(int inSlotNum)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		return VDK::Thread::FreeTLSSlot(inSlotNum);
	}

}  // namespace VDK





/**
 * Thread local storage stuff
 *
 * These APIs are for internal use only.  They allow libraries to associate
 * state with specific threads, allowing reentrancy for libraries that require
 * static state variables.
 */

//
// We CANNOT use a Bitfield for the UnusedSlots, because static initialisers run
// *after* the C runtime library gets initialised. This would cause the slot
// allocation for CLIB to be forgotten.
//
unsigned int        VDK::Thread::c_UnusedSlots = 0xFF;    // kMaxNumDataSlots == 8
void               (*VDK::Thread::c_CleanupFns[VDK::Thread::kMaxNumDataSlots]) (void *);

bool
VDK::Thread::AllocateTLSSlot(int *ioSlotNum, void (*pfCleanup)(void *))
{
	bool result = false;

	TMK_AcquireMasterKernelLock();

	// 
	if (*ioSlotNum == VDK::kTLSUnallocated)
	{
		int slotNum = FindFirstSet(c_UnusedSlots);
			
		if (VDK::kNoneSet != slotNum)
		{
			c_UnusedSlots &= ~(1 << slotNum);
			*ioSlotNum = slotNum;
			c_CleanupFns[slotNum] = pfCleanup;
			result = true;
		}
	}

	TMK_ReleaseMasterKernelLock();

	return result;
}


bool
VDK::Thread::SetTLSSlotNum(int inSlotNum, void *inValue)
{
	if ((unsigned)inSlotNum >= kMaxNumDataSlots ||
		IsBitSet(c_UnusedSlots, inSlotNum))
		return false;

	m_SlotValue[inSlotNum] = inValue;

	return true;
}


void* 
VDK::Thread::GetTLSSlotValue(int inSlotNum)
{
	if ((unsigned)inSlotNum >= kMaxNumDataSlots ||
		IsBitSet(c_UnusedSlots, inSlotNum))
		return NULL;

	return m_SlotValue[inSlotNum];
}

//
// Should really be in a separate module, as it's unlikely
// to be required by applications.
//
bool
VDK::Thread::FreeTLSSlot(int inSlotNum)
{
	if ((unsigned)inSlotNum >= kMaxNumDataSlots ||
		IsBitSet(c_UnusedSlots, inSlotNum))
		return false;

	TMK_AcquireMasterKernelLock();
	c_UnusedSlots |= (1 << inSlotNum);
	TMK_ReleaseMasterKernelLock();

	return true;
}






