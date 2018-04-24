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
#include <stdio.h>
#include "../ProdCons.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  Local function to generate an outgoing item
 */
 
static int produce_item(void **ppItem)
{
	int val = rand();
	*ppItem = reinterpret_cast<void*>(val);
		   
	return val;
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
        int val = produce_item(&item);
        
        // Wait for an empty to arrive
    	VDK::MessageID msg = VDK::PendMessage(EMPTY_MSG_CHANNEL, 0);
        
    	// Find out where it came from
    	VDK::MsgChannel channel;
    	VDK::ThreadID sender;
    	VDK::GetMessageReceiveInfo(msg, &channel, &sender);
    	
        // Prepare the message for sending
		VDK::SetMessagePayload(msg, FULL_MSG, 0, item);
		
		printf("Producer ID %d sending random number %d to Consumer %d\n",
			   VDK::GetThreadID(),
			   val,
			   sender);
		// Send item to the provider of the empty message
		VDK::PostMessage(sender, msg, FULL_MSG_CHANNEL);
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
