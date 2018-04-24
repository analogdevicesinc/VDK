/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *																   
 *   Description: The implementation of the public Event & EventBits API functions
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
#include "BitOps.h"
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "Event.h"
#include "EventBit.h"
#pragma file_attr("OS_Component=Events")
#pragma file_attr("Events")

namespace VDK
{
    volatile unsigned    g_EventBitNeedsRecalculate;
    volatile unsigned    g_EventNeedsRecalculate;

	extern void (*g_PFEventRecalcHandler)(); // defined in main.cpp
    extern int        	g_SchedulerRegionCount;
    extern volatile unsigned        g_EventHasReadyThreads;
	extern int           g_remainingTicks;

    static TMK_Lock s_eventsLock = TMK_INIT_GLOBAL_INTERRUPT_LOCK(TMK_MAX_LOCK_PRECEDENCE);

    class EventChangeDPC: public TMK_DpcElement // DpcQueueElement must be first in inheritance list, as
	{                                            // we assume that its offset from the object is zero
	public:
		EventChangeDPC()
		{
			TMK_InitDpcElement(this, Invoke);         // set up DPC invoke function pointer
		}
	
	protected:
		static void Invoke(TMK_DpcElement*);
	};

	EventChangeDPC g_EventsObject;

	//
	// This function, recalculateEvents(), does the bulk of the work associated
	// with a change of Event/EventBit state. It is called in one of two ways:
	// - from ReSchedule(), via the DPC queue
	// - from Schedule(), via a function pointer
	// In the first case the function executes at interrupt level, in the second
	// at runtime level. Its activities must therefore be acceptable in either
	// context.
	//
	// The reason for both of these forms of indirection is to eliminate the need
	// for linkage dependency between Schedule()/ReSchedule() and the Events functions
	// which would otherwise force the Events code to be linked into every application,
	// whether needed or not.
	//
	static void recalculateEvents()
	{
		TMK_AcquireMasterKernelLock();

		// Recalculate any Event bit necessary.
		if (0 == g_EventBitNeedsRecalculate)
		{
			TMK_ReleaseMasterKernelLock();
			return;
		}

		while (0 != g_EventBitNeedsRecalculate)
		{
			TMK_LockState state = TMK_AcquireGlobalInterruptLock(&s_eventsLock);

			int bit_num = FindFirstSet(g_EventBitNeedsRecalculate);
			g_EventBitNeedsRecalculate &= ~(1 << bit_num);

			TMK_ReleaseGlobalInterruptLock(&s_eventsLock, state);
			
			g_EventBits[bit_num].Recalculate();
		}

		// Recalculate any Events if necessary
		if (g_EventNeedsRecalculate != 0)
    	{
			int bit_num;
			while((bit_num = FindFirstSet(g_EventNeedsRecalculate)) != VDK::kNoneSet)
	        {
				g_Events[bit_num].Recalculate();
				g_EventNeedsRecalculate &= ~(1 << bit_num);
			}
		}
		// Move any necessary threads around
		if (g_EventHasReadyThreads != 0)
    	{
			int bit_num;
			while((bit_num = FindFirstSet(g_EventHasReadyThreads)) != VDK::kNoneSet)
    	    {
				VDK::Thread  *pThread;
				while ((pThread = g_Events[bit_num].GetThread()) != 0)
    	        {
					TMK_RemoveThreadFromList(pThread);
					TMK_CancelTimeout(pThread);
					HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
					TMK_MakeThreadReady(pThread);
				}
				g_EventHasReadyThreads &= ~(1 << bit_num);
			}
		}

		TMK_ReleaseMasterKernelLock();
	}

	static void recalcHandler()
	{
		g_PFEventRecalcHandler = TMK_ResumeScheduling;

		recalculateEvents();
		
		TMK_ResumeScheduling();
	}

	//
	// This is the static Invoke() function for class EventChangeDPC. It is
	// called from ReSchedule(), i.e. at interrupt level, as a result of the
	// EventChangeDPC object, g_EventsObject, having been placed on the
	// DPC queue.
	//
	void EventChangeDPC::Invoke(TMK_DpcElement *)
	{
        if (0 == g_SchedulerRegionCount)   // i.e. was zero at time of ISR
        {
			recalculateEvents();
		}
		else
		{
			g_PFEventRecalcHandler = recalcHandler;
		}
	}

