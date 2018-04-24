/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread UserThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "UserThread.h"
#include <new>
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


/******************************************************************************
 *  UserThread Run Function (UserThread's main{})
 */
 
void
UserThread::Run()
{
    for (int i = 0; i < 10; i++)
    {
    	printf("Thread %d run %d times\n",VDK::GetThreadID(),i);
		VDK::Yield();
    }
    printf("Thread %d finished\n",VDK::GetThreadID());
    VDK::PendSemaphore(kExampleFinished,0);
    
}

/******************************************************************************
 *  UserThread Error Handler
 */
 
int
UserThread::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  UserThread Constructor
 */
 
UserThread::UserThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  UserThread Destructor
 */
 
UserThread::~UserThread()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  UserThread Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
UserThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

   	return new (tcb) UserThread(tcb);
}

/* ========================================================================== */
