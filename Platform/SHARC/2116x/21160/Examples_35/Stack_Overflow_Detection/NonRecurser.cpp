/* =============================================================================
 *
 *   Description: This module implements the 'NonRecurser' thread class. Its 
 *                purpose is to simulate work, which the 'Waiter' thread type
 *                attempts to determine which thread has blown its stack.
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "NonRecurser.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  NonRecurser Run Function (NonRecurser's main{})
 */
 
void
NonRecurser::Run()
{
    while (1)
    {
    	for (volatile int i = 0; i < 10000;)
			i++;
			
		VDK::Sleep(1);
    }
}

/******************************************************************************
 *  NonRecurser Error Handler
 */
 
int
NonRecurser::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  NonRecurser Constructor
 */
 
NonRecurser::NonRecurser(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  NonRecurser Destructor
 */
 
NonRecurser::~NonRecurser()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  NonRecurser Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
NonRecurser::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) NonRecurser(tcb);
}

/* ========================================================================== */
