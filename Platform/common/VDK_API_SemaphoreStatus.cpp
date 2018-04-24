/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: APIs that relate to the status information of semaphores
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

#include "TMK.h"

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "VDK_History.h"
#include "SemaphoreTable.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


namespace VDK
{
	SystemError
	GetSemaphoreDetails( const SemaphoreID  inSemID, 
						 Ticks				*outPeriod, 
						 unsigned int		*outMaxCount )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		
		SystemError errorCode = kUnknownSemaphore;

		// Check to see that the semaphore we're looking at exists.
		TMK_AcquireMasterKernelLock();

	    Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemID);
		if (NULL != pSemaphore)
		{
	    	// The semaphore exists, return the required data
			if(outPeriod != NULL)
			{
				*outPeriod		= pSemaphore->m_Period;
			}
			if(outMaxCount != NULL)
			{
				*outMaxCount	= pSemaphore->m_MaxCount;
			}
			errorCode		= kNoError; 
		}
		TMK_ReleaseMasterKernelLock();

	    return errorCode;
	}


	SystemError
	GetSemaphorePendingThreads( const SemaphoreID   inSemID, 
								int					*outNumThreads, 
								ThreadID			outThreadArray[],
								int					inArraySize )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		
		SystemError errorCode = kUnknownSemaphore;
		
		// If we need to fill an array, check we're not going to dereference a 
		// NULL pointer.
		// This reduces to 'true' for non-error checking libs
		if ERRCHK( (outNumThreads != NULL) && ((outThreadArray != NULL) || (inArraySize == 0)) )
		{
			// We are going to be reusing the passed-in ThreadID array to get
			// the the thread pointers from the Micro Kernel. 
			//
			// These are completely unrelated types, so trying to cast it in an
			// elegent way just isn't going to work. 
			TMK_Thread **transArray = reinterpret_cast<TMK_Thread **>(outThreadArray);

			// Check to see that the semaphore we're looking at exists.
			TMK_AcquireMasterKernelLock();

			Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemID);
			if (NULL != pSemaphore)
			{
	    		// The semaphore exists, get the threadlist
				*outNumThreads = TMK_QueryThreadlist( &pSemaphore->m_PendingThreads,
														transArray,
														inArraySize );
				
				//Now we have to convert from TMK_Thread pointers to thread ID's.
				int numFilled = (*outNumThreads < inArraySize? *outNumThreads : inArraySize);
				
				for(int i = 0; i < numFilled; i++)
				{
					outThreadArray[i] = static_cast<Thread *>(transArray[i])->ID();
				}

				errorCode		= kNoError; 
			}
			TMK_ReleaseMasterKernelLock();
		}
		else
		{
			// We have been passed a NULL pointer, set the errorCode and return
			errorCode = kInvalidPointer;
		}

	    return errorCode;
	}

	int
	GetAllSemaphores(SemaphoreID outSemaphoreIDArray[], int inArraySize)
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		// Check that if we are to fill an array that the pointer is not null
		if NERRCHK( (inArraySize != 0) && (outSemaphoreIDArray == NULL) )
			DispatchThreadError( kInvalidPointer, 0 );

		// Disable scheduling while we access a global table
		TMK_AcquireMasterKernelLock();
		int count = g_SemaphoreTable.GetAllObjectIDs(outSemaphoreIDArray, inArraySize);
		TMK_ReleaseMasterKernelLock();

		return count;
	}

} //namespace VDK
