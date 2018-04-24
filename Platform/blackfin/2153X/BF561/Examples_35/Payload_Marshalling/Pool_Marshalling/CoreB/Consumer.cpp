/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Consumer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/
 
#include "Consumer.h"
#include <new>
#include <stdlib.h>
#include <stdio.h>
#include "../ProdCons.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define N 10	// Number of messages

/******************************************************************************
 *  Local function to handle an incoming item
 */
 
static void consume_item (void *item)
{
	static int expected = 0;
	int *current = reinterpret_cast<int*>(item);
	
	// Expected message item has been received
	if (*current == expected) 
	{
		printf("Consumer ID %d Received %d from Producer\n", VDK::GetThreadID(), *current);
	}
	// Unexpected message item has been received
	else
	{
	 	printf("Consumer Error: Expecting %d Received %d\n",expected, *current);
	}
	expected++;

	VDK::Yield();
}


/******************************************************************************
 *  Consumer Run Function (Consumer's main{})
 */
 
void
Consumer::Run()
{
	// Send N empties
	for (int i = 0;
		 i < N;
		 ++i)
	{
		// Allocate and initialise memory block in memory pool
		void *mem_block = (void *) VDK::MallocBlock(kMarshalledMessages);
		strcpy((char *)mem_block,"empty block");
		
		// Create message, set message payload and send message
		VDK::MessageID msg = VDK::CreateMessage(MSG_TYPE, 0, NULL);
		VDK::SetMessagePayload(msg, MSG_TYPE, MBSIZE, mem_block);
		VDK::PostMessage(kProducer1, msg, MSG_CHANNEL);
	}
	
	while (1)
	{
		// Get message containing an item
		VDK::MessageID msg = VDK::PendMessage(MSG_CHANNEL, 0);
		
		// Extract the item from the message
		int type;
		unsigned int size;
		void *item;
		VDK::GetMessagePayload(msg, &type, &size, &item);
		
		// Do something with the item
		consume_item(item);
		
		// Send back message for re-use
		strcpy((char *)item,"empty block");
		VDK::SetMessagePayload(msg, MSG_TYPE, MBSIZE, item);
		VDK::PostMessage(kProducer1, msg, MSG_CHANNEL);
	}
}

/******************************************************************************
 *  Consumer Error Handler
 */
 
int
Consumer::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Consumer Constructor
 */
 
Consumer::Consumer(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Consumer Destructor
 */
 
Consumer::~Consumer()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Consumer Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Consumer::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Consumer(tcb);
}

/* ========================================================================== */
