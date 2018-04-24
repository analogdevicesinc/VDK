/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public mempool create API
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
#pragma file_attr("SignalCreation")
#pragma file_attr("MemPoolCreation")
#pragma file_attr("FuncName=CreatePoolEx")
#pragma file_attr("FuncName=CreatePool")

namespace VDK {
// Creates a memory pool object and puts it in the next avaible slot in the pool 
// array. returns -1 upon failure on un-instrumented build, calls error handler
// in the instrumented build with kPoolCreationFailure. It allows the users to 
// specify on which heap the pool should get created.
//
//  Errors: In Instrumented build kInvalidPoolParms for any invalid pool parameters.  
//  					kPoolCreationFailure in case of max limit of the pools is reached.
//
//  	    It will return -1 for any type of failure and will 
//  	    dispatch errors.
//
//  	    Note: 0 is a vaild pool-id
//  	    
//  	    Need to be modified if they are multiple heaps
//  	    
//  	           Support req       Support not req
//  	           ----------        ---------------
//  	    	1. TigerSharc         1. BlackFin
//  	    	2. HammerHead.        2. Sharc
//  	    	
//
//
PoolID CreatePoolEx(unsigned int inBlockSz, const unsigned int inNumBlocks, const bool inCreateNow,int inWhichHeap)
{
	PoolID thePoolID = static_cast <PoolID> (UINT_MAX);

	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	if ERRCHK( 0 != (inBlockSz * inNumBlocks))
	{
		TMK_SuspendScheduling();

		thePoolID = g_MemoryPoolTable.GetNextObjectID();

		if(thePoolID != UINT_MAX)
		{
			void * pool_addr = _heap_malloc(inWhichHeap, sizeof(MemoryPool));
			MemoryPool *newObj = new (pool_addr) MemoryPool(thePoolID);

			if(newObj && (newObj->AllocateMemoryPool(inBlockSz,inNumBlocks,inCreateNow,inWhichHeap)))
			{
				g_MemoryPoolTable.AddObject(newObj);
				HISTORY_(kPoolCreated,thePoolID,GetThreadID());
			}
			else
			{
				// This case arises when pool object is allocated properly but
				// 'malloc' failed for actual pool memory.
				//
				if(newObj)
				{
					newObj->~MemoryPool();
					_heap_free(inWhichHeap, pool_addr);
				}
				thePoolID = static_cast <PoolID> (UINT_MAX);
			}
		}

		TMK_ResumeScheduling();
	
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	  if(thePoolID == UINT_MAX)
			DispatchThreadError(kPoolCreationFailure,0);
#endif
	} /* if inNumBlocks * inBlockSz = 0 */
	else
		DispatchThreadError(kInvalidPoolParms,0);

	return(thePoolID);
}

PoolID CreatePool(unsigned int inBlockSz, const unsigned int inNumBlocks, const bool inCreateNow)
{
	// inWhichHeap argument should be the default
	return CreatePoolEx(inBlockSz, inNumBlocks,inCreateNow,0);
}

}
