/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: APIs that relate to the status information of messages
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <new>

#include "TMK.h"

#include "VDK_Message.h"
#include "Message.h"
#include "MessageQueue.h"
#include "VDK_MemPool.h"
#include "MemoryPoolTable.h"
#include "ThreadTable.h"
#include "VDK_Thread.h"
#include "IDTable.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Messages")
#pragma file_attr("Messages")

#ifndef __ADSPTS__
// TS does not like suppressing this warning
#pragma diag(suppress:1994:" multicharacter character literal potential portability problem  ")
#endif

#if __ADSP21000__ || __ADSPTS__
#define MESSAGE_SIGNATURE (static_cast<MessageID>(0x414d5347))
#define DEAD_MESSAGE_SIGNATURE (static_cast<MessageID>(0x444d5347))
#else
#define MESSAGE_SIGNATURE (static_cast<MessageID>('AMSG'))
#define DEAD_MESSAGE_SIGNATURE (static_cast<MessageID>('DMSG'))
#endif

#define MSG_TST(pMessage) ((pMessage)->ID() == MESSAGE_SIGNATURE)

namespace VDK
{
	extern PoolID			g_MessagePoolID ;
	extern MemoryPoolTable	g_MemoryPoolTable;

	void
	GetMessageDetails(MessageID inMessageID,
					  MessageDetails *pOutMessageDetails,
					  PayloadDetails *pOutPayloadDetails)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kNoError;

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);

		if ERRCHK( MSG_TST(pMessage) )
		{
			if ERRCHK(!pMessage->IsInList())
			{
				if ERRCHK( GetThreadID() == pMessage->Owner() )
				{
					if (NULL != pOutMessageDetails)
					{
						pOutMessageDetails->channel = pMessage->Channel();
						pOutMessageDetails->sender  = pMessage->Sender();
						pOutMessageDetails->target  = pMessage->Target();
					}

					if (NULL != pOutPayloadDetails)
					{
						pOutPayloadDetails->type = pMessage->PayloadType();
						pOutPayloadDetails->size = pMessage->PayloadSize();
						pOutPayloadDetails->addr = pMessage->PayloadPtr();
					}
				}
				else
				{
					// Throw an error: only the owner is allowed to 
					// 		access the internals of a message.
					errorCode = kInvalidMessageOwner;
				}
			}
			else
			{
				// Can't access a message while it's on a queue
				errorCode = kMessageInQueue;
			}
		}
		else
		{
			// message ID table did not return a valid pointer
			errorCode = kInvalidMessageID;
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, inMessageID);
#endif
	}

	SystemError
	GetMessageStatusInfo( MessageID			inMessageID,
						  MessageDetails	*outMessageDetails,
						  PayloadDetails	*outPayloadDetails )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		SystemError errorCode = kInvalidMessageID;

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);

		 
		// As the calling thread does not need to be the owner, we must
		// disable scheduling in case the owner decides to destroy it.
		TMK_AcquireMasterKernelLock();
		
		// If the message was found in MessageID table...
		if ( MSG_TST(pMessage) )
		{
			// Like GetMessageDetails, NULL pointers can be used.
			if( NULL != outMessageDetails )
			{
				outMessageDetails->channel	= pMessage->Channel();
				outMessageDetails->sender	= pMessage->Sender();
				outMessageDetails->target	= pMessage->Target();
			}

			if( NULL != outPayloadDetails )
			{
				outPayloadDetails->type		= pMessage->PayloadType();
				outPayloadDetails->size		= pMessage->PayloadSize();
				outPayloadDetails->addr		= pMessage->PayloadPtr();
			}
			errorCode = kNoError;
		}
		TMK_ReleaseMasterKernelLock();
		// The message doesn't exist, or it's a dead message.
		// Either way it's an error.
		return errorCode;

	}


	int GetAllMessages( MessageID	outMessageIDArray[],
						int			inArraySize )
	{

		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		if NERRCHK( (inArraySize != 0) && (outMessageIDArray == NULL) )
			DispatchThreadError( kInvalidPointer, 0 );

		// Excluding this from an any locking/checking, assuming the user will
		// *not* delete the memory pool.
		MemoryPool *pMemoryPool = g_MemoryPoolTable.GetObjectPtr(g_MessagePoolID);

		TMK_AcquireMasterKernelLock();
		int numMsgs = pMemoryPool->NumAllocBlocks();
		
		if(inArraySize > 0)
		{
			// The messages are stored in memory pool blocks that are
			// sizeof(message) in size. As such we are going to treat
			// it as an array of messages.
			Message *nextMsg = reinterpret_cast<Message*>(pMemoryPool->m_PoolStart);

			// Make sure we only fill the right number of elements and not 
			// overflow the array
			int numToFill	= ( numMsgs < inArraySize? numMsgs : inArraySize );
			int filled		= 0;

			// We walk the memory pool checking if each block is a message,
			// filling an array with any that are.
			while( filled < numToFill )  
			{
				if(MSG_TST(nextMsg))
				{
					outMessageIDArray[filled++] = static_cast<MessageID>(
										reinterpret_cast<unsigned int>(nextMsg));
				}
				nextMsg++;
			}
		}
		TMK_ReleaseMasterKernelLock();
		
		return numMsgs;
	}

}	// namespace VDK

