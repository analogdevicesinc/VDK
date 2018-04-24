/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Output
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Output_H_    
#define _Output_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Output : public VDK::Thread 
{
public:
    Output(VDK::Thread::ThreadCreationBlock&);
    virtual ~Output();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Output_H_ */

/* ========================================================================== */
