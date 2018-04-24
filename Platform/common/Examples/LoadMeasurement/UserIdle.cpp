/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread UserIdle
 *
 * -----------------------------------------------------------------------------
 *  Comments: This thread implements a user written alternative to the VDK idle
 *            thread. This should be the lowest priority thread in the
 *	          application (kPriority30).
 * ===========================================================================*/

#include "UserIdle.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

extern int g_idleWork;

/******************************************************************************
 *  UserIdle Run Function (UserIdle's main{})
 */
 
void
UserIdle::Run()
{
    while (1)
    {
        // Increment idleWork by 1
        // This variable has to be protected at this point to ensure that the 
        // calculation is thread safe
        VDK::PushUnscheduledRegion();
        g_idleWork++;
        VDK::PopUnscheduledRegion();
		
		//VDK::FreeDestroyedThreads();		// Uncomment this function call if
    }										// required (see Note 2 in readme.txt)
}

/******************************************************************************
 *  UserIdle Error Handler
 */
 
int
UserIdle::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  UserIdle Constructor
 */
 
UserIdle::UserIdle(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  UserIdle Destructor
 */
 
UserIdle::~UserIdle()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  UserIdle Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
UserIdle::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) UserIdle(tcb);
}

/* ========================================================================== */
