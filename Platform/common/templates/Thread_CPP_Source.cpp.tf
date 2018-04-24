/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "%HeaderFile%"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  %Name% Run Function (%Name%'s main{})
 */
 
void
%Name%::Run()
{
    // TODO - Put the thread's "main" Initialization HERE

    while (1)
    {
        // TODO - Put the thread's "main" body HERE

        // Use a "break" instruction to exit the "while (1)" loop
    }

    // TODO - Put the thread's exit from "main" HERE
	// A thread is automatically Destroyed when it exits its run function
}

/******************************************************************************
 *  %Name% Error Handler
 */
 
int
%Name%::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  %Name% Constructor
 */
 
%Name%::%Name%(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  %Name% Destructor
 */
 
%Name%::~%Name%()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  %Name% Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
%Name%::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) %Name%(tcb);
}

/* ========================================================================== */
