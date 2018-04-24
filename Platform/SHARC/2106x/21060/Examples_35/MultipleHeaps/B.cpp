/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread B
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "B.h"
#include <new>
#include <iostream.h>

/******************************************************************************
 *  B Run Function (B's main{})
 */
 
void
B::Run()
{
	int times_run=0;

    while (1)
    {
    		cout << "In thread B" << times_run++ <<endl;
             //printf("In thread B %d\n",times_run++ );
                if (times_run == 10)
                        break;
                else
                        VDK::Yield();
    }

    cout << "Thread B finished run" << endl;
    cout << "Done" << endl;
    //printf("Thread B finished run\n");     
    //printf( "Done\n");    

    // TODO - Put the thread's exit from "main" HERE
	// A thread is automatically Destroyed when it exits its run function
}

/******************************************************************************
 *  B Error Handler
 */
 
int
B::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  B Constructor
 */
 
B::B(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  B Destructor
 */
 
B::~B()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  B Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
B::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) B(tcb);
}

/* ========================================================================== */
