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
// Tigersharc does not like disabling this warning
#pragma diag(suppress:1994:"Multicharacter character literal potential portability problem")
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

    extern int	g_CriticalRegionCount;
    extern int	g_SchedulerRegionCount;
	extern int	g_remainingTicks;

	extern ThreadID g_vRoutingThreads[];
	extern ThreadID g_vRoutingThreads2[];
	extern unsigned int kNumRoutingThreads;
	extern unsigned int g_localNode ;
	extern unsigned short g_ChannelRoutingMask;
	extern MarshallingEntry     g_vMarshallingTable[];

/*************************************************
 *
 *   Create & Destroy Messages
 *
 */
	MessageID
	CreateMessage(int inPayloadType, unsigned int inPayloadSize, void *inPayloadAddr)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		SystemError errorCode = kNoError;

	    MessageID msgID = MESSAGE_SIGNATURE;

		// Instantiate the message object ...
		//   Note that the alternate version of operator new() 
		// (which uses MallocBlock()) will be invoked.
		Message	*pMessage = new Message(msgID, inPayloadType, inPayloadSize, inPayloadAddr);

		// If the constructor has succeeded, add the object to the ID table, 
		// and return the MessageID provided by the IDTable.
		if ( pMessage != 0 )
		{
			pMessage->Owner() = GetThreadID();
			msgID = static_cast<MessageID>(reinterpret_cast<unsigned int>(pMessage));
			HISTORY_(kMessageCreated, msgID, GetThreadID());
		}
		else
		{
			// The Message class contructor has failed, so dispatch an error 
			// and return (-1) as the MessageID.
			errorCode  = kMaxCountExceeded;
			msgID = static_cast <MessageID> (UINT_MAX);
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, inPayloadType);
#endif

		return msgID;
	}


/*************************************************
 *
 *   Destroy a message & free its associated memory
 *
 */
	void
	DestroyMessage(MessageID inMessageID)
	{	
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kNoError;

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);

		// If the message was found in MessageID table...
		if ERRCHK( MSG_TST(pMessage) )
		{
			if ERRCHK(!pMessage->IsInList())
			{
				if ERRCHK( GetThreadID() == pMessage->Owner() )
				{
					pMessage->ID() = DEAD_MESSAGE_SIGNATURE;

					// remove it from any msg queue it might be sitting in,
					// this is really a no-op as it is illegal to destroy a
					// message while it's on a queue.
					pMessage->Remove();
			
					// and free the memory.
					delete pMessage;
					HISTORY_(kMessageDestroyed, inMessageID, GetThreadID());
				}
				else
				{
					// Throw an error: only the owner is allowed to destroy a message.
					errorCode = kInvalidMessageOwner;
				}
			}
			else
			{
				// Can't destroy a message while it's on a queue
				errorCode = kMessageInQueue;
			}
		}
		else
		{
			// Throw an error because it was not in the ID table
	        errorCode = kInvalidMessageID;
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, inMessageID);
#endif
	}


/*************************************************
 *
 *   Destroy a message & free its associated payload
 *
 */
	void
	DestroyMessageAndFreePayload(MessageID inMessageID)
	{	
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kNoError;

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);

		// If the message was found in MessageID table...
		if ERRCHK( MSG_TST(pMessage) )
		{
			if ERRCHK(!pMessage->IsInList())
			{
				if ERRCHK( GetThreadID() == pMessage->Owner() )
				{
					if (ISMARSHALLEDTYPE(pMessage->PayloadType())) // if it's a marshalled type
					{
						int index = MARSHALLINGINDEX(pMessage->PayloadType());
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
					
					pMessage->ID() = DEAD_MESSAGE_SIGNATURE;

					// remove it from any msg queue it might be sitting in,
					// this is really a no-op as it is illegal to destroy a
					// message while it's on a queue.
					pMessage->Remove();
			
					// and free the memory.
					delete pMessage;
					HISTORY_(kMessageDestroyed, inMessageID, GetThreadID());
				}
				else
				{
					// Throw an error: only the owner is allowed to destroy a message.
					errorCode = kInvalidMessageOwner;
				}
			}
			else
			{
				// Can't destroy a message while it's on a queue
				errorCode = kMessageInQueue;
			}
		}
		else
		{
			// Throw an error because it was not in the ID table
	        errorCode = kInvalidMessageID;
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, inMessageID);
#endif
	}


