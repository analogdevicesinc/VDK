/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: 
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The algorithm implementation of the base class for arrays of 
 *	ProrityLists, such as ReadyQueue and MessageQueue.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef __QUEUEMGR__
#define __QUEUEMGR__

#include <VDK_ListElement.h>
#include <PriorityListElement.h>
#include <PriorityListMgr.h>
#include <IDElement.h>
#include "BitOps.h"

namespace VDK
{

    class QueueMgr
    {
		
	public:

		// Default constructor
		QueueMgr()
		{
			m_IsElementPresentAtPriority = 0;
		}


		// The members declared here are independent both of the type of
		// object in the queue and of the size of the priority array, they
		// can therefore be used directly by clients of the derived Queue classes.

		unsigned		PrioritiesWithElements() const
							{ return m_IsElementPresentAtPriority; }

	protected:
		// The members declared here are only for use by derived (templatized)
		// Queue classes. This in the "untyped" interface which gets wrapped by
		// the type-specific derived class.
    
        /**
         * Returns the next element to run
         *
         * FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED!
         */
        PriorityListElement*     GetNextElement(PriorityListMgr inReadyElements[])
		{
			Priority priority = static_cast<Priority>(FindFirstSet(m_IsElementPresentAtPriority));
	
			return inReadyElements[priority].GetNextUnchecked();
		}
     
        /**
         * Removes the element from the queue
         *
         * FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED!
         */
        void            Remove(PriorityListMgr inOutReadyElements[], PriorityListElement* inElementP);
    
        /**
         * Inserts an element into the queue
         *
         * FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED!
         */
        void            Insert(PriorityListMgr inOutReadyElements[], PriorityListElement* inElementP);

        /**
         * Moves an element into the end of its priority list in the queue.
		 *
		 * This function can be static as it doesn't need to access
		 * any instance variables (the ReadElements array is passed in
		 * as an argument and m_IsElementPresentAtPriority isn't
		 * required here). Making it static (i.e a class function) is
		 * more efficient as the "this" pointer isn't passed by the
		 * caller.
         *
         * FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED!
		 */
        static bool     MoveToEnd(PriorityListMgr inOutReadyElements[], PriorityListElement* inElementP);

        /**
         * @clientCardinality 1..* 
         */
        unsigned int        	m_IsElementPresentAtPriority;
    
    }; // class QueueMgr



} // namespace VDK

#endif //  __QUEUEMGR__


