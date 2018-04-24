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

#ifndef _VDK_MEM_POOL_H_
#define _VDK_MEM_POOL_H_

#include "VDK_API.h"
#include "IDTable.h"

namespace VDK
{

//
// Memory pool class allocats a chunk of memory, devides into blocks and efficiently manages them
// It uses heap approach, i.e., top will have the next available free block. If its 0 then there 
// are no free blocks in this pool.
//
class MemoryPool : public IDElementTemplate<PoolID>
{

friend SystemError GetPoolDetails( const PoolID inPoolID,
								   unsigned int *outNumBlocks, 
								   void			**outMemoryAddress );

friend int GetAllMessages( MessageID	outMessageIDArray[],
						   int			inArraySize );

public:
// An abstract structre for the block. The first word of the block will have the next
// free block information.
//
struct block
{
	void* next;
};

	 MemoryPool(PoolID id) : m_BlkTop(0),m_NumFreeBlks(0),m_PoolStart(0),
	 							m_BlkCreatCount(0),m_TotalNumBlks(0), m_WhichHeap(0)
	 {
		ID() = id; 
	 }
	~MemoryPool();


// BEGIN_INTERNALS_STRIP

	 // Allocates the memory, makes the pool ready for conumption,
	 // as number of blocks and the size of block is specified.
	 //
	 bool      AllocateMemoryPool(unsigned int &inBlockSz, const unsigned int& inNumBlocks,const bool inLazyCreate,int inWhichHeap);
	 
	 
	 // Get the top free block avaibale,if there are available free blocks they 
	 // will sit in the top of the list/heap. Returns the block id and fills the
	 // outBlkPtr supplied by the user. Returns -1 if it fails to find a free one.
	 //
	 void*   GetFreeBlock();


	 // Puts the block in the free list. It will not clean up the data init. 
	 // The last freed block is the first available. 
	 //
	 bool 	 FreeBlock(void* inBlkPtr);

	 inline bool IsPoolEmpty()
		 { return((NumAllocBlocks()) == 0); }

	inline bool IsThisPoolPtr(void *inBlkPtr)
		 {
			 return((inBlkPtr) >= m_PoolStart) && (inBlkPtr < (static_cast<char*>(m_PoolStart) + (m_TotalNumBlks * m_BlkSize)));
		 }

	 // Returns the number of free blocks in the pool.
	 // 
	 inline unsigned int& NumFreeBlocks(void)
		 { return(m_NumFreeBlks); }
	 
	 inline const unsigned int& NumFreeBlocks(void) const
	 { return(m_NumFreeBlks); }

	 // Returns the number of allocated blocks in the pool.
	 // 
	 inline unsigned int NumAllocBlocks(void)
		 { return(m_TotalNumBlks - m_NumFreeBlks); }

	 unsigned int BlkSize()
	     { return m_BlkSize; }

	inline int& WhichHeap() {return(m_WhichHeap);}
private:
	 // For given block id, this function returns the start address of it.
	 //
	 inline void* BlockPtr(const unsigned int inSlot)
	 {
	 	return static_cast<void*> (static_cast<char *>(m_PoolStart) + ((inSlot - 1) * m_BlkSize));
	 }

	 // Constructs the free list
	 //
	 bool ConstructBlock(void);
	 //
	 // Memory pool data
	 // 
// END_INTERNALS_STRIP
private:
	  void*               m_BlkTop;        // index of top free block
	  void*               m_PoolStart;     // start of malloc/new alloced memory area
	  									   //
	  unsigned int        m_BlkSize;       // block size of this memory pool
	  unsigned int        m_NumFreeBlks;   // total number of free blocks in this pool
	  unsigned int        m_BlkCreatCount; // keeps track of already created blocks
	  unsigned int        m_TotalNumBlks;  // total number of blocks
          int 		      m_WhichHeap;
};

      typedef struct BootPoolInfo
      {
              unsigned int inPoolID;
              unsigned int inBlockSize;
              unsigned int inNumBlocks;
              unsigned int inLazyCreate;
              int inWhichHeap;
      }BootPoolInfo;



} /* namespace VDK */
#endif /* _VDK_MEM_POOL_H_ */

