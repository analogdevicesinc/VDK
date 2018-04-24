/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Producer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Producer.h"
#include <new>
#include <stdlib.h>
#include "../ProdCons.h"

 
/******************************************************************************
 *  Local function to generate an outgoing item
 */
 
static void produce_item(void **ppItem)
{
	static int i = 0;
	*ppItem = reinterpret_cast<void*>(i++);
}


/******************************************************************************
 *  Producer Run Function (Producer's main{})
 */
 
void
Producer::Run()
{
    while (1)
    {
		// Generate something to put in the message
		void *item;
		produce_item(&item);
		
		// Wait for an empty to arrive
		VDK::MessageID msg = VDK::PendMessage(MSG_CHANNEL, 0);
		
		// Find out where it came from
		VDK::MsgChannel channel;
		VDK::ThreadID sender;
		VDK::GetMessageReceiveInfo(msg, &channel, &sender);
		
		// Get message payload
		int type;
		unsigned int size;
		void *mem_block;
		VDK::GetMessagePayload(msg, &type, &size, &mem_block);
		
		// Reset this memory space and copy next item into it.
		memset(mem_block, 0, MBSIZE);
		memcpy(mem_block, &item, sizeof(int));
		
		// Setup and send new message item
		VDK::SetMessagePayload(msg, MSG_TYPE, MBSIZE, mem_block);
		VDK::PostMessage(sender, msg, MSG_CHANNEL);
    }
}

/******************************************************************************
 *  Producer Error Handler
 */
 
int
Producer::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Producer Constructor
 */
 
Producer::Producer(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Producer Destructor
 */
 
Producer::~Producer()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Producer Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Producer::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Producer(tcb);
}

/* ========================================================================== */
