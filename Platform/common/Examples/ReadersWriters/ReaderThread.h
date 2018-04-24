/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread ReaderThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _ReaderThread_H_    
#define _ReaderThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

/* Definitions used by both reader and writer */

extern bool g_EndFlag; /* defined in SysStartThread.h */

class ReaderThread : public VDK::Thread 
{
public:
    ReaderThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~ReaderThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _ReaderThread_H_ */


