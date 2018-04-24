/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread A
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "A.h"
#include <new>
#include <iostream.h>

/******************************************************************************
 *  A Run Function (A's main{})
 */
 
void
A::Run()
{
    // TODO - Put the thread's "main" Initialization HERE
	int times_run=0;
    while (1)
    {
      //  printf("In thread A %d\n",times_run++ );
      cout<< "In thread A" << times_run++ << endl;
                if (times_run == 10)
                        break;
                else
                VDK::Yield();
    }
    //printf("Thread A finished run\n"); 
    cout << "Thread A finished run" << endl;


    // TODO - Put the thread's exit from "main" HERE
	// A thread is automatically Destroyed when it exits its run function
}

/******************************************************************************
 *  A Error Handler
 */
 
int
A::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  A Constructor
 */
 
A::A(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  A Destructor
 */
 
A::~A()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  A Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
A::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) A(tcb);
}

/* ========================================================================== */
