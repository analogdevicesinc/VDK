/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public CreateSemaphore function
 *
 *   Last modified $Modtime:   May 22 2001 16:31:24  $
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "VDK_API.h"
#include "VDK_History.h"
#include "VDK_Thread.h"
#include "Semaphore.h"
#include "SemaphoreTable.h"
#include "Heap.h"
#include <stdlib.h>
#include <new>
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")
#pragma file_attr("SemaphoreCreation")
#pragma file_attr("SignalCreation")
#pragma file_attr("FuncName=CreateSemaphore")


namespace VDK
{
	VDK::SemaphoreID
	CreateSemaphore(  unsigned int inInitialValue, unsigned int inMaxCount, VDK::Ticks inInitialDelay, VDK::Ticks inPeriod)
	{
	    SemaphoreID semaphore_ID = static_cast <SemaphoreID> (UINT_MAX);
	    Semaphore  *pSemaphore = NULL;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	if (IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

	    if (inInitialValue > inMaxCount)
		{
			DispatchThreadError(kMaxCountExceeded, semaphore_ID);

			return static_cast <SemaphoreID> (UINT_MAX);
		}

		// A periodic semaphore should not have delay 0
		if (0 != inPeriod && 0 == inInitialDelay)
		{
	        DispatchThreadError(kSemaphoreCreationFailure, semaphore_ID);

			return static_cast <SemaphoreID> (UINT_MAX);
		}
#endif
		// Don't want to get context-switched out after we have
		// found a valid SemaphoreID.
		TMK_SuspendScheduling();
		
		semaphore_ID = g_SemaphoreTable.GetNextObjectID();
			
		// If the returned ID was UINT_MAX then the table was full
		if (semaphore_ID != UINT_MAX)
	    {
			// Create the semaphore
		    pSemaphore = (Semaphore*) _heap_malloc(g_Heaps[g_SemaphoreHeap].m_Index, sizeof(Semaphore));

			// Check that we've actually allocated the space
			if (NULL != pSemaphore)
			{
				// Use "placement new" to initialise the raw memory
				pSemaphore = new (pSemaphore) Semaphore(semaphore_ID, 
														inInitialValue, 
														inMaxCount,
														inInitialDelay,
														inPeriod);
				// Add it to the time queue if it is periodic
				if (0 != inPeriod && 0 != inInitialDelay)
				{
					TMK_AcquireMasterKernelLock();
					TMK_SetTimeout(pSemaphore, inInitialDelay);
					TMK_ReleaseMasterKernelLock();
				}

				// Add it to the semaphore table
				g_SemaphoreTable.AddObject(pSemaphore);
				HISTORY_(VDK::kSemaphoreCreated, semaphore_ID, GetThreadID());
			}
		    else 
				semaphore_ID = static_cast <SemaphoreID> (UINT_MAX);
		}

		// Re-enable scheduling
		TMK_ResumeScheduling();

	    // If we managed to create the semaphore correctly, return the 
	    // SemaphoreID, otherwise return the error

#if (VDK_INSTRUMENTATION_LEVEL_>0)
	    if (pSemaphore == 0)
	        DispatchThreadError(kSemaphoreCreationFailure, semaphore_ID);
#endif
	    return semaphore_ID; // if there is an error this will be UINT_MAX

	}
} // namespace VDK
