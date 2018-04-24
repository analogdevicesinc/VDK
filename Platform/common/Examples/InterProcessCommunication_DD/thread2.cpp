/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread thread2
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "thread2.h"
#include <stdio.h>
#include <string.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  thread2 Constructor
 */
 
thread2::thread2(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

	/* TODO - Put code to be executed when this thread has just been created HERE */

}

/******************************************************************************
 *  thread2 Destructor
 */
 
thread2::~thread2()
{
	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */

	/* TODO - Put code to be executed just before this thread is destroyed HERE */

}

/******************************************************************************
 *  thread2 Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
thread2::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

    return new (tcb) thread2(tcb);
}

/******************************************************************************
 *  thread2 Run Function (thread2's main{})
 */
 
void
thread2::Run()
{
    // Setup some variables
	char	                string_received[33];
	VDK::DeviceDescriptor	dev_id;

    // Open the IPC channel '1' as a read channel
	dev_id = VDK::OpenDevice(kIPC_Device, "R1");

    // Loop forever
	while (1)
	{
        // Clear off the input string
		memset(string_received, '\0', sizeof(string_received));

        // Read the string from thread1
        // Note that the final byte of the string is '\0'.  Thread1 doesn't
        // send a NULL terminated string.
		VDK::SyncRead(dev_id, string_received, sizeof(string_received) - 1, 0);

        // Note that the string printed is the concatenation of a couple
        // of sends from Thread1 since Thread2 asked for more data then
        // Thread1 sent in one transmission.
		printf("This was the string received: %s\n", string_received);
        fflush(stdout);
	}

	/* TODO - Put the thread's exit from "main" HERE */
}

/******************************************************************************
 *  thread2 Error Handler
 */
 
int
thread2::ErrorHandler()
{
	/* TODO - Put this thread's error handling code HERE */

	/* The default ErrorHandler (called below) kills the thread */
	return (VDK::Thread::ErrorHandler());
}