	////////////////////////////////////////
	// EVENTBITS
	////////////////////////////////////////
	bool
	GetEventBitValue( const EventBitID inEventBitID )
	{
		if NERRCHK(IS_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

		// Check to see that the event we're getting is a defined one
		if ERRCHK(inEventBitID < kNumEventBits)
			return g_EventBits[inEventBitID].Value();
		else
			return static_cast<bool> (DispatchThreadError( kUnknownEventBit, inEventBitID ));
	}


	void
	SetEventBit( const EventBitID inEventBitID )
	{
		if NERRCHK(IS_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		// Check to see that the event we're getting is a defined one
		if ERRCHK(inEventBitID < kNumEventBits)
	    {
			TMK_SuspendScheduling();

	        // Log the event
	        // Set the high bit with the EventBitID's current value
	        HISTORY_(VDK::kEventBitSet, inEventBitID | (g_EventBits[inEventBitID].Value()? INT_MIN:0), GetThreadID());

			TMK_LockState state = TMK_AcquireGlobalInterruptLock(&s_eventsLock);			

			// Set this value
			if (!IsBitSet(g_EventBitState, inEventBitID))
			{
				g_EventBitState |= 1 << inEventBitID;
				g_EventBitNeedsRecalculate |= (1 << inEventBitID);

				// We want to FORCE a reschedule
				g_PFEventRecalcHandler = recalcHandler;
			}

			TMK_ReleaseGlobalInterruptLock(&s_eventsLock, state);

			if (0 == g_SchedulerRegionCount)
				(*g_PFEventRecalcHandler)();
			else
				TMK_ResumeScheduling();
	    }
		else
		    DispatchThreadError( kUnknownEventBit, inEventBitID );
	}


	void
	ClearEventBit( const EventBitID inEventBitID )
	{
		if NERRCHK(IS_ISR_LEVEL()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		// Check to see that the event we're getting is a defined one
		if ERRCHK(inEventBitID < kNumEventBits)
	    {
			TMK_SuspendScheduling();

	        // Log the event
	        // Set the high bit with the EventBitID's current value
	        HISTORY_(VDK::kEventBitCleared, inEventBitID | (g_EventBits[inEventBitID].Value()? INT_MIN:0), GetThreadID());

			TMK_LockState state = TMK_AcquireGlobalInterruptLock(&s_eventsLock);			

			// Clear this value
			if (IsBitSet(g_EventBitState, inEventBitID))
			{
				g_EventBitState &= ~(1 << inEventBitID);
				g_EventBitNeedsRecalculate |= (1 << inEventBitID);

				// We want to FORCE a reschedule
				g_PFEventRecalcHandler = recalcHandler;
			}

			TMK_ReleaseGlobalInterruptLock(&s_eventsLock, state);

			if (0 == g_SchedulerRegionCount)
				(*g_PFEventRecalcHandler)();
			else
				TMK_ResumeScheduling();
	    }
		else
		    DispatchThreadError( kUnknownEventBit, inEventBitID );
	}


	bool
	GetEventValue( const EventID inEventID )
	{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
	    if ERRCHK(inEventID < kNumEvents)
	        return g_Events[inEventID].Value();
		else
			return static_cast<bool> (DispatchThreadError( kUnknownEvent, inEventID ));
	}


	EventData
	GetEventData( const EventID inEventID )
	{
	    EventData   ret_val={false,0,0};
	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
	    if ERRCHK(inEventID < kNumEvents)
	        ret_val = g_Events[inEventID].EventData();
	    else
			DispatchThreadError(kUnknownEvent, inEventID);

	    return ret_val;
	}

	void
	LoadEvent( const EventID inEventID, const EventData inEventData )
	{
	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
	    if ERRCHK(inEventID < kNumEvents)
	    {
			TMK_SuspendScheduling();

			TMK_AcquireMasterKernelLock();
	        g_Events[inEventID].SetEventData(inEventData);
			TMK_ReleaseMasterKernelLock();

            // We want to FORCE a reschedule
			g_PFEventRecalcHandler = recalcHandler;
			TMK_ResumeScheduling();             // OOPS! This isn't going to work!!
	    }
	    else
	        DispatchThreadError(kUnknownEvent, inEventID);
	}


	
	SystemError
	GetEventPendingThreads( const EventID inEventID, 
							int			  *outNumThreads, 
							ThreadID	  outThreadArray[],
							int			  inArraySize )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
			
		SystemError errorCode = kUnknownEvent;

		// If we need to fill an array, check we're not going to dereference a 
		// NULL pointer.
		// This reduces to 'true' for non-error checking libs
		if ERRCHK( (outNumThreads != NULL) && ((outThreadArray != NULL) || (inArraySize == 0)) )
		{
			// Check to see that the event we're looking at exists.
			if (inEventID < kNumEvents)
			{
				// We are going to be reusing the passed-in ThreadID array to get
				// the the thread pointers from the Micro Kernel. 
				//
				// These are completely unrelated types, so trying to cast it in an
				// elegent way just isn't going to work. 
				TMK_Thread **transArray = reinterpret_cast<TMK_Thread **>(outThreadArray);

				TMK_AcquireMasterKernelLock();
				// The event exists, get the threadlist
				*outNumThreads = TMK_QueryThreadlist( &g_Events[inEventID].m_PendingThreads,
														transArray,
														inArraySize);
				
				//Now we have to convert from TMK_Thread pointers to thread ID's.
				int numFilled = (*outNumThreads < inArraySize? *outNumThreads : inArraySize);
				
				for(int i = 0; i < numFilled; i++)
				{
					outThreadArray[i] = static_cast<Thread *>(transArray[i])->ID();
				}

				TMK_ReleaseMasterKernelLock();

				errorCode		= kNoError; 
			}
		}
		else
		{
			// We have been passed a NULL pointer, set the errorCode and return
			errorCode = kInvalidPointer;
		}

		return errorCode;
	}

}	// namespace VDK


