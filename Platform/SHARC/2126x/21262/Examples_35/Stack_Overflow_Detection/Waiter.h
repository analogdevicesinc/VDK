/* =============================================================================
 *
 *   Description: This is a C++ header file for Thread Waiter
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _Waiter_H_    
#define _Waiter_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Waiter : public VDK::Thread 
{
public:
    Waiter(VDK::Thread::ThreadCreationBlock&);
    virtual ~Waiter();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Waiter_H_ */

