/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Initialisation of boot semaphores
 *
 *   Last modified $Date$
 *       Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: The public interface to the RTOS, including the public functions,
 *  user accessible global variables, typedefs and enumerations.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include "Semaphore.h"
#include "SemaphoreTable.h"
#include "Heap.h"
#include <stdlib.h>
#include "new.h"
#include "VDK_History.h"
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")
#pragma file_attr("Startup")

namespace VDK{
  extern int                 kNumBootSemaphores;
  extern SemaphoreInfo   g_BootSemaphores[];
  extern HeapID                 g_SemaphoreHeap;

Semaphore* SemaphoreInfo::CreateSemaphore()
{
    Semaphore *pSemaphore = (Semaphore*) _heap_malloc(g_Heaps[g_SemaphoreHeap].m_Index, sizeof (Semaphore));
    pSemaphore = new (pSemaphore)
        Semaphore(m_ID, m_InitialValue, m_MaxCount, m_InitialDelay, m_Period );

    // Check that we've actually obtained the semaphore created.
    // if so, add the semaphore to the appropriate tables
    if (pSemaphore != 0)
    {
		TMK_AcquireMasterKernelLock();

		g_SemaphoreTable.AddObject(pSemaphore);

		if (0 != m_Period && 0 != m_InitialDelay)
			TMK_SetTimeout(pSemaphore, m_InitialDelay);

		TMK_ReleaseMasterKernelLock();
		
		HISTORY_(kSemaphoreCreated, m_ID, GetThreadID());
    }

   return pSemaphore;
}

bool InitBootSemaphores()
{
    for (int i = 0; i < kNumBootSemaphores; i++)
    {
	Semaphore *pSemaphore = g_BootSemaphores[i].CreateSemaphore();
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (NULL == pSemaphore) KernelPanic(kBootError,kSemaphoreCreationFailure,i);
#endif
    }
    
    return false;
}
}

