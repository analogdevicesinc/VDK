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

#include "TMK.h"

#include "Event.h"
#include "EventBit.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Events")
#pragma file_attr("Events")

namespace VDK
{
	volatile unsigned    g_EventHasReadyThreads;

	/** ----------------------------------------------------------------------------
	 * FUNC   : Event::Event()
	 *
	 * DESC   : Constructor for the event class.
	 *
	 *          Initializes the array of Events based on the global variable
	 *			g_Events[] defined and initialized in vdk.cpp,
	 *			which is generated by the IDDE.
	 *
	 * PARAMS : inID = the enumeration value
	 *			inEventData = the EventData struct
	 * 
	 * RETURNS: N/A
	 */ // -------------------------------------------------------------------------
	Event::Event(EventID inID, bool inAllBitsMatch, 
					unsigned int inMask, unsigned int inVector)
	{
		int bit_num;

		TMK_InitThreadList(&m_PendingThreads);
		m_EventData.matchAll = inAllBitsMatch;
		m_EventData.mask = inMask;
		m_EventData.values = inVector;
		ID() = inID;

	    // Make this event dependent on correct event bits
	    while((bit_num = FindFirstSet(inMask)) != VDK::kNoneSet)
		{
	        g_EventBits[bit_num].MakeDependent(static_cast<EventID>(ID()));
	        inMask &= ~(1 << bit_num);
		}

	    // We want to recalculate now...
	    Recalculate();
	}


    //
    // This routine SHOULD NEVER BE CALLED --> OR LINKED IN!
    // It is here to prevent the compiler BUG that is calling an event
    // copy constructor even though one is NOT necessary.
    //
    Event::Event(Event &inEvent)
    {
        while(1);
    }


	void
	Event::Recalculate()
	{
	    // Save off the value so we know if we should be moving some threads around
	    bool    the_old_value = m_Value;

#ifdef __COMPILER_BUG_FIXED_

	    // Should we be testing ALL events
	    if (m_EventData.matchAll == true)
	        m_Value = (((g_EventBitState & m_EventData.mask) ^ m_EventData.values) == 0)?
	                        true : false;
	    else        // We are ORing on the events...
	        m_Value = (((~(g_EventBitState ^ m_EventData.values)) & m_EventData.mask) != 0)?
	                        true : false;
#else
	    // Should we be testing ALL events
	    if (m_EventData.matchAll == true)
		{
			unsigned temp1 = g_EventBitState & m_EventData.mask;
			unsigned temp2 = temp1 ^ m_EventData.values;
			if( temp2 == 0 )
				m_Value = true;
			else
				m_Value = false;
		}
		else  // We are ORing on the events...
		{
			unsigned temp3 = ~(g_EventBitState ^ m_EventData.values);
			unsigned temp4 = temp3 & m_EventData.mask;
			if( temp4 != 0 )
				m_Value = true;
			else
				m_Value = false;
		}
#endif

	    // Should we be moving this Event's Threads????
	    // Set the global flag
	    if ((m_Value == true) && (the_old_value == false) && !TMK_IsThreadListEmpty(&m_PendingThreads))
	        g_EventHasReadyThreads |= (1 << ID());
	}


	void
	Event::SetEventData( VDK::EventData inEventData )
	{
	    int bit_num;

	    // Remove this event from all the event relevant bits
	    while((bit_num = FindFirstSet(m_EventData.mask)) != VDK::kNoneSet)
	    {
	        g_EventBits[bit_num].MakeIndependent(static_cast<EventID>(ID()));
	        m_EventData.mask &= ~(1 << bit_num);
	    }

	    // Make the event data correct.
	    m_EventData = inEventData;

	    while((bit_num = FindFirstSet(inEventData.mask)) != VDK::kNoneSet)
		{
	        g_EventBits[bit_num].MakeDependent(static_cast<EventID>(ID()));
		inEventData.mask &= ~(1 << bit_num); // reset the bit
		}

	    // Add this event from all the event relevant bits

	    // Fix own value
	    Recalculate();
	}

}	// namespace VDK

