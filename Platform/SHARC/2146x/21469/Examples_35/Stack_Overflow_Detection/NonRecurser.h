/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread NonRecurser
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _NonRecurser_H_    
#define _NonRecurser_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class NonRecurser : public VDK::Thread 
{
public:
    NonRecurser(VDK::Thread::ThreadCreationBlock&);
    virtual ~NonRecurser();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _NonRecurser_H_ */

/* ========================================================================== */
