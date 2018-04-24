/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: APIs that relate to the status information of memory pools
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

namespace VDK { 

SystemError
GetPoolDetails( const PoolID  inPoolID,	
			    unsigned int  *outNumBlocks, 
				void		  **outMemoryAddress )
{

	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	SystemError errorCode = kInvalidPoolID;
	
	//Check of the Pool exists
	TMK_AcquireMasterKernelLock();

	MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(inPoolID);
	if(pMemoryPool != NULL)
	{
		//Pool exists
		if(outNumBlocks != NULL)
		{
	    	*outNumBlocks		= pMemoryPool->m_TotalNumBlks;
		}
		if(outMemoryAddress != NULL)
		{
			*outMemoryAddress	= static_cast<void *>(pMemoryPool->m_PoolStart);
		}
		errorCode = kNoError;
	}
	TMK_ReleaseMasterKernelLock();

	return errorCode;
}


int
GetAllMemoryPools( PoolID outPoolIDArray[], int inArraySize )
{
	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	// If we are to fill an array, check the pointer is not NULL
	if NERRCHK( (inArraySize != 0) && (outPoolIDArray == NULL) )
		DispatchThreadError( kInvalidPointer, 0);

	// Disable scheduling while we access a global table
	TMK_AcquireMasterKernelLock();
	int count = g_MemoryPoolTable.GetAllObjectIDs(outPoolIDArray, inArraySize);
	TMK_ReleaseMasterKernelLock();

	return count;
}

} // namespace VDK
