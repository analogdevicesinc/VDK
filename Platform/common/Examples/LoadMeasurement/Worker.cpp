/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Worker
 *
 * -----------------------------------------------------------------------------
 *  Comments: This thread is intended to simulate the behaviour of a thread that
 *            processes bursts of data as they arrive, where the data packets
 *            are of varying length and arrive at random intervals. The 
 *            intention is to ensure that the application is under a constantly 
 *            varying work load as this will best demonstrate the load 
 *            measurement routine.
 * ===========================================================================*/

#include "Worker.h"
#include <new>
#include <stdlib.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  Generate Work Paramaters Function 
 */
 
void Worker::generateWorkParamaters()
{
    // Generate the amount of iterations it will take to execute one unit of work
	m_runTime = rand() % 5000;
	
	// Generate the time (in ticks) before the next unit of work will begin
	m_sleepTime = (rand() % 11) + 1;   
}

/******************************************************************************
 *  Worker Run Function (Worker's main{})
 */
 
void
Worker::Run()
{    
    while (1)
    {
        generateWorkParamaters();
		
        // Execute one unit of work ()
		for (volatile int i = 0; i < m_runTime;)
			i++;	// i is declared as volatile so it will not be removed
					// due to compiler optimisation
					
		// Wait until the next unit of work begins
		VDK::Sleep(m_sleepTime);
	}
}

/******************************************************************************
 *  Worker Error Handler
 */
 
int
Worker::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Worker Constructor
 */
 
Worker::Worker(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  Worker Destructor
 */
 
Worker::~Worker()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  Worker Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Worker::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) Worker(tcb);
}

/* ========================================================================== */