/*************************************************
 *
 *   Free a message's associated payload, resets
 * the payload atributes to all zeros.
 *
 */
	void
	FreeMessagePayload(MessageID inMessageID)
	{	
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		SystemError errorCode = kNoError;

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);

		// If the message was found in MessageID table...
		if ERRCHK( MSG_TST(pMessage) )
		{
			if ERRCHK(!pMessage->IsInList())
			{
				if ERRCHK( GetThreadID() == pMessage->Owner() )
				{
					if (ISMARSHALLEDTYPE(pMessage->PayloadType())) // if it's a marshalled type
					{
						int index = MARSHALLINGINDEX(pMessage->PayloadType());
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

					pMessage->PayloadType() = 0;
					pMessage->PayloadSize() = 0;
					pMessage->PayloadPtr()  = NULL;
				}
				else
				{
					// Throw an error: only the owner is allowed to destroy a message.
					errorCode = kInvalidMessageOwner;
				}
			}
			else
			{
				// Can't destroy a message while it's on a queue
				errorCode = kMessageInQueue;
			}
		}
		else
		{
			// Throw an error because it was not in the ID table
	        errorCode = kInvalidMessageID;
		}

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, inMessageID);
#endif
	}


/*************************************************
 *
 *   Post Message
 *
 */
	void
	PostMessage(ThreadID 		inTargetID, 
				MessageID 		inMessageID,
				MsgChannel		inChannel)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		// Convert the inChannel mask into a bit number, i.e. a priority
		int channelNumber = FindFirstSet(inChannel);
		Priority priority = static_cast<Priority>(channelNumber);

#if (VDK_INSTRUMENTATION_LEVEL_ > 0)
		// There are only 15 message channels/priorities, regardless of platform.
		// Only 1 bit should be set in the inChannel mask.
		if( (channelNumber < 0) || (channelNumber > 14) || (inChannel != (1 << channelNumber)) )
		{
			DispatchThreadError(kInvalidMessageChannel, inChannel);
			return;
		}
#endif

		Message *pMessage = reinterpret_cast<Message *>(inMessageID);
		const int dstNode = IDTableMgr::Node_From_ObjectID(inTargetID); // get the index of the target chip

		TMK_AcquireMasterKernelLock(); // freeze the ready queue and thread table

		Thread *pDstThread;
		ThreadID dstThread = inTargetID;

		if (dstNode != g_localNode)
		{
			// message is being sent off-node

			if NERRCHK(dstNode >= kNumRoutingThreads)
			{
				DispatchThreadError(kInvalidTargetDSP, dstNode);
				return;
			}
		    if (pMessage->Channel() & g_ChannelRoutingMask)
				dstThread = g_vRoutingThreads2[dstNode];
			else
				dstThread = g_vRoutingThreads[dstNode];
	    }

		// Get the pointer to the destination thread
	    pDstThread = g_ThreadTable.GetObjectPtr(dstThread);

		// Do lots of (optional) error checking
		//
		SystemError errorCode = kNoError;
		int errorValue = 0;

		// ...and if a valid ThreadID was found in ThreadTable...
		if NERRCHK( pDstThread == 0 )
		{
			// Throw an error: the ThreadID was not found in the ThreadTable...
			errorCode  = kUnknownThread;
			errorValue = inTargetID;
		}
		else if NERRCHK( !MSG_TST(pMessage) ) // ...and if a valid MessageID was found in MessageTable...
		{
			// Throw an error: the message pointer returned from the table was invalid...
			errorCode  = kInvalidMessageID;
			errorValue = inMessageID;
		}
		else if NERRCHK(pMessage->IsInList())
		{
			errorCode  = kMessageInQueue;
			errorValue = pMessage->Owner();
		}
		else if NERRCHK(NULL == pDstThread->MessageQueue()) 
		{
			// Throw an error: the destination thread does not have a message queue
			errorCode  = kInvalidMessageRecipient;
			errorValue = pDstThread->ID();
		}
#if 0 /* this broke message forwarding */
		else if NERRCHK( GetThreadID() != pMessage->Owner() && pMessage->Sender() != pMessage->Owner() )
		{
			// Throw an error: only the owner is allowed to post a message.
			errorCode  = kInvalidMessageOwner;
			errorValue = pMessage->Owner();
		}
