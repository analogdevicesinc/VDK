/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions to access 
 *	 history data.
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
#include "VDK_History.h"
#include "VDK_AsmMacros.h"
#include "vdk_macros.h"
#include "KernelPanic.h"

#pragma file_attr("HistoryAndStatus")

namespace VDK 
{
	
	/* 
	This API can be called at all instrumentation levels, but reduces to NULL
	when fully instrumented libs are not being used.
	*/
	void
	LogHistoryEvent( VDK::HistoryEnum inEnum, int inValue )
	{
#if (VDK_INSTRUMENTATION_LEVEL_==2)
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
        
		VDK::ThreadID   theThreadID;
		theThreadID = GetThreadID();
		HISTORY_(inEnum, inValue, theThreadID);
#endif
	}


	/**
	* The rest of the history APIs can only be called when using fully
	* instrumentated libraries.
	*/
#if (VDK_INSTRUMENTATION_LEVEL_==2)

	extern HistoryBuffer   g_History;		// in vdk.cpp

	SystemError
	GetHistoryEvent(unsigned int inHistEventNum, HistoryEvent *outHistoryEvent)
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

		SystemError errorCode = kMaxHistoryEventExceeded;

		// Check the pointer is not NULL.
		// We do not need to use any N/ERRCHK macros, as this function
		// is only available for fully instrumented libs.
		if( outHistoryEvent != NULL )
		{
			TMK_Lock histLock;
			if(inHistEventNum < g_History.m_NumEvents)
			{
				// We need to disable everything, in case an interrupt level history log
				// decides to cause problems.
				TMK_LockState state = TMK_AcquireGlobalInterruptLock(&histLock);

				outHistoryEvent->time	= g_History.m_Events[inHistEventNum].m_Time;

				outHistoryEvent->threadID = 
					static_cast<ThreadID>(g_History.m_Events[inHistEventNum].m_ThreadID);
				outHistoryEvent->type = 
					static_cast<HistoryEnum>(g_History.m_Events[inHistEventNum].m_Type);
				outHistoryEvent->value	= g_History.m_Events[inHistEventNum].m_Value;
				
				TMK_ReleaseGlobalInterruptLock(&histLock, state);
				
				errorCode = kNoError;
			} 
		}
		else
		{
			// We have been passed a NULL pointer, set the errorCode and return
			errorCode = kInvalidPointer;
		}

		return errorCode;
	}

	unsigned int
	GetCurrentHistoryEventNum( void )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		
		// We don't need to disable interrupts for this.
		return g_History.m_CurrentEvent;
	}

	unsigned int
	GetHistoryBufferSize( void )
	{			
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

		// We don't need to disable interrupts for this.
		return g_History.m_NumEvents;				
	}

#endif /* VDK_INSTRUMENTATION_LEVEL_ */

} //namespace VDK
