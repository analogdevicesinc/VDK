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
#include "ProdCons.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define N 10


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
		VDK::MessageID empty = VDK::CreateMessage(EMPTY_MSG, 0, NULL);
		VDK::PostMessage(kProducer1, empty, EMPTY_MSG_CHANNEL);
	}
	
    while (1)
    {
    	// Get message containing an item
		VDK::MessageID msg = VDK::PendMessage(FULL_MSG_CHANNEL, 0);
        
        // Extract the item from the message
        int type;
        unsigned int size;
		void *item;
        VDK::GetMessagePayload(msg, &type, &size, &item);
        
        // At this point we would normally do something with the item

        // Send back empty reply
        VDK::SetMessagePayload(msg, EMPTY_MSG, 0, NULL);
		VDK::PostMessage(kProducer1, msg, EMPTY_MSG_CHANNEL);
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
