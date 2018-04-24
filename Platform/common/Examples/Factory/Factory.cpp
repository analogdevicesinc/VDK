/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Factory
 *
 * -----------------------------------------------------------------------------
 *  Comments: The factory thread creates three ramp function generators and
 *            then destroys itself.
 *
 * ===========================================================================*/

#include "Factory.h"
#include "ThreadData.h"
#include <new>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

/****************************************************
 * Global variables for passing info bettween threads
 */
SemaphoreID		g_SemaphoreToPendOn;
unsigned int	g_PeriodOfGenerator;
unsigned int	g_AmplitudeStep;


/******************************************************************************
 *  Factory Constructor
 */
 
Factory::Factory(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    /* Put code to be executed when this thread has just been created HERE */
    /* Nothing to do here... */

}

/******************************************************************************
 *  Factory Destructor
 */
 
Factory::~Factory()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    /* Put code to be executed just before this thread is destroyed HERE */
    /* Nothing to do here... */

}

/******************************************************************************
 *  Factory Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
Factory::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    /* Default code -- nothing special to do here... */

    return new (tcb) Factory(tcb);
}

/******************************************************************************
 *  Factory Run Function (Factory's main{})
 */
 
void
Factory::Run()
{
	// In the following lines, each of the three RampGen threads are created &
	// initialized.  The "parameters" are passed to the new threads using a 
	// thread creation block along with the CreateThreadEx() API.

	// Declare a data structure to store the thread paramaters
	threadData threadParamaters;
	
	// Set up a Thread Creation Block (TCB)...
	VDK::ThreadCreationBlock tcb_kRampGen;
	
	// The following information is set up in the TCB...
	tcb_kRampGen.template_id = kRampGen;				// Thread Template Name
	tcb_kRampGen.thread_stack_size = 255;				// Thread Stack Size
	tcb_kRampGen.thread_priority = VDK::kPriority10;	// Thread Priority
	tcb_kRampGen.user_data_ptr = &threadParamaters;		// User Data Pointer
	
	// Set the thread paramaters of the first RampGen thread...
	threadParamaters.semaphoreToPendOn = kRepeatEvery3;
	threadParamaters.periodOfGenerator = 3;
	threadParamaters.amplitudeStep = 1591;
	
	// CreateThreadEx will create the RampGen thread, the paramaters passed 
	// in will be assigned to local variables in the RampGen constructor.
	VDK::CreateThreadEx(&tcb_kRampGen);	

	// Set the thread paramaters of the second RampGen thread...
	threadParamaters.semaphoreToPendOn = kRepeatEvery5;
	threadParamaters.periodOfGenerator = 5;
	threadParamaters.amplitudeStep = 1290;
	
	// CreateThreadEx will create the RampGen thread, the paramaters passed 
	// in will be assigned to local variables in the RampGen constructor.
	VDK::CreateThreadEx(&tcb_kRampGen);

	// Set the thread paramaters of the third RampGen thread...
	threadParamaters.semaphoreToPendOn = kRepeatEvery7;
	threadParamaters.periodOfGenerator = 7;
	threadParamaters.amplitudeStep = 1110;

	// CreateThreadEx will create the RampGen thread, the paramaters passed 
	// in will be assigned to local variables in the RampGen constructor.
	VDK::CreateThreadEx(&tcb_kRampGen);

	// The factory thread has now completely initialized all the RampGen
	// threads, so it can destroy itself and free its system resources.

	// The second parameter to DestroyThread ("true") indicates that
	// memory and other resources associated with the factory thread should 
	// be freed immediately rather than being put into a queue to be
	// cleaned up during idle thread cycles.	
	VDK::ThreadID thisThread = VDK::GetThreadID();
	
	// The temporary variable "thisThread" will be automatically removed
	// the compiler in an optimized build and is introduced for clarity only.
	VDK::DestroyThread( thisThread, true );
}

/******************************************************************************
 *  Factory Error Handler
 */
 
int
Factory::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* Default code -- nothing special to do here... */

    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}


