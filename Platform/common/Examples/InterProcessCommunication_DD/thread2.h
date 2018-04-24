/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread thread2
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _thread2_H_    
#define _thread2_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class thread2 : public VDK::Thread 
{
public:
    thread2(VDK::Thread::ThreadCreationBlock&);
    virtual ~thread2();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _thread2_H_ */

/* ========================================================================== */
