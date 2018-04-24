/* =============================================================================
 *
 *   Description: This module implements the 'Waiter' thread class. Its only
 *                purpose is to pend on the 'kOverflow' semaphore, and print
 *                the message "Thread stack overflowed!" whenever the
 *                semaphore is posted.
 *                The Waiter thread needs to run at as high a priority as is
 *                practical, so that it runs as soon as the semaphore is
 *                posted.
 *                After printing its message the Waiter destroys the
 *                Recurser thread, on the assumption (correct in this case)
 *                that it is the failing thread. However, this approach is
 *                not applicable to the general case, as there is (at present)
 *                no way for the code to determine which thread was running at
 *                the time of the overflow interrupt. Instead, during 
 *                development (i.e. with debugging and instrumentation) it is 
 *                better to set a breakpoint on the puts() line, or immediately
 *                after it. If/when the program stops at this breakpoint, the 
 *                VDK History window will show which thread was active at the 
 *                point where the Waiter's status changed from SemaphoreBlocked
 *                to Ready (or Running). This Thread (i.e. the thread which 
 *                caused the overflow interrupt) will show a SemaphorePosted 
 *                event arrow at the point where the interrupt occurred.
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "Waiter.h"
#include <new>
#include <stdio.h>

/******************************************************************************
 *  Waiter Run Function (Waiter's main{})
 */
 
void
Waiter::Run()
{
	// Create the Recurser thread
	VDK::ThreadID recurser = VDK::CreateThread(kRecurser);

	// Wait for a CB7 interrupt to post the semaphore
	VDK::PendSemaphore(kOverflow, 0);

	// Report the overflow
	puts("Thread stack overflowed!\n");
	fflush(stdout);

	// Destroy the errant thread, since we know which it
	// is. In the general case we wouldn't know this.
	// (Set breakpoint here, as described in readme.txt)
	VDK::DestroyThread(recurser, true);
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

