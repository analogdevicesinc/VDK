/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Relay
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <defBF535.h>
#include "Relay.h"
#include <new>
#include "AudioMsg.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define SetIop(addr, val) {(*(volatile short *) addr) = (val); asm volatile("ssync;");}
#define GetIop(addr) (*(volatile short *) addr)

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
	
	if (max > 0x1FFF)
		bits = 0xF;
	else if (max > 0x7FF)
		bits = 0x7;
	else if (max > 0x1FF)
		bits = 0x3;
	else if (max > 0x7F)
		bits = 0x1;
	else
		bits = 0;
	
	SetIop(FIO_FLAG_C, 0xF);  // clear flags
	SetIop(FIO_FLAG_S, bits); // set flags
}


/******************************************************************************
 *  Relay Run Function (Relay's main{})
 */
 
void
Relay::Run()
{
	VDK::ThreadID self = VDK::GetThreadID();
	int count = 0;
	
	// Prime the message return channel with buffer messages.
	for (int i = 0;
		 i < 4;
		 i++)
	{
		void *pBuffer = VDK::MallocBlock(kAudioBuffers);
		VDK::MessageID msg = VDK::CreateMessage(PCM_BUFFER, BUFF_SIZE, pBuffer);
		VDK::PostMessage(self, msg, RETURN_BUFF_CHANNEL);
	}

	while (true)
    {
    	// Wait until we have (at least) one full and one empty message
    	VDK::MessageID msgOut =
    	    VDK::PendMessage(SEND_BUFF_CHANNEL |
    	    				 RETURN_BUFF_CHANNEL |
    	    				 VDK::kMsgWaitForAll,
    	    				 0);
    	// Get the details of the "full" message
     	VDK::MsgChannel channel;
    	VDK::ThreadID sender;
    	VDK::GetMessageReceiveInfo(msgOut, &channel, &sender);

   		// We will have received on the SEND channel first but we know
   		// there's a message on the RETURN channel as well
    	VDK::MessageID msgRet =
    	    VDK::PendMessage(RETURN_BUFF_CHANNEL, 0);
    	
    	// Swap the payloads between the two messages
    	int      typeRet,      typeOut;
    	unsigned sizeRet,      sizeOut;
    	void    *pPayloadRet, *pPayloadOut;
    	
    	VDK::GetMessagePayload(msgOut, &typeOut, &sizeOut, &pPayloadOut);
    	VDK::GetMessagePayload(msgRet, &typeRet, &sizeRet, &pPayloadRet);
    	
    	VDK::SetMessagePayload(msgRet, typeOut, sizeOut, pPayloadOut);
    	VDK::SetMessagePayload(msgOut, typeRet, sizeRet, pPayloadRet);

    	// Snoop the contents of every 100th audio buffer
    	if (PCM_BUFFER == typeOut && 100 == ++count)
    	{
    		processBuffer((short (*)[2])pPayloadOut, sizeOut / 4);
    		count = 0;
    	}
    	
    	// Send the full buffer on to the destination thread
    	VDK::PostMessage(m_audioDst, msgRet, channel);
    	
    	// Send the empty buffer to the sender of the full message
    	VDK::PostMessage(sender, msgOut, RETURN_BUFF_CHANNEL);    	
    }

}

#if 0
void
Relay::Run()
{
	VDK::ThreadID self = VDK::GetThreadID();
	int count = 0;

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
#endif

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
