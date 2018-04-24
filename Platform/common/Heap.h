/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A single semaphore with a list of pending threads & timeouts
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Semaphores provide a mechanism that prevents multiple clients 
 *  from accessing the same system resource inappropriately..
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef HEAP_H_
#define HEAP_H_

#ifdef __ADSPBLACKFIN__
#include <stdlib.h>
#endif


namespace VDK{

#ifdef  __ADSP21000__
typedef char* UserID;
#else
typedef unsigned int UserID;
#endif

    struct HeapInfo {
	public:
	   UserID m_ID;
	   unsigned int m_Index;
    } ;

extern HeapInfo g_Heaps[];
    
} // namespace VDK

#endif //HEAP_H_





