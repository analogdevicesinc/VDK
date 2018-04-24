/* =============================================================================
 *
 *   Description: This module implements the 'Recurser' thread class. Its only
 *                purpose is to overflow its own stack and hence invoke the
 *                CB7 interrupt.		  
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "Recurser.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/*
 * If compiler optimization is enabled then this function can be optimized away.
 * We need to keep the function in order to overflow the stack
 */
#pragma optimize_off
static void recurse (volatile int num)
{
	if (num > 0 )
		recurse (num -1);
}

/******************************************************************************
 *  Recurser Run Function (Recurser's main{})
 *
 * Its only purpose is to overflow its own stack by calling the function
 * recurse().
 *
 * The size of Recurser's stack is 255 words (the default) so 1000-deep
 * recursion is more than sufficient to overrun the stack.
 *
 * It is worth noting that the circular buffering hardware will cause
 * the stack to wrap around to its top again whenever it overflows the base,
 * so erroneous behaviour may only be observed after several levels of
 * function return, when the stack wraps back to its corrupted base.
 */
 
void
Recurser::Run()
{
    while (1)
    {
		recurse (1000);
    }
}

/******************************************************************************
 *  Recurser Error Handler
 */
 
int
Recurser::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Recurser Constructor
 */
 
Recurser::Recurser(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Recurser Destructor
 */
 
Recurser::~Recurser()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Recurser Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
Recurser::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Recurser(tcb);
}

