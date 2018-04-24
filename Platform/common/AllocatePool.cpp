/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation AllocateMemoryPool for the different 
 *   architectures
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

#include "VDK_MemPool.h"
#include <stdlib.h>
#pragma file_attr("OS_Component=MemoryPools")
#pragma file_attr("MemoryPools")
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")

//
// Allocates memory, by computing with paramentes Block Size and Num Blocks
// Blocks will be allocated on demand.Once they are allocated they will be
// managed like a heap,i.e.,BlockTop will always point to the next free block
// it will be 0 if there is no block available.
// Allocate pool call is expected to call from an unscheduled region from the
// higher level API.
// Block Size is the size of the block in the smallest addressable units.
//
bool 
VDK::MemoryPool::AllocateMemoryPool(unsigned int& inBlockSz, 
	                      	  const unsigned int& inNumBlocks,
							  const bool          inCreateNow, int inWhichHeap)
{
		// round off block size to the nearest address divisible by the size of pointer. 
		// if its not already.
		//
		int val;
    	inBlockSz = ((val = (inBlockSz % sizeof(void*) )) ? inBlockSz + (sizeof(void*) - val) : inBlockSz);
		m_PoolStart =  _heap_calloc(inWhichHeap, inNumBlocks, sizeof(char) * inBlockSz);

		if (0 == m_PoolStart) // malloc failed
			return false;

		m_BlkSize 		   = inBlockSz;
		m_NumFreeBlks      = inNumBlocks;
		m_BlkCreatCount    = inNumBlocks;
		m_TotalNumBlks     = inNumBlocks;
		m_WhichHeap		   = inWhichHeap;

		//
		// Sets up the free list. In the free list every block the first word contains the
		// next available free block. If a block is freed then it will sit in the top of the
		// free list.
		//
		if(inCreateNow) 
		{
			while(ConstructBlock()) // TODO:optimize with only one call with a flag passed to it.
				continue;
		}
		return(true);
}

//
// Deletes the memory pool by freeing the malloced memory.
//
VDK::MemoryPool::~MemoryPool()
{
        if(m_PoolStart)
                _heap_free(m_WhichHeap, m_PoolStart);
}

