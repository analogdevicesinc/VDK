/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Producer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Producer_H_    
#define _Producer_H_


#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Producer : public VDK::Thread 
{
public:
    Producer(VDK::Thread::ThreadCreationBlock&);
    virtual ~Producer();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Producer_H_ */

/* ========================================================================== */
