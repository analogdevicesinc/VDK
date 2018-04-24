/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
 *
 *   Last modified $Modtime:   Feb 20 2001 16:51:44  $
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
#include "Semaphore.h"
#include "VDK_History.h"
#include "VDK_Thread.h"
#include "SemaphoreTable.h"
#include "Heap.h"
#include <stdlib.h>
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")
#pragma file_attr("SignalDestruction")
#pragma file_attr("SemaphoreDestruction")
#pragma file_attr("FuncName=DestroySemaphore")


namespace VDK
{
    void
    DestroySemaphore( const SemaphoreID inSemaphoreID)
    {
		TMK_AcquireMasterKernelLock(); // Freeze the table
#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

		Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr( inSemaphoreID );
		SystemError error = kUnknownSemaphore;

		// Check to see if we got the semaphore
	    if (NULL != pSemaphore)
	    {
			// Semaphore exists

			// if there are threads pending we return an error at the moment
			// This behaviour might change in the future
			if (TMK_IsThreadListEmpty(&pSemaphore->m_PendingThreads))
			{
				// if the semaphore was periodic, remove it from the time queue
				if (pSemaphore->m_Period != 0)
        				TMK_CancelTimeout(pSemaphore);
				g_SemaphoreTable.RemoveObject(inSemaphoreID);
				HISTORY_(VDK::kSemaphoreDestroyed, inSemaphoreID, GetThreadID());
			}
			else // can't destroy it
			{
				pSemaphore = NULL;
				error = kSemaphoreDestructionFailure;
			}
		}

		TMK_ReleaseMasterKernelLock();

		// Do the deletion (or error reporting) outside the locked region
		if (NULL != pSemaphore)
		{
			pSemaphore->~Semaphore(); // call destructor
			_heap_free(g_Heaps[g_SemaphoreHeap].m_Index, pSemaphore);
		}
#if (VDK_INSTRUMENTATION_LEVEL_>0)
		else 
			DispatchThreadError(error, inSemaphoreID);
#endif
	}


} // Namespace VDK















