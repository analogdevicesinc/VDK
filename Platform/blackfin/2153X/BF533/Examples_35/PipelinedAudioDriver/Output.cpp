/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Output
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Output.h"
#include <new>
#include "AudioMsg.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

static int putBuffer(VDK::DeviceDescriptor dd,
					 VDK::MessageID msg,
					 VDK::Ticks timeout = 0)
{
	int type;
	unsigned int size;
	void *pBuffer = NULL;
	VDK::GetMessagePayload(msg, &type, &size, &pBuffer);
	
	return VDK::SyncWrite(dd, (char*) pBuffer, size, timeout);
}


/******************************************************************************
 *  Output Run Function (Output's main{})
 */
 
void
Output::Run()
{
	VDK::DeviceDescriptor dd = VDK::OpenDevice(kCodec0, "W"); // open for writing

	// Wait until we have two buffers ready to output
	//	
	VDK::MessageID currMsg = VDK::PendMessage(SEND_BUFF_CHANNEL, 0);
	VDK::MessageID nextMsg = VDK::PendMessage(SEND_BUFF_CHANNEL, 0);
	
	// Make sure there is another message waiting in the queue before we start
	//
	VDK::MessageID tmpMsg  = VDK::PendMessage(SEND_BUFF_CHANNEL, 0);
	VDK::PostMessage(VDK::GetThreadID(), tmpMsg, SEND_BUFF_CHANNEL);
	
	// Start outputting the first buffer
	int ret = putBuffer(dd, currMsg); // pipeline start, won't block
	int count = 0;
	
	while (ret != -1)
	{
		// Queue nextMsg for output, this blocks until output
		// of currMsg has completed
		ret = putBuffer(dd, nextMsg);
		
		// currMsg has completed, send it back where it came from.
		// Output is continuing asynchronously on nextMsg.
    	VDK::MsgChannel channel;  // dummy, we don't need this info
    	VDK::ThreadID sender;
    	VDK::GetMessageReceiveInfo(currMsg, &channel, &sender);
    	VDK::PostMessage(sender, currMsg, RETURN_BUFF_CHANNEL);

    	// Get the next message for output
    	currMsg = nextMsg;
		nextMsg = VDK::PendMessage(SEND_BUFF_CHANNEL, 0);
	}		
	

	VDK::CloseDevice(dd);
}

/******************************************************************************
 *  Output Error Handler
 */
 
int
Output::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Output Constructor
 */
 
Output::Output(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
}

/******************************************************************************
 *  Output Destructor
 */
 
Output::~Output()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Output Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Output::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Output(tcb);
}

/* ========================================================================== */
