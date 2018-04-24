/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Attenuator
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Attenuator_H_    
#define _Attenuator_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Attenuator : public VDK::Thread 
{
public:
    Attenuator(VDK::Thread::ThreadCreationBlock&);
    virtual ~Attenuator();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
    
private:
	VDK::ThreadID m_audioDst;
	unsigned int m_factor;
};

#endif /* _Attenuator_H_ */

/* ========================================================================== */
