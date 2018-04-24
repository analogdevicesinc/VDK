/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Attenuator
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Attenuator.h"
#include <new>
#include <math.h>
#include "AudioMsg.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

extern "C" void AttenuateBuffer16to32(short src[], int dst[], int n, unsigned vol);


/******************************************************************************
 *  Attenuator Run Function (Attenuator's main{})
 */
 
void
Attenuator::Run()
{
	VDK::ThreadID self = VDK::GetThreadID();

	// Prime the message return channel with buffer messages.
	for (int i = 0;
		 i < 4;
		 i++)
	{
		void *pBuffer = new unsigned long[2*SAMPLEPAIRS_PER_BUFFER];;
		VDK::MessageID msg = VDK::CreateMessage(PCM_BUFFER_24, SAMPLEPAIRS_PER_BUFFER, pBuffer);
		VDK::PostMessage(self, msg, RETURN_BUFF_CHANNEL);
	}

	while (true)
    {
    	// Get the next message
    	VDK::MessageID msg = VDK::PendMessage(SEND_BUFF_CHANNEL | CONTROL_CHANNEL, 0);

    	// Get the details of the message
     	VDK::MsgChannel channel;
    	VDK::ThreadID sender;
    	VDK::GetMessageReceiveInfo(msg, &channel, &sender);

		// Get the payload
		int      type;
		unsigned size;
		void    *pPayload;
		VDK::GetMessagePayload(msg, &type, &size, &pPayload);

		// Process the different message types
		//
    	switch (type)
    	{
    	case PCM_BUFFER_16: // it's a 16-bit audio buffer
    		{
    			// Get a 24-bit audio message from the return channel
    			VDK::MessageID msg24 = VDK::PendMessage(RETURN_BUFF_CHANNEL, 0);

    			// Get the (empty) 24-bit payload buffer
				int      type24;
				unsigned size24;
				void    *pPayload24;
				VDK::GetMessagePayload(msg24, &type24, &size24, &pPayload24);

				// Call the assembly routine to fill the 24-bit buffer
				// with an attenuated version of the contents of the
				// 16-bit buffer.
				//
				AttenuateBuffer16to32((short*)pPayload,
									  (int*)pPayload24,
									  size,
									  m_factor);

				// Send the 24-bit attenuated buffer to the audio output
				VDK::PostMessage(m_audioDst, msg24, SEND_BUFF_CHANNEL);
				
				// Send the 16-bit input buffer back to its source
				VDK::PostMessage(sender,     msg, RETURN_BUFF_CHANNEL);
    		}
 
    		break;
    		
    	case PCM_BUFFER_24: // it's a 24-bit audio buffer
    		// Send it on to the output unchanged
    		VDK::ForwardMessage(m_audioDst, msg, channel, sender);
    		break;
    		
    	case VOL_CHANGE: // it's a volume change control message
    		{
    			if (1 == size)
    			{
    				// Calculate the new volume factor from the
    				// "Db of attenuation" payload parameter.
    				//
    				float *pDecibels = (float*)pPayload;
	    			float divisor = powf(2, pDecibels[0]/6.0);
	    			int scaledDivisor = 0x00FF & (int)(0x0080 / divisor);
	    			m_factor = scaledDivisor | (scaledDivisor << 16);
    			}
    			
    			// Return the message to sender
				VDK::PostMessage(sender, msg, RETURN_BUFF_CHANNEL);
	    		break;
	   		}
    	}
    }
}

/******************************************************************************
 *  Attenuator Error Handler
 */
 
int
Attenuator::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Attenuator Constructor
 */
 
Attenuator::Attenuator(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
	m_factor = 0x00800080;
	m_audioDst = kWriter;
}

/******************************************************************************
 *  Attenuator Destructor
 */
 
Attenuator::~Attenuator()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  Attenuator Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Attenuator::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) Attenuator(tcb);
}

/* ========================================================================== */
