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

#include "VDK_MemPool.h"
#include <stdlib.h>
#pragma file_attr("OS_Component=MemoryPools")
#pragma file_attr("MemoryPools")
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")

// AllocateMemoryPool and ~MemoryPool have been moved to AllocatePool.cpp

//  
// Constructs one block and places in the top of the free list. If there can not be created
// any blocks in the current pool it will return false.
//
bool VDK::MemoryPool::ConstructBlock()
{
int inBlockNum;
register struct block *blk;

		inBlockNum = m_BlkCreatCount--;
		if(inBlockNum <= 0)
		{
			m_BlkCreatCount = 0;
			return false;
		}
		blk = static_cast<struct block *> (BlockPtr(inBlockNum));
		blk->next = m_BlkTop;
		m_BlkTop = blk;
		return true;
}

//
// Gets a free block from the top of the free pool blocks.
// This call is expected to call from an unscheduled region.
//
void* VDK::MemoryPool::GetFreeBlock()
{
void* old = 0;

		if(m_BlkTop == 0 && !ConstructBlock())
				return old;

		old = m_BlkTop;
		m_BlkTop = (((static_cast<struct block *>(old)))->next);
		if(old)
			m_NumFreeBlks--;
		return(old);
}

//
// Put the block in the free block list and updates the top of the list.
// Should be called from with in unscheduled region from higher API(s).
//
bool VDK::MemoryPool::FreeBlock(void *inBlkPtr)
{
register struct block *blk;

		// TODO: re-visit the below condition for the case of 
		// Instrumented/uninstrumented/error checking
		// We may not want this checking as divison consumes more cycles.
		// 
		if((IsThisPoolPtr(inBlkPtr) &&  !((reinterpret_cast<char* >(inBlkPtr) - 
			reinterpret_cast<char*>(m_PoolStart)) % m_BlkSize)))
		{
			blk = static_cast<struct block *>(inBlkPtr);
			blk->next   = m_BlkTop;
			m_BlkTop    = inBlkPtr;
			m_NumFreeBlks++;
			return (true);
		}
			return(false); 
}

