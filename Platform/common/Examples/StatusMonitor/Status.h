/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Status
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Status_H_    
#define _Status_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Status : public VDK::Thread 
{
public:
    Status(VDK::Thread::ThreadCreationBlock&);
    virtual ~Status();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Status_H_ */

/* ========================================================================== */
