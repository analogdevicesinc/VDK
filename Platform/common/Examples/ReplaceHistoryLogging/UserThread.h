/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread UserThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _UserThread_H_    
#define _UserThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class UserThread : public VDK::Thread 
{
public:
    UserThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~UserThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _UserThread_H_ */

/* ========================================================================== */
