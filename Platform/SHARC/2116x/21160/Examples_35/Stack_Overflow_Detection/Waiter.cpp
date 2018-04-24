/* =============================================================================
 *
 *   Description: This module implements the 'Waiter' thread class. Its only
 *                purpose is to pend on the 'kOverflow' semaphore, and print
 *                the message "Thread stack overflowed!" whenever the
 *                semaphore is posted.
 *                The Waiter thread needs to run at as high a priority as is
 *                practical, so that it runs as soon as the semaphore is
 *                posted.
 *                After printing its message the Waiter destroys the thread 
 *                whose stack has overflown. It works out which thread this
 *                is by checking where the last thread switch came from in
 *                the history buffer.
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "Waiter.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  Waiter Run Function (Waiter's main{})
 */
 
void
Waiter::Run()
{
	// Wait for a CB7 interrupt to post the semaphore
	VDK::PendSemaphore(kOverflow, 0);
	
	// Now determine which thread's stack has overflown...
	VDK::HistoryEvent historyEvent;
	
	bool foundThreadSwitch = false;
	int historyIndex = VDK::GetCurrentHistoryEventNum();
	
	// Trace back through the history buffer to find the last thread switch.
	while(!foundThreadSwitch && historyIndex != 0)
	{
	    historyIndex--;
	    VDK::GetHistoryEvent(historyIndex, &historyEvent);
		
	    if (historyEvent.type == VDK::kThreadSwitched)
	    	foundThreadSwitch = true;
	}
	
	// We have either broke from this loop because we have found the thread switch
	// or because it was not possible to find it.
	
	// Appropriate action should be taken in each case.
	if (foundThreadSwitch)
	{
		// Report the overflow
		printf("ThreadID:%d stack overflowed!\n",historyEvent.threadID);
		fflush(stdout);

		// Destroy the errant thread.
		VDK::DestroyThread(historyEvent.threadID, true);
	
		printf("ThreadID:%d destroyed.\n",historyEvent.threadID);
	}
	else
		// Report the overflow
		printf("A stack overflow was detected in an unknown thread\n");
	
	// End the application as we have succesfully detected the stack overflow
	exit(0);
}			

/******************************************************************************
 *  Waiter Error Handler
 */
 
int
Waiter::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Waiter Constructor
 */
 
Waiter::Waiter(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Waiter Destructor
 */
 
Waiter::~Waiter()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Waiter Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
Waiter::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Waiter(tcb);
}

