/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread UserIdle
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _UserIdle_H_    
#define _UserIdle_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class UserIdle : public VDK::Thread 
{
public:
    UserIdle(VDK::Thread::ThreadCreationBlock&);
    virtual ~UserIdle();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _UserIdle_H_ */

/* ========================================================================== */
