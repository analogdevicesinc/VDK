/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A prioritized storage area owned by a thread where messages
 *		wait to be acknowledged. 
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: A MessageQueue is an array of 16 MessageLists.  Each thread has
 *	an aggregated MessageQueue on which it may receive incoming messages.  Each
 *	of the sixteen MessageLists is referred to as a "channel" and they are 
 *	prioritized from high to low (i.e. 15 = highest, 0 = lowest).
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "Message.h"
#include "Queue.h"

namespace VDK
{

        class MessageQueue : public Queue<Message, 15> // 15 message priorities (channels) on all platforms
	{
		public:
			MessageQueue() 
				{
					m_MsgChannelMask=0; 
					m_WaitForMsgOnAllChannels = false;
				}

		    ~MessageQueue();

	        const unsigned&            MsgChannelMask() const
	                                            { return m_MsgChannelMask; }
	        unsigned&                  MsgChannelMask()
	                                            { return m_MsgChannelMask; }
	        const bool&                     WaitForMsgOnAllChannels() const
	                                            { return m_WaitForMsgOnAllChannels; }
	        bool&                           WaitForMsgOnAllChannels()
	                                            { return m_WaitForMsgOnAllChannels; }

			bool IsWaitConditionMet()
				{
					unsigned theMaskedQueue = PrioritiesWithElements() & m_MsgChannelMask;
					return m_WaitForMsgOnAllChannels ? (theMaskedQueue == m_MsgChannelMask) : (theMaskedQueue != 0);
				}

		private:
			unsigned				m_MsgChannelMask;		   
			bool			    		m_WaitForMsgOnAllChannels;

	};


typedef MessageQueue *MessageQueuePtr;

} // namespace VDK

#endif //MESSAGEQUEUE_H


