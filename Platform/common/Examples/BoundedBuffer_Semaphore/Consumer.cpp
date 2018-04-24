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
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

extern VDK::MutexID mutex;
extern int bufferSize;
extern int buffer[];

int out_index = 0;

/******************************************************************************
 *  Consumer Run Function (Consumer's main{})
 */
 
void
Consumer::Run()
{
    while (1)
    {									// Decrement the 'full' count as we are
   		VDK::PendSemaphore(kFull,0);	// removing a number from the buffer

   	    // It is feasable that two or more instances of Producer/Consumer could 
   	    // be executing here at the same time. To protect the global variables 
   	    // 'out_index', 'bufferSize and 'buffer' the following section of code
   	    // must be made mutually exclusive. This can be done by using a binary
   	    // semaphore and bookending the section of code with calls to pend and 
   	    // post the semaphore.
		VDK::PendSemaphore(kMutex,0);
		
		// Increment the 'Out' index by one.
		out_index = (out_index + 1) % bufferSize;
   		
   		// Read an integer from the shared buffer
		int number = buffer[out_index];
		printf("%d	Retrieved from buffer at index: %d\n", number, out_index);
		
		VDK::PostSemaphore(kMutex);
		VDK::PostSemaphore(kEmpty);	// Increment the 'empty' count as we are
    }								// adding a number to the buffer
}

/******************************************************************************
 *  Consumer Error Handler
 */
 
int
Consumer::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
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

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  Consumer Destructor
 */
 
Consumer::~Consumer()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

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
