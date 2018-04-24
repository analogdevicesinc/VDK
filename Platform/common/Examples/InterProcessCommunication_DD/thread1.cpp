/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread thread1
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "thread1.h"
#include <string.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  thread1 Constructor
 */
 
thread1::thread1(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

	/* TODO - Put code to be executed when this thread has just been created HERE */

}

/******************************************************************************
 *  thread1 Destructor
 */
 
thread1::~thread1()
{
	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */

	/* TODO - Put code to be executed just before this thread is destroyed HERE */

}

/******************************************************************************
 *  thread1 Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
thread1::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

    return new (tcb) thread1(tcb);
}

/******************************************************************************
 *  thread1 Run Function (thread1's main{})
 */
 
void
thread1::Run()
{
    // Setup some variables we're going to need
	char                    string_to_send[] = "Hello World";
	VDK::DeviceDescriptor	dev_id;

    // Open the IPC device as a write channel from this thread, and name the
    // channel '1'
	dev_id = VDK::OpenDevice(kIPC_Device, "W1");

    // Give thread2 a chance to run (and open IPC '1' as a read channel)
	VDK::Yield();

    // Keep looping, and outputing the string
	while (1)
        // Note that we're NOT sending the NULL termination at the end of
        // the string
		VDK::SyncWrite(dev_id, string_to_send, strlen(string_to_send), 0);
}

/******************************************************************************
 *  thread1 Error Handler
 */
 
int
thread1::ErrorHandler()
{
	/* TODO - Put this thread's error handling code HERE */

	/* The default ErrorHandler (called below) kills the thread */
	return (VDK::Thread::ErrorHandler());
}

