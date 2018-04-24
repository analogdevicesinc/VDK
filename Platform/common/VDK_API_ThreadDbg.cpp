/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the Public thread debug info APIs
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
#include "DebugInfo.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"

#ifdef __ADSPBLACKFIN__
#include <complex.h>
#define MERGE_TO_64BIT_(lo_, hi_) __builtin_compose_i64(lo_, hi_)
#elif defined(__ADSPTS__)
#include <builtins.h>
#define MERGE_TO_64BIT_(lo_, hi_) __builtin_compose_64u(lo_, hi_)
#endif

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("HistoryAndStatus")

// Debug data is only logged for fully instrumented builds
#if (VDK_INSTRUMENTATION_LEVEL_==2)
namespace VDK 
{
	SystemError
	GetNumTimesRun( const ThreadID inThreadID, unsigned int *outRunCount )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		
		SystemError errorCode = kUnknownThread;

		// This function can only used for fully instrumented builds,
		// so this error checking step is ok without a ERRCHK macro
		if( outRunCount != NULL )
		{
			// Find the thread
  			TMK_AcquireMasterKernelLock();
			Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

			// If we have found the thread, then return its NumTimesRun
			// otherwise, return the default error kUnknownThread.
			if (NULL != pThread) 
			{
				*outRunCount = pThread->ThreadDebugInfo()->m_nNumTimesRun;
				errorCode = kNoError;
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

#ifdef __ADSP21000__ 
	SystemError
	GetSharcThreadCycleData( const VDK::ThreadID	inThreadID,
							 unsigned int			outCreationTime[2],
							 unsigned int			outStartTime[2],
							 unsigned int			outLastTime[2],
							 unsigned int			outTotalTime[2] )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		SystemError errorCode = kUnknownThread;

		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then return its cycle data
		// otherwise, return the default error kUnknownThread.
		if (NULL != pThread) 
		{	
			DebugInfo *pDbgInfo = pThread->ThreadDebugInfo();
			if(outCreationTime != NULL)
			{
				outCreationTime[0]	= pDbgInfo->m_llCreationCycle[0];
				outCreationTime[1]   = pDbgInfo->m_llCreationCycle[1]; 
			}
			if(outStartTime != NULL)
			{
				outStartTime[0]		= pDbgInfo->m_llRunStartCycles[0];
				outStartTime[1]		= pDbgInfo->m_llRunStartCycles[1];
			}
			if(outLastTime != NULL)
			{
				outLastTime[0]		= pDbgInfo->m_llRunLastCycles[0];
				outLastTime[1]		= pDbgInfo->m_llRunLastCycles[1];
			}
			if(outTotalTime != NULL)
			{
				outTotalTime[0]		= pDbgInfo->m_llRunTotalCycles[0];
				outTotalTime[1]		= pDbgInfo->m_llRunTotalCycles[1];
			}

			errorCode = kNoError;
		}
  		TMK_ReleaseMasterKernelLock();

		return errorCode;
	}

#else // !__ADSP21000__
	SystemError
	GetThreadCycleData( const VDK::ThreadID		inThreadID,
						unsigned long long int	*outCreationTime,
						unsigned long long int	*outStartTime,
						unsigned long long int	*outLastTime,
						unsigned long long int	*outTotalTime )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	


		SystemError errorCode = kUnknownThread;

		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then return its cycle data
		// otherwise, return the default error kUnknownThread.
		if (NULL != pThread) 
		{

			DebugInfo *pDbgInfo = pThread->ThreadDebugInfo();

			if(outCreationTime != NULL)
			{
				*outCreationTime = MERGE_TO_64BIT_(
										pDbgInfo->m_llCreationCycle[1], 
										pDbgInfo->m_llCreationCycle[0]);
			}
			if(outStartTime != NULL)
			{
				*outStartTime	= MERGE_TO_64BIT_(
										pDbgInfo->m_llRunStartCycles[1], 
										pDbgInfo->m_llRunStartCycles[0]);
			}
			if(outLastTime != NULL)
			{
				*outLastTime	= MERGE_TO_64BIT_(
										pDbgInfo->m_llRunLastCycles[1], 
										pDbgInfo->m_llRunLastCycles[0]);
			}
			if(outTotalTime != NULL)
			{
				*outTotalTime	= MERGE_TO_64BIT_(
										pDbgInfo->m_llRunTotalCycles[1], 
										pDbgInfo->m_llRunTotalCycles[0]);
			}
			errorCode = kNoError;
		}
  		TMK_ReleaseMasterKernelLock();

		return errorCode;

	}
#endif //__ADSP21000__


	SystemError
	GetThreadTickData( const VDK::ThreadID	inThreadID,
					   Ticks	*outCreationTime,
					   Ticks	*outStartTime,
					   Ticks	*outLastTime,
					   Ticks	*outTotalTime )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		SystemError errorCode = kUnknownThread;

		// Find the thread
  		TMK_AcquireMasterKernelLock();
		Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );

		// If we have found the thread, then return its Ticks data
		// otherwise, return the default error kUnknownThread.
		if (NULL != pThread) 
		{
			DebugInfo *pDbgInfo	= pThread->ThreadDebugInfo();
			
			if(outCreationTime != NULL)
			{
				*outCreationTime	= pDbgInfo->m_tCreationTime;
			}
			if(outStartTime != NULL)
			{
				*outStartTime		= pDbgInfo->m_tRunStartTime;
			}
			if(outLastTime != NULL)
			{
				*outLastTime		= pDbgInfo->m_tRunLastTime;
			}
			if(outTotalTime != NULL)
			{
				*outTotalTime		= pDbgInfo->m_tRunTotalTime;
			}
			errorCode = kNoError;
		}
  		TMK_ReleaseMasterKernelLock();

		return errorCode;
	}

} //namespace VDK 

#endif /* VDK_INSTRUMENTATION_LEVEL_ */
