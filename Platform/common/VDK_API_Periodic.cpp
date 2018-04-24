/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public Periodic Semaphore API functions
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


#include"TMK.h"

#include "VDK_API.h"
#include "VDK_History.h"
#include "VDK_Macros.h"
#include "VDK_Thread.h"
#include "SemaphoreTable.h"
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


namespace VDK
{
	void
	MakePeriodic( const SemaphoreID inSemID, const Ticks inDelay, const Ticks inPeriod )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kUnknownSemaphore;
		int errorValue = inSemID;

		TMK_AcquireMasterKernelLock();
	    Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemID);

		// Check to see that the event we're making periodic is defined
		if ERRCHK(NULL != pSemaphore)
	    {
		    // If the semaphore has a non-zero period, it is already in the
	    	// periodic queue, so throw the error.  Otherwise...
		    if ERRCHK(pSemaphore->m_Period == 0)
	    	{
		        // If the delay value is invalid, throw an error
	        	if ERRCHK(inDelay > 0 && inDelay < INT_MAX)
	    	    {
	            	// Check that the period is valid
		            if (inPeriod > 0 && inPeriod < INT_MAX)
	            	{
	            	    // Setup the period
	                	pSemaphore->m_Period = inPeriod;

	                    // Insert the semaphore in the queue
	                    TMK_SetTimeout(pSemaphore, inDelay);  // safe wrt. RescheduleISR
						errorCode = kNoError;
	                }
	                else
					{
	                    // An invalid period was entered
						errorCode  = kInvalidPeriod;
						errorValue = inPeriod;
					}
	            }
	            else
				{
	                // An invalid delay was entered
					errorCode  = kInvalidDelay;
					errorValue = inDelay;
				}
	        }
	        else
			{
	            // The semaphore was already periodic
		    	errorCode = kAlreadyPeriodic;
			}
	    }
	    
		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError( errorCode, errorValue );
#endif
	}

	void
	RemovePeriodic( const SemaphoreID inSemID )
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kUnknownSemaphore;

		TMK_AcquireMasterKernelLock();
		Semaphore *pSemaphore = g_SemaphoreTable.GetObjectPtr(inSemID);

		// Check to see that the semaphore we're making non-periodic is defined
		if ERRCHK(NULL != pSemaphore)
        {
	    	// Check to see that the semaphore we're making NONperiodic is periodic
		    if ERRCHK(pSemaphore->m_Period != 0)
	        {
				TMK_CancelTimeout(pSemaphore);	// safe wrt. RescheduleISR
                pSemaphore->m_Period = 0;
				errorCode = kNoError;
	        }
	        else
	            // The semaphore was already periodic
		    	errorCode = kNonperiodicSemaphore;
        }

		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError( errorCode, inSemID );
#endif
	}

} // namespace VDK



