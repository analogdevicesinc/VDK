/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Relay
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Relay_H_    
#define _Relay_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Relay : public VDK::Thread 
{
public:
    Relay(VDK::Thread::ThreadCreationBlock&);
    virtual ~Relay();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);

private:
	VDK::ThreadID m_audioDst;
};

#endif /* _Relay_H_ */

/* ========================================================================== */
