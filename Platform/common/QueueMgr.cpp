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
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#include <VDK_API.h>
#include "QueueMgr.h"
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")



////////////////////////////////////////
// Removes the specified element from the queue
//
// FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED
////////////////////////////////////////
void
VDK::QueueMgr::Remove(PriorityListMgr inOutReadyElements[], PriorityListElement* pInElement)
{
    VDK::Priority priority = pInElement->Priority();

    // Remove the element from the priority level
    pInElement->Remove();

    // Do we need to dink with the bool value that tells whether there is a
    // element at that priority. 
    if (inOutReadyElements[priority].ElementsPresent() == false)
        m_IsElementPresentAtPriority &= ~(1 << priority);
}



////////////////////////////////////////
// Inserts the specified element into the queue
//
// FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED
////////////////////////////////////////
void
VDK::QueueMgr::Insert(PriorityListMgr inOutReadyElements[], PriorityListElement* pInElement)
{
    // Remove the element from the priority level
    VDK::Priority priority = pInElement->Priority();
    inOutReadyElements[priority].Insert(pInElement);

    // Dink the bit that tells whether or not there are elements at this
    // priority
    m_IsElementPresentAtPriority |= (1 << priority);
}


////////////////////////////////////////
// Removes the specified element from the queue
//
// FOR READYQUEUE, SHOULD ALWAYS BE CALLED WITH THE RESCHEDULE INTERRUPT MASKED
////////////////////////////////////////
bool
VDK::QueueMgr::MoveToEnd(PriorityListMgr inOutReadyElements[], PriorityListElement* pInElement)
{
    VDK::Priority priority = pInElement->Priority();

    if (inOutReadyElements[priority].MultipleElementsPresent())
    {
		// Remove the element from the priority level
		pInElement->Remove();
		inOutReadyElements[priority].Insert(pInElement);

		return true;
    }

    return false;
}

