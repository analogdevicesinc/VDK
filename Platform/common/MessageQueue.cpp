/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The queue of threads ready to run
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "VDK_API.h"
#include "Message.h"
#include "MessageQueue.h"
#include "VDK_History.h"
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")

namespace VDK 
{ 
    extern unsigned int         kNumMarshalledTypes;
    extern MarshallingEntry     g_vMarshallingTable[];

    void  MessageQueueCleanup(MessageQueue* inMessageQueue)
    {
		if (NULL != inMessageQueue)
			inMessageQueue->~MessageQueue();  // call destructor
    }

    ////////////////////////////////////////
	// MessageQueue destructor
    ////////////////////////////////////////
    VDK::MessageQueue::~MessageQueue()
    {
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		while (0 != this->PrioritiesWithElements())
		{
			Message *pMessage = this->GetNextElement();
			this->Remove(pMessage);

			if (ISMARSHALLEDTYPE(pMessage->PayloadType())) // if it's a marshalled type
			{
				int index = MARSHALLINGINDEX(pMessage->PayloadType());
		
				if (index < kNumMarshalledTypes)
				{
					MsgWireFormat msgBuff;
					msgBuff.payload.type = pMessage->PayloadType();
					msgBuff.payload.size = pMessage->PayloadSize();
					msgBuff.payload.addr = pMessage->PayloadPtr();
					(*g_vMarshallingTable[index].pfMarshaller)(RELEASE,
															   &msgBuff,
															   NULL,
															   g_vMarshallingTable[index].area,
															   0 /* timeout */);
				}
			}

			delete pMessage;

			HISTORY_(kMessageDestroyed, pMessage->ID(), GetThreadID());
		}
    }
}




