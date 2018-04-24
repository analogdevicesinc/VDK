/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Input
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Input_H_    
#define _Input_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Input : public VDK::Thread 
{
public:
    Input(VDK::Thread::ThreadCreationBlock&);
    virtual ~Input();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);

private:
	VDK::ThreadID m_audioDst;
};

#endif /* _Input_H_ */

/* ========================================================================== */
