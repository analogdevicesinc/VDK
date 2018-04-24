/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread RThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_RThread.h"
#include "Heap.h"
#include <new>
#include "VDK_API.h"
#include "Message.h"
#include "MessageQueue.h"
#include "ThreadTable.h"
#include "VDK_Thread.h"
#include "IDTable.h"
#include "IODeviceTable.h"
#include "KernelPanic.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")

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

int rthreadMsgCount = 0;

namespace VDK
{													   
	extern MarshallingEntry     g_vMarshallingTable[];
	extern unsigned int kNumMarshalledTypes;

	extern unsigned int g_localNode;
    
	extern MsgThreadEntry g_RoutingThreadInfo[];
    extern unsigned int kNumRoutingThreads;    // Number of routing threads

	SemaphoreID g_rthreadSemaphore = static_cast<VDK::SemaphoreID>(-1);

	inline ThreadID reconstituteThreadID(int nodeAndIndex)
	{
		unsigned int result = nodeAndIndex & 0x3FFF;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		// If the node number refers to the local processor then
		// expand the ID with the count from the thread table
		//
		if (g_localNode == IDTableMgr::Node_From_ObjectID(nodeAndIndex))
			result |= g_ThreadTable.Count_For_Index(nodeAndIndex & 0x1FF);
#endif

		return static_cast<ThreadID>(result);
	}


void InstallMessageControlSemaphore(SemaphoreID inSemaphore)
{
	if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
	
	g_rthreadSemaphore = inSemaphore;
}


/******************************************************************************
 *  RThread Run Function (RThread's main{})
 */
 
void
RThread::Run()
{
#ifdef __ADSPTS__
// for TS we need to have the variable aligned to 4 because it will be used
// with DMA which can only cope with this alignment.
#pragma align 4
#endif
	MsgWireFormat msgBuff;
	DeviceInfoBlock messageDev;

	messageDev.dd =	OpenDevice(m_pParams->device, m_pParams->psDevArg);

	if (kOUTGOING == m_pParams->direction) while (1)
    {
		// Get the next message from the queue
		HISTORY_(kMessagePended, 0x7FFF, ID());

		TMK_AcquireMasterKernelLock();

		unsigned maskedQueue = MessageQueue()->PrioritiesWithElements();

		// If the stated conditions for waiting messages are not met, then block...
		if( 0 == maskedQueue )
		{
			// Save off the arguments to PendMessage() so that PostMessage has enough
			// information to determine whether or not it should unblock the thread.
			MessageQueue()->MsgChannelMask() = 0x7FFF;
			MessageQueue()->WaitForMsgOnAllChannels() = false;

			HISTORY_(VDK::kThreadStatusChange, runStatus, ID()); // log previous status
#pragma suppress_null_check
			TMK_Thread *pThread = this;
			TMK_MakeThreadNotReady(pThread, kMessageBlocked);

			TMK_Block();

			// Another thead has called PostMessage which has initiated a context switch back to 
			// this thread.  Execution resumes here...			

			// Reset the "waiting" mask since we're not pending anymore
			MessageQueue()->MsgChannelMask() = 0;

			// Recompute maskedQueue from new state
			maskedQueue = MessageQueue()->PrioritiesWithElements();
		}
			
		// Get a pointer to the highest priority message
		//
		VDK::Priority priority = static_cast<VDK::Priority>(FindFirstSet(maskedQueue));
	    Message *pMessage = MessageQueue()->GetNextElementAtPriority(priority);

		if (MSG_TST(pMessage)) // ...and remove it from the message queue.
			MessageQueue()->Remove(pMessage);
		else
			pMessage = NULL;

		TMK_ReleaseMasterKernelLock();

		if (NULL == pMessage)
			DispatchThreadError(kInvalidMessageID, 0);

		// Extract its details
		MsgChannel   channel = pMessage->Channel();
		ThreadID     sender  = pMessage->Sender();
		ThreadID     target  = pMessage->Target();

		// Pack them into the header word
		int channelNumber = 15 - FindFirstSet(channel);
		msgBuff.header       = (channelNumber << 28) | ((target & 0x3FFF) << 14) | (sender & 0x3FFF);

		// Copy the payload details
		msgBuff.payload.type = pMessage->PayloadType();
		msgBuff.payload.size = pMessage->PayloadSize();
		msgBuff.payload.addr = pMessage->PayloadPtr();
		rthreadMsgCount++;
		
		if (ISMARSHALLEDTYPE(msgBuff.payload.type)) // if it's a marshalled type
		{
			int index = MARSHALLINGINDEX(msgBuff.payload.type);
		 
#if (VDK_INSTRUMENTATION_LEVEL_>0)
			if (index >= kNumMarshalledTypes)
				DispatchThreadError(kInvalidMarshalledType, msgBuff.payload.type);
#endif
			(*g_vMarshallingTable[index].pfMarshaller)(TRANSMIT_AND_RELEASE,
										  &msgBuff,
										  &messageDev,
										  g_vMarshallingTable[index].area,
										  0 /* timeout */);
		}
		else
		{
			// Punt it over the wire
			SyncWrite(messageDev.dd, (char*)&msgBuff, sizeof (msgBuff), 0);
		}

		pMessage->ID() = DEAD_MESSAGE_SIGNATURE;
			
		// and free the memory.
		delete pMessage;
		HISTORY_(kMessageDestroyed, reinterpret_cast<unsigned int>(pMessage), ID());

		if (static_cast<SemaphoreID>(-1) != g_rthreadSemaphore)
			PostSemaphore(g_rthreadSemaphore);
    }
    else while (1)  // kINCOMING == m_pParams->direction
    {
		// Get the next message packet from the wire
		//
		SyncRead(messageDev.dd, (char*)&msgBuff, sizeof(msgBuff), 0);

		int channelNumber  = msgBuff.header >> 28;
		MsgChannel channel = static_cast<MsgChannel>((unsigned)0x8000 >> channelNumber);
		ThreadID   target  = reconstituteThreadID(msgBuff.header >> 14);
		ThreadID   sender  = reconstituteThreadID(msgBuff.header);
		rthreadMsgCount++;
		
		if (static_cast<SemaphoreID>(-1) != g_rthreadSemaphore)
			PendSemaphore(g_rthreadSemaphore, 0);

		if (ISMARSHALLEDTYPE(msgBuff.payload.type)) // if it's a marshalled type
		{
			int index = MARSHALLINGINDEX(msgBuff.payload.type);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
			if (index >= kNumMarshalledTypes)
				DispatchThreadError(kInvalidMarshalledType, msgBuff.payload.type);
#endif
			(*g_vMarshallingTable[index].pfMarshaller)(ALLOCATE_AND_RECEIVE,
										  &msgBuff,
										  &messageDev,
										  g_vMarshallingTable[index].area,
										  0 /* timeout */);
		}

	    MessageID msgID = MESSAGE_SIGNATURE;

		// Instantiate the message object ...
		//   Note that the alternate version of operator new() 
		// (which uses MallocBlock()) will be invoked.
		Message	*pMessage = new Message(msgID, msgBuff.payload.type, msgBuff.payload.size, msgBuff.payload.addr);

		// If the constructor has succeeded, add the object to the ID table, 
		// and return the MessageID provided by the IDTable.
		if ERRCHK( NULL != pMessage )
		{
			pMessage->Owner() = ID();
			HISTORY_(kMessageCreated, msgID, ID());
			msgID = static_cast<MessageID>(reinterpret_cast<unsigned int>(pMessage));
		}
		else
		{
			// The Message class contructor has failed, so dispatch an error 
			// and return (-1) as the MessageID.
			msgID = static_cast <MessageID> (UINT_MAX);
			DispatchThreadError(kMaxCountExceeded, msgBuff.payload.type);
		}

		pMessage->Owner() = sender;
		PostMessage(target, msgID, channel);
    }

	//    CloseDevice(messageDev.dd); not reached
}

/******************************************************************************
 *  RThread Error Handler
 */
 
int
RThread::ErrorHandler()
{
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  RThread Constructor
 */
 
RThread::RThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	m_pParams = (VDK::MsgThreadEntry *)tcb.user_data_ptr;
}

/******************************************************************************
 *  RThread Destructor
 */
 
RThread::~RThread()
{
}

/******************************************************************************
 *  RThread Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
RThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
    return new (tcb) RThread(tcb);
}


bool InitRoutingThreads() {

    for (int i = 0; i < kNumRoutingThreads; ++i)
    {

		ThreadCreationBlock tcb;
		tcb.template_id       = (ThreadType)-1;
		tcb.thread_priority   = static_cast<VDK::Priority>(0);
		tcb.thread_stack_size = 0;
		tcb.user_data_ptr     = &g_RoutingThreadInfo[i];
		g_RoutingThreadInfo[i].tplate.createFunction=RThread::Create;
		tcb.pTemplate         = &(g_RoutingThreadInfo[i].tplate);

		ThreadID rthread = CreateThreadEx (&tcb);

		// verify that the return thread ID is the one we expect
		if (rthread != g_RoutingThreadInfo[i].threadId)
			KernelPanic(kBootError, kThreadCreationFailure,g_RoutingThreadInfo[i].threadId);
    }
    
    return false;
}

}; // namespace VDK

/* ========================================================================== */
