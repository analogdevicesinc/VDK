/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A circularly doubly linked list element
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: This class abstracts a circularly doubly linked list element.
 *
 *  Since there are LOTS of Circularly Doubly Linked Lists in VDK, we can 
 *  keep the code in one central location, and it will all work (HAAA, ha, ha...).
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef VDK_LIST_ELEMENT_H_
#define VDK_LIST_ELEMENT_H_

#include "vdk_macros.h"

namespace VDK
{

    /**
     * The ListElement takes 2 arguments in its template.  The first argument is
     * the pointer type, and the second is the casting type
     */
    template <class PointerType, class CastType = PointerType>
    class ListElement
    {
    public:
        /**
         * Forces a CDLL element to point to itself
         */
        ListElement()
        {
            m_NextP = static_cast<PointerType*>(this);
            m_PrevP = static_cast<PointerType*>(this);
        }

        /**
         * A Next accessor, for walking CDLLs
         */
        inline PointerType*     Next()
                                    { return m_NextP; }

        /**
         * A Previous accessor for walking CDLLs
         */
        inline PointerType*     Prev()
                                    { return m_PrevP; }
                            
        /**
         * Inserts the parameter PAST this element!
         */
        void    InsertAfter(PointerType *inListElement)
                {
                    static_cast<CastType*>(inListElement)->m_NextP = m_NextP;
                    static_cast<CastType*>(inListElement)->m_PrevP = static_cast<PointerType*>(this);
                    static_cast<CastType*>(m_NextP)->m_PrevP = inListElement;
                    m_NextP = inListElement;
                }

        bool IsInList()
	        { return (m_NextP != static_cast<PointerType*>(this)); }

// BEGIN_INTERNALS_STRIP


        /**
         * Removes this element from a list
         *
         * This function can work since Any CDLL element can make the elements
         * around it point past it.
         */
        void    Remove()
                {
                    static_cast<CastType*>(m_PrevP)->m_NextP = m_NextP;
                    static_cast<CastType*>(m_NextP)->m_PrevP = m_PrevP;
                    m_NextP = static_cast<PointerType*>(this);
                    m_PrevP = static_cast<PointerType*>(this);
                }


// END_INTERNALS_STRIP

    protected:
        PointerType     *m_NextP;
        PointerType     *m_PrevP;
    };


} // namespace VDK


#endif /* VDK_LIST_ELEMENT_H_ */





