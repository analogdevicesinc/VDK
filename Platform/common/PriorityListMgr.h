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
 *  Comments: 
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef __PRIORITYLISTMGR__
#define __PRIORITYLISTMGR__

#include <VDK_ListElement.h>
#include <PriorityListElement.h>
#include <IDElement.h>

namespace VDK
{

/*
 * This class implements all of the functionality of the VDK::PriorityList class,
 * but uses (void *) instead of the actual class.  The derived class PriorityList
 * is templated, and recasts to the correct class.  By putting the bulk of the
 * code here, the templated class adds very little overhead even if it is
 * instantiated many times.
 */


// TODO: Put these in the right places:
// BEGIN_INTERNALS_STRIP
// END_INTERNALS_STRIP


class PriorityListMgr
{
    public:
        PriorityListElement*  	GetNext()
                        {
                            if (ElementsPresent())
                                return static_cast<PriorityListElement*>(m_LE.Next());
                            return 0;
                        }
    
        PriorityListElement*  	GetNextUnchecked()
                        {
                                return static_cast<PriorityListElement*>(m_LE.Next());
                        }
    
        void            Insert( PriorityListElement *inPtr )
                        {
                            // We want to insert at the END of the list....
                            // so, the previos of the head, is the tail.
                            (m_LE.Prev())->InsertAfter(inPtr);
                        }

        bool            ElementsPresent()
                        {   
                            return ( (static_cast<PriorityListElement *>(m_LE.Next())) != (&m_LE) );
                        }

        bool            MultipleElementsPresent()
                        {
                            return (m_LE.Next() != m_LE.Prev());
                        }
    protected:

        /**
         * @clientCardinality 1..* 
         */
        PriorityListElementMgr    m_LE;
};


} //namespace VDK

#endif // __PRIORITYLISTMGR__


