/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The ID table for Semaphores
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: A table for registering Semaphores so that they can be created
 *	dynamically with unique IDs.  Cross ref from ID to ptr.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef __SEMAPHORETABLE__
#define __SEMAPHORETABLE__

#include "VDK_API.h"
#include "IDTable.h"
#include "Semaphore.h"


namespace VDK
{

    typedef IDTable<Semaphore,SemaphoreID> SemaphoreTable;

    extern VDK::SemaphoreTable g_SemaphoreTable;

} // namespace VDK


#endif  // __SEMAPHORETABLE__