#endif
		else
		{
			HISTORY_(kMessagePosted, inMessageID, GetThreadID());

			pMessage->Sender()   = pMessage->Owner();
			pMessage->Owner()    = pDstThread->ID();
			pMessage->Target()   = inTargetID;
			pMessage->Priority() = priority;

			pDstThread->MessageQueue()->Insert(pMessage);

			// Check first for the destination thread being in the
			// ready state. This early-out test should accelerate any
			// case of posting to an unblocked thread, including the
			// post-to-self case, by avoiding evaluation of the wait
			// condition.
			if( kReady != pDstThread->runStatus &&
				pDstThread->MessageQueue()->IsWaitConditionMet() )
			{
				// Is the message out of the ready queue because it is pending on a message?
				switch(pDstThread->runStatus)
				{
				case kMessageBlockedWithTimeout:
					TMK_CancelTimeout(pDstThread);  // safe wrt. RescheduleISR
				case kMessageBlocked:
					// Put the thread back into the ready queue
				    HISTORY_(VDK::kThreadStatusChange, pDstThread->runStatus, pDstThread->ID()); // log previous status
					TMK_MakeThreadReady(pDstThread);

					// We've changed the ready queue so we may need to 
					// context-switch.
					TMK_Reschedule();
				}
			}
		}

		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (kNoError != errorCode)
			DispatchThreadError(errorCode, errorValue);
#endif
	}



/*************************************************
 *
 *   Receive Message
 *
 */
	MessageID
	PendMessage(unsigned int inMsgChannelMask, Ticks inTimeout)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		// the thread did not have a MessageQueue so it could not pend on a message
		if (!pThread->MessageQueue()) {
			DispatchThreadError(kInvalidThread, 0);
			return static_cast<MessageID>(UINT_MAX);
		}
#endif
		bool waitForMsgOnAllChannels = (0 != (inMsgChannelMask & kMsgWaitForAll));
		inMsgChannelMask &= 0x7FFF;
	
#if (VDK_INSTRUMENTATION_LEVEL_ > 0 )
		// Check that the timeout is within range
		if (INT_MAX == inTimeout || (Ticks) INT_MIN == inTimeout)
		{
			// We had an out-of-range timeout passed in
			DispatchThreadError(kInvalidTimeout, inTimeout);
			return static_cast<MessageID>(UINT_MAX);
		}

		if (TMK_IsSchedulingSuspended() && kDoNotWait != inTimeout)
		{
			// We are going to perform a Pend, and we may block.
			// This is a possible error state, so throw an error to the current thread
			DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
		}

		// Only the bottom 15 bits in the mask are valid, and at least 1 must be set
		if ( (0 == inMsgChannelMask) || ((0x7FFF & inMsgChannelMask) != inMsgChannelMask) )
		{
			DispatchThreadError(kInvalidMessageChannel, inMsgChannelMask);
			return static_cast<MessageID>(UINT_MAX);
		}
#endif
		HISTORY_(kMessagePended, inMsgChannelMask, GetThreadID());

		TMK_AcquireMasterKernelLock();

		unsigned maskedQueue = pThread->MessageQueue()->PrioritiesWithElements() & inMsgChannelMask;

		// If the stated conditions for waiting messages are not met, then block...
		if( waitForMsgOnAllChannels ? (maskedQueue != inMsgChannelMask) : (maskedQueue == 0))
		{
			// If inTimeout is 0xFFFFFFFF then don't block, return failure
			if (kDoNotWait == inTimeout)
			{
				TMK_ReleaseMasterKernelLock();
				return static_cast<MessageID>(UINT_MAX);
			}
				
#if (VDK_INSTRUMENTATION_LEVEL_>0)
			// We're going to block, check that we aren't inside a region.
			if (0 != g_SchedulerRegionCount)
			{
				TMK_ReleaseMasterKernelLock();
				DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
				return static_cast<MessageID>(UINT_MAX);
			}
#endif
			// Save off the arguments to PendMessage() so that PostMessage has enough
			// information to determine whether or not it should unblock the thread.
			pThread->MessageQueue()->MsgChannelMask() = inMsgChannelMask;
			pThread->MessageQueue()->WaitForMsgOnAllChannels() = waitForMsgOnAllChannels;

			if (inTimeout != 0 && inTimeout != kNoTimeoutError)
			{
			    HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
				TMK_MakeThreadNotReady(pThread, kMessageBlockedWithTimeout);

				// Put the thread into the time queue. Use TMK_ResetTimeout() instead of
				// TMK_SetTimeout() in case the current thread is round-robin.
				//
				g_remainingTicks = TMK_ResetTimeout(pThread, inTimeout & INT_MAX);   // mask out the sign bit
			}
			else
			{
			    HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
				TMK_MakeThreadNotReady(pThread, kMessageBlocked);
			}

			TMK_Block();

			// Another thread has called PostMessage which has initiated a context switch back to 
			// this thread, or else a timeout has occurred.  Execution resumes here...			

			// Reset the "waiting" mask since we're not pending anymore
			pThread->MessageQueue()->MsgChannelMask() = 0;

			// Check the error status.  This is set in Thread::Timeout()
			if (pThread->TimeoutOccurred())
			{
				TMK_ReleaseMasterKernelLock();
				if ((int)inTimeout > 0)   // test sign bit
					DispatchThreadError(kMessageTimeout, inTimeout);
				pThread->TimeoutOccurred() = false;

				return static_cast<MessageID>(UINT_MAX);
			}

			// Recompute maskedQueue from new state
			maskedQueue = pThread->MessageQueue()->PrioritiesWithElements() & inMsgChannelMask;
		}
			
		// Get a pointer to the highest "priority" message ***of those specified in inMsgChannelMask***
		//
		Priority priority = static_cast<Priority>(FindFirstSet(maskedQueue));
	    Message *pMessage = pThread->MessageQueue()->GetNextElementAtPriority(priority);
		MessageID result = static_cast<MessageID>(UINT_MAX);

		if (MSG_TST(pMessage))
		{
			// ...and remove it from the message queue.
			pThread->MessageQueue()->Remove(pMessage);
			result = static_cast<MessageID>(reinterpret_cast<unsigned int>(pMessage));
		}

		TMK_ReleaseMasterKernelLock();

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (pMessage == 0)
		{
			// There was *no* waiting message in the table, this is an
			// internal error, i.e. VDK has messed up, but we bring it
			// to the user's attention anyway.
			DispatchThreadError(kInvalidMessageID, 0);
		}
