/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "TMK.h"

#include "VDK_Thread.h"
#include "VDK_StackMeasure.h"
#include "ThreadTable.h"
#include <sysreg.h>
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma diag(suppress:1434)

// different processors use different registers for the stack pointer so we
// take that into account
#if __ADSP21000__
#define store_stack "%0=I7;"
#elif __ADSPBLACKFIN__
#define store_stack "%0=SP;"
#elif __ADSPTS__ 
#define store_stack "%0=J27;;"
#define store_stack2 "%0=K27;;"
#endif

namespace VDK
{
#if VDK_INSTRUMENTATION_LEVEL_ == 2 

//
// Fills the memory from the starting address up to the specified size with the inFillData
// the addressable unit is "unsigned int"  for these locations.
//
void 
FillMemory(unsigned int *inStartAddr,const unsigned int inMemSize, const unsigned int inFillData)
{
	int i;
	unsigned int *ptr;

	if (inStartAddr)
	{
		ptr = inStartAddr;
	
		for (i = 0; i < inMemSize; i++)
			*ptr-- = inFillData;
	}
}
#endif

//
// Returns the amount of consumed memory.
//
unsigned int
CheckMemory(unsigned int *inStartAddr,const unsigned int inMemSize, const unsigned int inFillData)
{
	int count=0;

	// if we didn't manage to allocate any stack, there is nothing to check
	if (NULL == inStartAddr)
		return 0;

	unsigned int *ptr = inStartAddr - inMemSize + 1;
	
	for(int i=0; i < inMemSize; i++)
	{
		if( *ptr == inFillData)
		{
			count++; ptr++;
		}
		else 
			break;
	}

	return(inMemSize - count);
}

void InstrumentStackNum(unsigned int inStackNum)
{
	unsigned int stack_size;
	unsigned int *sp_ptr=NULL;

    // Find the thread
	TMK_AcquireMasterKernelLock();
	Thread* ThreadPtr = g_ThreadTable.GetObjectPtr( GetThreadID());

	if( ThreadPtr != 0 )
	{
		// Get sp and calculate the size we have to fill in
#if __ADSPTS__
		// Tigersharc is the only processor in which the threads have two stacks
		if (inStackNum == 1) {
			asm(store_stack:"=j"(sp_ptr):"j"(sp_ptr));
			sp_ptr--;
			// in ThreadMemoryAlloc.cpp we do m_StackP += m_StackSize - 4 
			// so we take the 4 into account
			stack_size = (ThreadPtr->m_StackSize - 3)  - (ThreadPtr->m_StackP - sp_ptr);
		}
		else 
		{
			asm(store_stack2:"=k"(sp_ptr):"k"(sp_ptr));
			sp_ptr--;
			// in ThreadMemoryAlloc.cpp we do m_StackP2 += m_StackSize2 - 4 
			// so we take the 4 into account
			stack_size = (ThreadPtr->m_StackSize2 - 3) - (ThreadPtr->m_StackP2 - sp_ptr);
		}
#else
		asm(store_stack:"=d"(sp_ptr):"d"(sp_ptr));
		sp_ptr--;
		stack_size = ThreadPtr->m_StackSize - (ThreadPtr->m_StackP - sp_ptr);
#endif
		while (stack_size--)
			*sp_ptr-- = STACK_FILL_VALUE;
	}
	
	TMK_ReleaseMasterKernelLock();
}

void
InstrumentStack( )
{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#if __ADSPTS__
	InstrumentStackNum(2);
#endif
	InstrumentStackNum(1);
}

/**
 * Gets the maximum stack used
 */
unsigned int Thread::GetStackUsed()
{
#ifdef __ADSPTS__
        return(CheckMemory(m_StackP + 3,m_StackSize,STACK_FILL_VALUE));
#elif __ADSPBLACKFIN__
        return(CheckMemory(m_StackP,m_StackSize,STACK_FILL_VALUE));
#else /* SHARC */
        return(CheckMemory(m_StackP,m_StackSize,STACK_FILL_VALUE));
#endif
}

#ifdef __ADSPTS__
unsigned int VDK::Thread::GetStackUsed2()
{
        return(CheckMemory(m_StackP2 + 3,m_StackSize2,STACK_FILL_VALUE));
}
#endif

#ifdef __ADSPTS__
bool Thread::StackGuardWordChanged()
{
    unsigned int *pGuardWordJ = m_StackP  + 4 - m_StackSize;
    unsigned int *pGuardWordK = m_StackP2 + 4 - m_StackSize2;

    return (*pGuardWordJ != STACK_FILL_VALUE) || (*pGuardWordK != STACK_FILL_VALUE);
}
#else
bool Thread::StackGuardWordChanged()
{
    unsigned int *pGuardWord = m_StackP;

#if defined(__ADSPTS__) || defined(__ADSPBLACKFIN__)
    pGuardWord += 4 - m_StackSize;
#else
    pGuardWord += 1 - m_StackSize;
#endif

    return (*pGuardWord != STACK_FILL_VALUE);
}
#endif

} /* VDK namespace */

