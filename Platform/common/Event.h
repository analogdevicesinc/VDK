/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Events alert threads on compound system state conditions 
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Events provide a means of alerting threads when a specified set
 *  of system state meets selected conditions.  Threads pend on events in a
 *  manner parallel to semaphores.  However, the "post" operation only occurs
 *  as a result of a change in system state, and is never performed manually.
 *  Also, while the event's conditions are met, any and all threads that are 
 *  pending on the event (or were previously pending) are allowed to run.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/




#ifndef __EVENT_H__
#define __EVENT_H__

#include "TMK.h"

#include "VDK_API.h"
#include "IDElement.h"
#include "VDK_Thread.h"

namespace VDK
{
    class Event : public IDElementTemplate<EventID>
    {
	friend bool PendEvent( const EventID inEventID, const Ticks inTimeout );

	friend 	SystemError GetEventPendingThreads( const EventID	inEventID, 
												int				*outNumThreads, 
												ThreadID		outThreadArray[],
												int				inArraySize );

        Event(Event&);
    public:
    
        /**
         * Constructor
         *
         * Uses data from vdk.cpp
         */
		Event(EventID inID, bool inAllBitsMatch, 
					unsigned int inMask, unsigned int inVector);

// BEGIN_INTERNALS_STRIP
        /**
         * Recalculates the Event Value
         */
        void            Recalculate();

        /**
         * Get the current state of the event.
         */
        bool            Value() const
                            { return m_Value; }

        /**
         * Get a thread from the event's queue
         * returns 0 when there are no more threads
         */
        VDK::Thread*     GetThread()
                            { return static_cast<Thread*>(TMK_GetNextThreadFromList(&m_PendingThreads)); }

        /**
         * EventData interfaces.
         */
        const VDK::EventData&   EventData() const
                                    { return m_EventData; }
        void                    SetEventData( VDK::EventData inEventData );

// END_INTERNALS_STRIP
    
    private:
        bool          		m_Value;
        TMK_ThreadList		m_PendingThreads;
        VDK::EventData		m_EventData;
    };

    extern volatile unsigned int     g_EventHasReadyThreads; // declared in Event.cpp
    extern VDK::Event       g_Events[];     // declared in vdk.cpp
    extern unsigned int	    kNumEvents;     // declared in vdk.cpp

} // namespace VDK

#endif // __EVENT_H__





