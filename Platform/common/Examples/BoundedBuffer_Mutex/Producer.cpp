/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Producer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Producer.h"
#include <stdlib.h>
#include <stdio.h>
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int bufferSize = BUFFER_SIZE;

int in_index;
VDK::MutexID mutex;

/******************************************************************************
 *  Producer Run Function (Producer's main{})
 */
  
void
Producer::Run()
{
    mutex = VDK::CreateMutex();
    
    while (1)
    {									// Decrement the 'empty' count as we are
   		VDK::PendSemaphore(kEmpty,0);	// adding a number to the buffer.
   		
   	    // It is feasable that two or more instances of Producer/Consumer could 
   	    // be executing here at the same time. To protect the global variables 
   	    // 'in', 'bufferSize and 'buffer' the following section of code must be 
   	    // made mutually exclusive. This can be done by bookending the section 
   	    // of code with calls to the Acquire and Release Mutex APIs.
   	    VDK::AcquireMutex(mutex);

		// Increment the 'In' Index by one
		in_index = (in_index + 1) % bufferSize;
   		
   		// Write a random integer to the shared buffer.
		buffer[in_index] = rand();
		printf("%d	Written to buffer at index: %d\n", buffer[in_index], in_index);
		
		VDK::ReleaseMutex(mutex);
		VDK::PostSemaphore(kFull);		// Increment the 'full' count as a number
	}									// has just been added to the buffer.
}

/******************************************************************************
 *  Producer Error Handler
 */
 
int
Producer::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
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

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  Producer Destructor
 */
 
Producer::~Producer()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

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
