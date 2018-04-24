/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Echo
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Echo_H_    
#define _Echo_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Echo : public VDK::Thread 
{
public:
    Echo(VDK::Thread::ThreadCreationBlock&);
    virtual ~Echo();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Echo_H_ */

/* ========================================================================== */
