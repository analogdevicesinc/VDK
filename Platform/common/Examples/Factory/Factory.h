/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Factory
 *
 * -----------------------------------------------------------------------------
 *  Comments: The factory thread creates three ramp function generators and
 *            then destroys itself.
 *
 * ===========================================================================*/

#ifndef _Factory_H_    
#define _Factory_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Factory : public VDK::Thread 
{
public:
    Factory(VDK::Thread::ThreadCreationBlock&);
    virtual ~Factory();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Factory_H_ */


