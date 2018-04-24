/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public mempool destroy API
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source: /home/cvsusr/MASTERROOT/VDK/mainline/common/VDK_API_PoolMgr.cpp,v
$
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
#pragma file_attr("SignalDestruction")
#pragma file_attr("MemPoolDestruction")
#pragma file_attr("FuncName=DestroyPool")

namespace VDK {

// 
//  Deletes the pool from the system, right now does not care about dangling
//  references.
//
void DestroyPool(const PoolID inPoolID)
{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	TMK_SuspendScheduling();

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);

	if(pMemoryPool != NULL)
	{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if(!pMemoryPool->IsPoolEmpty())
		{
			TMK_ResumeScheduling();
			DispatchThreadError(kErrorPoolNotEmpty,inPoolID);
			return;
		}
#endif
		// We came here means we are freeing the pool
		//
		g_MemoryPoolTable.RemoveObject(inPoolID);
		int whichHeap = pMemoryPool->WhichHeap();
		pMemoryPool->~MemoryPool(); //call destructor
		_heap_free(whichHeap,pMemoryPool);
    	HISTORY_(kPoolDestroyed,inPoolID,GetThreadID());
	}
	
	TMK_ResumeScheduling();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if(NULL == pMemoryPool)
		DispatchThreadError(kInvalidPoolID,inPoolID);
#endif
}

}
