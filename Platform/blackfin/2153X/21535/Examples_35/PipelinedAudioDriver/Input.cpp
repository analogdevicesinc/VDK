/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Input
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Input.h"
#include <new>
#include "AudioMsg.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

static int fillBuffer(VDK::DeviceDescriptor dd,
					  VDK::MessageID msg,
					  VDK::Ticks timeout = 0)
{
	int type;
	unsigned int size;
	void *pBuffer = NULL;
	VDK::GetMessagePayload(msg, &type, &size, &pBuffer);
	
	return VDK::SyncRead(dd, (char*) pBuffer, size, timeout);
}

	
/******************************************************************************
 *  Input Run Function (Input's main{})
 */
 
void
Input::Run()
{
	VDK::DeviceDescriptor dd = VDK::OpenDevice(kCodec0, "R"); // open for reading
	VDK::ThreadID self = VDK::GetThreadID();
	
	// Prime the message return channel with buffer messages.
	for (int i = 0;
		 i < 4;
		 i++)
	{
		void *pBuffer = VDK::MallocBlock(kAudioBuffers);
		VDK::MessageID msg = VDK::CreateMessage(PCM_BUFFER, BUFF_SIZE, pBuffer);
		VDK::PostMessage(self, msg, RETURN_BUFF_CHANNEL);
	}

	// Get the first buffer message
	VDK::MessageID currMsg = PendMessage(RETURN_BUFF_CHANNEL, 0);
	
	// Start up the pipelined input on currMsg, this call will
	// return immediately (i.e. won't ever block) but input will
	// continue asynchronously on the buffer.
	int ret = fillBuffer(dd, currMsg); // pipeline start, returns immediately

	while (-1 != ret)
    {
		// We're in the steady-state of the pipeline now.
		// Get the next empty message
		VDK::MessageID nextMsg = PendMessage(RETURN_BUFF_CHANNEL, 0);
		
		// and fill it. This blocks until input completes on currMsg.
		ret = fillBuffer(dd, nextMsg);

		if (-1 == ret) break;

		// We have completed input on currMsg, now we send it to the
		// destination thread. Input is continuing asynchronously on nextMsg.
		VDK::PostMessage(m_audioDst, currMsg, SEND_BUFF_CHANNEL);
		currMsg = nextMsg;
    }

	VDK::CloseDevice(dd);
}

/******************************************************************************
 *  Input Error Handler
 */
 
int
Input::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Input Constructor
 */
 
Input::Input(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

	m_audioDst = kMonitor;
}

/******************************************************************************
 *  Input Destructor
 */
 
Input::~Input()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Input Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Input::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Input(tcb);
}

/* ========================================================================== */
