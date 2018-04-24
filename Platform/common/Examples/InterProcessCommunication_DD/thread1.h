/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread thread1
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _thread1_H_    
#define _thread1_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class thread1 : public VDK::Thread 
{
public:
    thread1(VDK::Thread::ThreadCreationBlock&);
    virtual ~thread1();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _thread1_H_ */

/* ========================================================================== */
