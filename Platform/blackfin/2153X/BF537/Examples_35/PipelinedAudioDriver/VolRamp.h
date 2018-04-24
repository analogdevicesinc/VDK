/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread VolRamp
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _VolRamp_H_    
#define _VolRamp_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class VolRamp : public VDK::Thread 
{
public:
    VolRamp(VDK::Thread::ThreadCreationBlock&);
    virtual ~VolRamp();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _VolRamp_H_ */

/* ========================================================================== */
