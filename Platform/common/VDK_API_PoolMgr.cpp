/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
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
#include "vdk_macros.h"
#include "vdk_MemPool.h"
#include "MemoryPoolTable.h"
#include "VDK_History.h"
#include "VDK_Thread.h"
#include <limits.h>
#include <stdlib.h>
#include <new>
#include "Heap.h"
#pragma file_attr("OS_Component=MemoryPools")
#pragma file_attr("MemoryPools")
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")


namespace VDK
{

	static TMK_Lock s_lock = TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(TMK_MAX_LOCK_PRECEDENCE);

//
//
// Allocates a block from the pool specified. Upon successful return the outBlkPtr
// will have the start of the block address.
//
// Works analogous to c-runtime malloc. It will return NULL upon failure. Will not call error
// handler for any type of errors.
//
void* MallocBlock(const PoolID inPoolID)
{
	void *pBlock=NULL;

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

	if(NULL != pMemoryPool)
		pBlock = pMemoryPool->GetFreeBlock();
	    
	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (NULL == pMemoryPool)
			DispatchThreadError(kInvalidPoolID,inPoolID);
		else if (NULL == pBlock)
			DispatchThreadError(kErrorMallocBlock,pMemoryPool->NumFreeBlocks());
#endif /* VDK_INSTRUMENTATION_LEVEL_ */

	return pBlock;
}

//
// Free's a block from the specified pool
// 
//  FreeBlock will throw an error in Instrumented build for any Invalid pointers. It will 
//  not return any error in un-instrumented builds.
//
void FreeBlock(const PoolID inPoolID, void *inBlockPtr)
{
	bool ret=false;

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);
	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

     if (NULL != pMemoryPool) 
	    ret = pMemoryPool->FreeBlock(inBlockPtr);

	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if(NULL == pMemoryPool)
		DispatchThreadError(kInvalidPoolID,inPoolID); 
	else if(!ret)
		DispatchThreadError(kInvalidBlockPointer,reinterpret_cast<unsigned int>(inBlockPtr)); 
#endif
}

//
// Get number of free blocks of a given pool. Users put it in critical/unscheduled
// region if they want to get it atomically.
//
// Instrumented for Invalid pool ID throws an error with kInvalidPoolParms and returns -1.
// Un instrumetned builds it will return -1 for invalid pool-id
// 
unsigned int GetNumFreeBlocks(const PoolID inPoolID)
{
	unsigned int blockCount = UINT_MAX;

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

	if (NULL != pMemoryPool)
	 	 blockCount = pMemoryPool->NumFreeBlocks();

	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if(NULL == pMemoryPool)
		DispatchThreadError(kInvalidPoolID,inPoolID);
#endif 

	return(blockCount);
}

//
// Get number of allocated blocks of a given pool. Users are expected to call it in
// critical or unscheduled region to ensure atomicity.
//
unsigned int GetNumAllocatedBlocks(const PoolID inPoolID)
{
	unsigned int blockCount = UINT_MAX;

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

	if(pMemoryPool != NULL)
	    blockCount = pMemoryPool->NumAllocBlocks();

	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if(NULL == pMemoryPool)
		DispatchThreadError(kInvalidPoolID,inPoolID);
#endif

	return blockCount;
}

unsigned int
GetBlockSize (const PoolID inPoolID)
{
	unsigned int blockSize=UINT_MAX;

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

	if(pMemoryPool != NULL)
	    blockSize = pMemoryPool->BlkSize();

	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if(NULL == pMemoryPool)
		DispatchThreadError(kInvalidPoolID, inPoolID);
#endif

	return blockSize;
}


//
// Locate the pool to which the passed block pointer belongs and then free it.
// If not found in the instrumented build it will throw an error.
// 
void LocateAndFreeBlock(void *inBlkPtr)
{
	bool success = false;
	
	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_LockState state = LCK_AcquireRescheduleInterruptLock(&s_lock);

	for (int i=0; i < g_MemoryPoolTable.m_TableSize; i++)
	{
	    MemoryPool *pMemoryPool =
		static_cast<MemoryPool*>(g_MemoryPoolTable.m_IDTable[i].m_ObjectPtr);

	    if (NULL != pMemoryPool)
	    {	
		if (pMemoryPool->IsThisPoolPtr(inBlkPtr))
		{
		    success = pMemoryPool->FreeBlock(inBlkPtr);
		    break;
		}
	    }
	}

	LCK_ReleaseRescheduleInterruptLock(&s_lock, state);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if(!success)
		  DispatchThreadError(kInvalidBlockPointer,reinterpret_cast<unsigned int>(inBlkPtr)); 
#endif 
}
} /* namespace VDK */
