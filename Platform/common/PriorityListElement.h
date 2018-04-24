/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The base class for objects that sit in non-time oriented 
 *		VDK lists.
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The base class for objects that sit in lists -- mainly threads
 *	and messages.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef __PRIORITYLISTELEMENT__
#define __PRIORITYLISTELEMENT__


#include <VDK_ListElement.h>

namespace VDK
{

	
    /**
	 * OLD:
	 *
     * The ThreadListElement class is an abstraction to allow A Thread
     * to be both a ListElement, and a TimeQueuElement.  Without this class
     * there are some name managment issues.  This class allows the 
     * compiler to resolve function call scoping...
	 *
	 * 10/18/01
	 * ThreadListElements have been generalized into PriorityListElements.
     */


	class PriorityListElement;

    class PriorityListElementMgr : public VDK::ListElement<VDK::PriorityListElementMgr>
    {
    };



    class PriorityListElement : public PriorityListElementMgr
    {
        public:
            inline VDK::Priority& Priority()
             { return m_Priority; }
        
            inline const VDK::Priority& Priority() const
             { return m_Priority; }
        private:
            VDK::Priority    m_Priority;
    };



}



#endif  // __PRIORITYLISTELEMENT__


