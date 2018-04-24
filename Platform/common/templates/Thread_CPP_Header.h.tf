/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _%Name%_H_    
#define _%Name%_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class %Name% : public VDK::Thread 
{
public:
    %Name%(VDK::Thread::ThreadCreationBlock&);
    virtual ~%Name%();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _%Name%_H_ */

/* ========================================================================== */
