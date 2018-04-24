/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread RampGen
 *
 * -----------------------------------------------------------------------------
 *  Comments: The RampGen thread generates a repeating ramp (sawtooth) signal
 *            with a period set from a global variable at creation time.
 *
 * ===========================================================================*/

#include "RampGen.h"
#include "ThreadData.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/******************************************************************************
 *  RampGen Constructor
 */
 
RampGen::RampGen(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

	threadData threadParamaters = *(threadData*) tcb.user_data_ptr;
     
     // Initialize the values for this instance of the thread type...
	m_MySemaphore = threadParamaters.semaphoreToPendOn;
	m_MyPeriod = threadParamaters.periodOfGenerator;
	m_MyAmplitudeStep = threadParamaters.amplitudeStep;
}

/******************************************************************************
 *  RampGen Destructor
 */
 
RampGen::~RampGen()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    /* Put code to be executed just before this thread is destroyed HERE */
    /* Nothing to do here... */

}

/******************************************************************************
 *  RampGen Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
RampGen::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    /* Default code -- nothing special to do here... */

    return new (tcb) RampGen(tcb);
}

/******************************************************************************
 *  RampGen Run Function (RampGen's main{})
 */
 
void
RampGen::Run()
{
	// Wait to be told when to calculate values.	
	VDK::PendSemaphore( m_MySemaphore, 0 );
	
	// After all of the RampGen threads are created, each falls into an 
	// infinite loop.  In the loop, each fills a buffer with a signal and
	// then pends on a semaphore.  Note that the same code is used for
	// each generator thread even though they each have distinct signal
	// frequencies and signaling semaphores.
	m_LastValue = 0;
	while(1)
	{
		// Calculate one buffer's worth of data... 
		for( int i = 0; i < BUFFER_SIZE; ++i )
 	    {
			m_MyBuffer[i] = (m_LastValue + (i * m_MyAmplitudeStep) ) % MAX_VALUE;
		}
		// ...and then wait to be told when to calculate more.  The second 
		// parameter indicates that the thread should pend until it can
		// be scheduled normally rather than "time out" if it cannot get processor
		// time with a specified number of ticks.
		VDK::PendSemaphore( m_MySemaphore, 0 );
    }
}

/******************************************************************************
 *  RampGen Error Handler
 */
 
int
RampGen::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* Default code -- nothing special to do here... */

    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}


