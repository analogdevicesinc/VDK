/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread IOThreadB
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _IOThreadB_H_    
#define _IOThreadB_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

extern void ReadWrite(int);

class IOThreadB : public VDK::Thread 
{
public:
    IOThreadB(VDK::Thread::ThreadCreationBlock&);
    virtual ~IOThreadB();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
protected:
	int m_Initialiser;
};

#endif /* _IOThreadB_H_ */

/* ========================================================================== */
