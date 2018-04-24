/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread IOThreadA
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "IOThreadA.h"
#include <new>

/******************************************************************************
 *  IOThreadA Run Function (IOThread's main{})
 */
int ddSlot = VDK::kTLSUnallocated;
 
// Setup a look up table containing the different Boot Device IDs 
// 
static const VDK::IOID deviceIOData[2] =
{
    kBootDevA,
    kBootDevB
};

void
IOThreadA::Run()
{
    // Assign the correct Device ID as specified by the thread initialiser
    VDK::IOID deviceID = deviceIOData[m_Initialiser];
    
	// Obtain a Device Descriptor by opening the device, the descriptor 
	// returned here will only be valid for this instance of IOThreadA
	VDK::DeviceDescriptor dd = VDK::OpenDevice(deviceID,0);
	
	// Tag the Descriptor onto the TLS slot
	VDK::SetThreadSlotValue(ddSlot, (void*) dd);

	while(1)
	{
	    // Call a global function that will make use of the Device Driver
		ReadWrite(ddSlot);
	}
}

/******************************************************************************
 *  IOThread Error Handler
 */
 
int
IOThreadA::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  IOThreadA Constructor
 */
 
IOThreadA::IOThreadA(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
     
    // Obtain the thread Initialiser value
    m_Initialiser = *(int*) tcb.user_data_ptr;
         
    // Allocate a Thread Slot (only if it has not all ready been allocated)
	if (ddSlot == VDK::kTLSUnallocated)
		VDK::AllocateThreadSlot(&ddSlot);
}

/******************************************************************************
 *  IOThreadA Destructor
 */
 
IOThreadA::~IOThreadA()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  IOThreadA Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
IOThreadA::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */
    
     return new (tcb) IOThreadA(tcb);
}

/* ========================================================================== */
