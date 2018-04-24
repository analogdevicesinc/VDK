/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread VolRamp
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "VolRamp.h"
#include <new>
#include <cdefbf537.h>
#include "AudioMsg.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define DIM(x) (sizeof(x)/sizeof(x[0]))

/******************************************************************************
 *  VolRamp Run Function (VolRamp's main{})
 */
 
void
VolRamp::Run()
{
    float volume = 0.0;
    VDK::MessageID msg =
        VDK::CreateMessage(VOL_CHANGE, volume, NULL);
    
    // Ramp down the volume over time.
    while (1)
    {
    	// The periodic semaphore controls the polling rate
        VDK::PendSemaphore(kVolButtonPoll, 0);
        
        float newVolume = volume;
        
        // Test the pushbuttons
        //
        if (*pPORTFIO & 0x4) // pushbutton 1 is down
        	newVolume += 1.0;     // then increase the attenuation
        
        if (*pPORTFIO & 0x8) // pushbutton 2 is down
        	newVolume -= 1.0;     // then decrease the attenuation
        
        // Clamp the volume to the range 0.0 - 48.0
        //
        if (newVolume > 48.0)
        	newVolume = 48.0;
        	
        if (newVolume < 0.0)
        	newVolume = 0.0;
        
        // If the volume has changed, send a control message
        // to the attenuator thread.
        //	
        if (newVolume != volume)
        {
        	volume = newVolume;
        	
	        // Do an RPC-style message roundtrip to set the volume.
	        // Because we block in PendMessage until the message
	        // is returned to us if is safe to use an automatic
	        // variable as the payload.
	        float arg[] = { volume };
	        
        	VDK::SetMessagePayload(msg, VOL_CHANGE, DIM(arg), arg);
	        VDK::PostMessage(kVolControl, msg, CONTROL_CHANNEL);
	        msg = VDK::PendMessage(RETURN_BUFF_CHANNEL, 0);
        }
    }
}

/******************************************************************************
 *  VolRamp Error Handler
 */
 
int
VolRamp::ErrorHandler()
{
    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  VolRamp Constructor
 */
 
VolRamp::VolRamp(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

}

/******************************************************************************
 *  VolRamp Destructor
 */
 
VolRamp::~VolRamp()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */
}

/******************************************************************************
 *  VolRamp Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
VolRamp::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) VolRamp(tcb);
}

/* ========================================================================== */
