/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The ID table for threads
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: A table for registering threads so that they can be created
 *	dynamically with unique IDs.  Cross ref from ID to ptr.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef __THREADTABLE__
#define __THREADTABLE__

#include "VDK_API.h"
#include "IDTable.h"


namespace VDK {

    typedef IDTable < Thread, ThreadID > ThreadTable;

    extern ThreadTable g_ThreadTable;

} // namespace VDK


#endif  // __THREADTABLE__


