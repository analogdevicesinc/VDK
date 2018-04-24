/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: 
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *  
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef __MEMORYPOOLTABLE__
#define __MEMORYPOOLTABLE__

#include "VDK_API.h"
#include "IDTable.h"
#include "VDK_MemPool.h"


namespace VDK
{
    typedef IDTable<VDK::MemoryPool,VDK::PoolID> MemoryPoolTable;

    extern MemoryPoolTable g_MemoryPoolTable;

} // namespace VDK


#endif  // __MEMORYPOOLTABLE__
