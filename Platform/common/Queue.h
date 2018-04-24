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
 *  Comments: The wrapper of the base class for arrays of ProrityLists, such as
 *	ReadyQueue and MessageQueue.
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef QUEUE_H
#define QUEUE_H
#include "QueueMgr.h"

namespace VDK
{

	template <class T, int tableSize>
	class Queue : public QueueMgr
	{
		public:

	        T*   		   GetNextElement()
	        			   	{ return static_cast<T*>(VDK::QueueMgr::GetNextElement(m_ReadyElements)); }

	        T*   		   GetNextElementAtPriority(Priority priority)
	        			   	{ return static_cast<T*>(m_ReadyElements[priority].GetNext()); }

	        void           Remove(T* theElement)
						   	{ VDK::QueueMgr::Remove(m_ReadyElements, static_cast<T*>(theElement)); }

	        void           Insert(T* theElement)
						   	{ VDK::QueueMgr::Insert(m_ReadyElements, static_cast<T*>(theElement)); }

	        bool           MoveToEnd(T* theElement)
						   	{ return VDK::QueueMgr::MoveToEnd(m_ReadyElements, static_cast<T*>(theElement)); }

	        bool           MultipleElementsPresent(const VDK::Priority inPriority)
						   	{ return m_ReadyElements[inPriority].MultipleElementsPresent(); }

	    protected:
			VDK::PriorityListMgr    m_ReadyElements[tableSize];

	};


} // namespace VDK

#endif //QUEUE_H




