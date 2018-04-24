/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread InitThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _InitThread_H_    
#define _InitThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class InitThread : public VDK::Thread 
{
public:
    InitThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~InitThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _InitThread_H_ */

/* ========================================================================== */
