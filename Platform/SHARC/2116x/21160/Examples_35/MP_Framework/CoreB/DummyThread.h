/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread DummyThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _DummyThread_H_    
#define _DummyThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class DummyThread : public VDK::Thread 
{
public:
    DummyThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~DummyThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _DummyThread_H_ */

/* ========================================================================== */
