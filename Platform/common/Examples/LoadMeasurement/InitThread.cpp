/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread InitThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: This is the highest priority thread in the application. It is
 *            first used to calibrate the work done by the User Idle thread when
 *            it is the only active thread in the system.
 *
 *            When this figure has been calculated, InitThread will allow the
 *            Worker threads to begin executing.
 *            
 *            This thread will then wake up regularly at a set period of ticks
 *            to determine how much work has been done by the idle thread.
 *            A breif calculation will then take place, and the application load
 *            will be printed to the screen. The work done by the idle thread 
 *            will then be reset to zero and this process will continue 
 *            indefinitely.
 * ===========================================================================*/

#include "InitThread.h"
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define SAMPLE_PERIOD 5	// The period (in ticks) between each sample reading

volatile int g_idleWork;

/******************************************************************************
 *  InitThread Run Function (InitThread's main{})
 */
 
void
InitThread::Run()
{
    // During the Calibration stage the only active thread in the system is the
    // user defined Idle Thread.
	printf("Start of Calibration stage...\n");

	// Enable the Periodic Semaphore	
	VDK::MakePeriodic(kPeriodic, SAMPLE_PERIOD, SAMPLE_PERIOD);

	// Reset the first three samples and save the last, this will allow the reading
	// to settle before it is saved.
	for(int i = 0; i < 3; i++)
	{
	    g_idleWork = 0;
	    VDK::PendSemaphore(kPeriodic,0);
	}
	
	// Save this sample as the work done by the idle thread when it has 100% 
	// execution time
	int idealLoad = g_idleWork;
    
	printf("...End of Calibration stage.\n");
	printf("Ideal Load: %d iterations per period\n",idealLoad);
	
	// Make the User Idle Thread the lowest priority thread in the system
	// This will allow the Worker threads to begin execution.
	VDK::SetPriority(kUserIdleThread, VDK::kPriority30);
	
	// From this point onward, this thread will sample the work done by the idle 
	// thread at regular intervals and use this figure in conjunction with the 
	// ideal load obtained earlier to calculate the current application load and
	// print it to the screen.
	while(1)
	{
	    g_idleWork = 0;
		VDK::PendSemaphore(kPeriodic,0);
		
		int appLoad = (100 - ((100 * g_idleWork)/idealLoad));
		printf("Application Load: %d%%\n", appLoad);
	}
}

/******************************************************************************
 *  InitThread Error Handler
 */
 
int
InitThread::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  InitThread Constructor
 */
 
InitThread::InitThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  InitThread Destructor
 */
 
InitThread::~InitThread()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  InitThread Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
InitThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) InitThread(tcb);
}

/* ========================================================================== */
