/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread SysStartThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _SysStartThread_H_    
#define _SysStartThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class SysStartThread : public VDK::Thread 
{
public:
    SysStartThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~SysStartThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _SysStartThread_H_ */


