/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Echo
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Echo.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

struct Buffer
{
	int vSamples[96];
};

/******************************************************************************
 *  Echo Run Function (Echo's main{})
 */
 
void
Echo::Run()
{
	VDK::DeviceDescriptor dd = VDK::OpenDevice(kCodec0, "RW");

	// These two packet buffers are used for pipelined (i.e. double-
	// -buffered) input.
	const int numBuffers = 4;
	static Buffer vBuffer[numBuffers];

	// Start up the pipelined input on buffer[0], this call will
	// return immediately (i.e. won't ever block) but input will
	// continue asynchronously on the buffer.
	int inBuffPending = 0;
	int outBuff = 0;
	int ret = VDK::SyncRead(dd, (char*)&vBuffer[inBuffPending], sizeof(Buffer), 0);

	while (-1 != ret)
    {

		// We're in the steady-state of the pipeline now.
		int inBuffCompleted = inBuffPending;
		inBuffPending   = (inBuffPending + 1) % numBuffers;
		
		ret = VDK::SyncRead(dd, (char*)&vBuffer[inBuffPending], sizeof(Buffer), 0);

		if (-1 == ret) break;

		// We have completed input on vBuffer[inBuffCompleted], input is
		// continuing asynchonously on vBuffer[inBuffPending]
		ret = VDK::SyncWrite(dd, (char*)&vBuffer[inBuffCompleted], sizeof(Buffer), 0);
    }

	VDK::CloseDevice(dd);
}

/******************************************************************************
 *  Echo Error Handler
 */
 
int
Echo::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Echo Constructor
 */
 
Echo::Echo(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Echo Destructor
 */
 
Echo::~Echo()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Echo Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Echo::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Echo(tcb);
}

/* ========================================================================== */
