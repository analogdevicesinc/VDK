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

#include "VDK.h"

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
