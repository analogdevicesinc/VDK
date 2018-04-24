/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Relay
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <defBF533.h>
#include "Relay.h"
#include <new>
#include "AudioMsg.h"
#include "Talkthrough.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

// Function to examine the contents of a buffer of 16-bit stereo audio samples
// and set the LED flags (roughly) according to the current peak level.
//
static void processBuffer(short vvSamples[][2], unsigned size)
{
	long max = 0;
	for (unsigned i = 0;
		 i < size;
		 ++i)
	{
		int left  = __builtin_abs(vvSamples[i][0]);
		int right = __builtin_abs(vvSamples[i][1]);
		
		int average = left + right;
		
		max = __builtin_max(max, average);
	}

	// Set up the flags bit pattern according to the peak value	
	short bits;
	
	if (max > 0x7FFF)
		bits = 0x3F;
	else if (max > 0x3FFF)
		bits = 0x1F;
	else if (max > 0x1FFF)
		bits = 0xF;
	else if (max > 0xFFF)
		bits = 0x7;
	else if (max > 0x7FF)
		bits = 0x3;
	else if (max > 0x3FF)
		bits = 0x1;
	else
		bits = 0;
	
	*pFlashA_PortB_Data = bits; // set flags
}


/******************************************************************************
 *  Relay Run Function (Relay's main{})
 */

void
Relay::Run()
{
	VDK::ThreadID self = VDK::GetThreadID();
	int count = 0;

	*pFlashA_PortB_Dir = 0x3f;
	*pFlashA_PortB_Data = 0;    // turn off the LEDs

	while (true)
    {
    	// Get the next message
    	VDK::MessageID msg = VDK::PendMessage(SEND_BUFF_CHANNEL, 0);

    	// Get the details of the message
     	VDK::MsgChannel channel;
    	VDK::ThreadID sender;
    	VDK::GetMessageReceiveInfo(msg, &channel, &sender);

		// Get the payload
		int      typeOut;
		unsigned sizeOut;
		void    *pPayloadOut;
		VDK::GetMessagePayload(msg, &typeOut, &sizeOut, &pPayloadOut);
    	
    	// Snoop the contents of every 100th audio buffer
    	if (PCM_BUFFER == typeOut && 100 == ++count)
    	{
    		processBuffer((short (*)[2])pPayloadOut, sizeOut / 4);
    		count = 0;
    	}
 
// Send the message on to the destination thread
		VDK::ForwardMessage(m_audioDst, msg, channel, sender);

    }

}

/******************************************************************************
 *  Relay Error Handler
 */
 
int
Relay::ErrorHandler()
{
    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Relay Constructor
 */
 
Relay::Relay(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

	m_audioDst = kWriter;
}

/******************************************************************************
 *  Relay Destructor
 */
 
Relay::~Relay()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  Relay Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Relay::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (tcb) Relay(tcb);
}

/* ========================================================================== */
