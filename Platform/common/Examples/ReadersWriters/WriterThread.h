/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread WriterThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _WriterThread_H_    
#define _WriterThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class WriterThread : public VDK::Thread 
{
public:
    WriterThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~WriterThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _WriterThread_H_ */