#endif

		return result;
	}


	bool 
	MessageAvailable(unsigned int inMsgChannelMask)
	{
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
		bool waitForMsgOnAllChannels = (0 != (inMsgChannelMask & kMsgWaitForAll));
		inMsgChannelMask &= 0x7FFF;

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		if (!pThread->MessageQueue()) 
		{
			DispatchThreadError(kInvalidThread, 0);
			return false;
		}
		// Only the bottom 15 bits in the mask are valid, and at least 1 must be set
		if ( (0 == inMsgChannelMask) || ((0x7FFF & inMsgChannelMask) != inMsgChannelMask) )
		{
			DispatchThreadError(kInvalidMessageChannel, inMsgChannelMask);
			return false;
		}
#endif

		unsigned maskedQueue = pThread->MessageQueue()->PrioritiesWithElements() & inMsgChannelMask;
	
		bool result = waitForMsgOnAllChannels ? (maskedQueue == inMsgChannelMask) : (maskedQueue != 0);
	
		return result;
	}


	void
	GetMessagePayload(MessageID inMessageID,
					  int *pOutPayloadType,
					  unsigned int *pOutPayloadSize,
					  void **ppOutPayloadAddr)
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
					*pOutPayloadType  = pMessage->PayloadType();
					*pOutPayloadSize  = pMessage->PayloadSize();
					*ppOutPayloadAddr = pMessage->PayloadPtr();
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
			DispatchThreadError(errorCode,inMessageID);
#endif
	}


	void
	SetMessagePayload(MessageID inMessageID, 
					  int inPayloadType,
					  unsigned int inPayloadSize,
					  void *inPayloadAddr)
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
					pMessage->PayloadType() = inPayloadType;
					pMessage->PayloadSize() = inPayloadSize;
					pMessage->PayloadPtr()  = inPayloadAddr;
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


	void
	GetMessageReceiveInfo(MessageID inMessageID,
						  MsgChannel *pOutChannel,
						  ThreadID   *pOutSender)
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
					*pOutChannel = pMessage->Channel();
					*pOutSender  = pMessage->Sender();
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


	void
	ForwardMessage(ThreadID 		inTargetID, 
				   MessageID 		inMessageID,
				   MsgChannel		inChannel,
				   ThreadID         inPseudoSender
				   )
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
					pMessage->Owner() = inPseudoSender;
					PostMessage(inTargetID, inMessageID, inChannel);
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


}	// namespace VDK


/*==============================================================================
 * FOOT NOTES :
 *
 * FN#1 : A bit of non-obvious information:
 *
 *  When pending on a msg, the thread is somewhat "hidden" in that is not sitting in any 
 *  lists as it would be if it were pending on a semaphore, for example, or if it were 
 *  sleeping.  However, if another thread posts a message to this thread's ID, the pointer 
 *  to this thread will be found in the ThreadTable, and the thread will be scheduled. 
 *  When a thread pends on a message, it is critical that another thread have the 
 *  pending thread's ID (or ptr, but those aren't exposed to users).  
 *
 * FN#2 : Conversion between kMsgChannelXX and kPriorityYY
 *
*	kPriorityYY = totalNumBits - 2 - static_cast<Bitfield>(kMsgChannelXX).FindFirstSet();
 *	kMsgChannelXX = 1 << (kPriorityYY + 2 + totalNumBits);
 *		where:  totalNumBits = sizeof(int) * CHAR_BIT;
 *              (the native word size of the machine)
 *
 * ==========================================================================*/

