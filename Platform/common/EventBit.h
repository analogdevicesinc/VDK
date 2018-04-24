/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: EventBit is a bit of global state with its dependent events
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: EventBit is an encapsulation of a single boolean condition and a  
 *  list of event flags that are dependent on that condition.  All of the event  
 *  flags associated with an event will be recalculated when the state of the
 *  event toggles.   Ultimately, it is likely that all of the events in the
 *  system may be implemented as a single bit mask with all of the event
 *  values, and an array of linked lists of event flags.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef EVENTBIT_H_
#define EVENTBIT_H_

#include "BitOps.h"

namespace VDK
{

    extern volatile unsigned int     g_EventBitState;   	        // defined in vdk.cpp
    extern volatile unsigned int     g_EventBitNeedsRecalculate; // declared in VDK_API_Events.cpp
    extern volatile unsigned int     g_EventNeedsRecalculate;    // declared in VDK_API_Events.cpp
    extern unsigned int     kNumEventBits;  			// defined in vdk.cpp

    class EventBit
    {
    public:

// BEGIN_INTERNALS_STRIP
        EventBitID      ID() const; // defined inline below
            
        /**
         * Gets the value
         */
        bool      		Value() const
                            { 
    							return  IsBitSet(g_EventBitState, ID());
    						}

        /**
         * Sets up the Bitfield that tells which Events need to be recalculated
         */
        void            Recalculate()
			{
				// Call recalculate for all Events that depend on this EventBit
				g_EventNeedsRecalculate |= m_DependentEvents;
				
			}
    
    
        /**
         * Tells an event that an specified EventBit is NO LONGER
         * dependent on its value.
         */
        void            MakeIndependent( EventID inEventID )
                            { m_DependentEvents &= ~(1 << inEventID); }
    
        /**
         * Tells an event that an specified EventBit IS
         * dependent on its value.
         */
        void            MakeDependent( EventID inEventID )
                            { m_DependentEvents |= (1 << inEventID); }
    
// END_INTERNALS_STRIP
    private:
    
        unsigned int   m_DependentEvents;
    };


	//
	// Moving these (EventBits and EventBit::ID()) down here avoids the
	// chicken-and-egg situation that would occur if we defined ID()
	// within the class, above.
	//
    extern VDK::EventBit    g_EventBits[];  			// defined in vdk.cpp

// BEGIN_INTERNALS_STRIP
	inline EventBitID      EventBit::ID() const
	{
		return static_cast<EventBitID>(this - g_EventBits);
	}
// END_INTERNALS_STRIP

} // namespace VDK

#endif /* EVENTBIT_H_ */





