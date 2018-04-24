/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread RampGen
 *
 * -----------------------------------------------------------------------------
 *  Comments: The RampGen thread generates a repeating ramp (sawtooth) signal
 *            with a period set from a global variable at creation time.
 *
 * ===========================================================================*/

#ifndef _RampGen_H_    
#define _RampGen_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

/****************************************************
 * Global variables for passing info between threads
 *   (These are defined in Factory.cpp)
 */
extern	SemaphoreID		g_SemaphoreToPendOn;
extern	unsigned int	g_PeriodOfGenerator;
extern	unsigned int	g_AmplitudeStep;

// The size of the buffer that the function generator fills
#define BUFFER_SIZE 256

// The peak amplitude of the function (min value == 0)
#define MAX_VALUE 47730

class RampGen : public VDK::Thread 
{
public:
    RampGen(VDK::Thread::ThreadCreationBlock&);
    virtual ~RampGen();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);



// These following are member variables (i.e. m_) of class RampGen that are
// private to RampGen functions.  No other class has direct access to this data
// and each thread that is an instantiation of the RampGen class will have a
// distinct local set of these members.

private:
	unsigned int	m_MyBuffer[BUFFER_SIZE];
	SemaphoreID		m_MySemaphore;
	unsigned int	m_MyPeriod;
	unsigned int	m_MyAmplitudeStep;
	unsigned int	m_LastValue;
};

#endif /* _RampGen_H_ */


