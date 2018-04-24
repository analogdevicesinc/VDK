/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"
#include "vdk_macros.h"
#include "vdk_MemPool.h"
#include "MemoryPoolTable.h"
#include "VDK_History.h"
#include <limits.h>
#include "KernelPanic.h"
#pragma file_attr("OS_Component=MemoryPools")
#pragma file_attr("MemoryPools")
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")
#pragma file_attr("Startup")

using namespace VDK;
// Creates boot pools
//
namespace VDK
{
	bool InitBootMemoryPools(void)
	{
	extern unsigned int kNumBootPools;
	extern struct BootPoolInfo g_BootMemoryPools[];
	PoolID expetedPoolID = static_cast<PoolID> (UINT_MAX);

		for(int i = 0; i < kNumBootPools; i++)
		{
			// Call the API that creates the semaphore
			//
			expetedPoolID = CreatePoolEx(g_BootMemoryPools[i].inBlockSize,g_BootMemoryPools[i].inNumBlocks,g_BootMemoryPools[i].inLazyCreate,g_BootMemoryPools[i].inWhichHeap);

			// We should check the value given by ID table manager
			// 
			if (expetedPoolID != g_BootMemoryPools[i].inPoolID)
			{
				KernelPanic(kBootError, kPoolCreationFailure,i);
				// Log History event  
				// TODO: common boot error instead of per type ??
				// HISTORY_(VDK::kBootPoolCreateFailure,g_BootMemoryPools[i],0) 
				//
				if (expetedPoolID != UINT_MAX)
					DestroyPool(expetedPoolID);
			}
		}
		return true;
	}
} /* namespace vdk */
