/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Consumer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Consumer_H_    
#define _Consumer_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Consumer : public VDK::Thread 
{
public:
    Consumer(VDK::Thread::ThreadCreationBlock&);
    virtual ~Consumer();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Consumer_H_ */

/* ========================================================================== */
