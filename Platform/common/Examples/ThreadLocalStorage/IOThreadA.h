/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread IOThreadA
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _IOThreadA_H_    
#define _IOThreadA_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

extern void ReadWrite(int);

class IOThreadA : public VDK::Thread 
{
public:
    IOThreadA(VDK::Thread::ThreadCreationBlock&);
    virtual ~IOThreadA();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
protected:
	int m_Initialiser;
};

#endif /* IOThreadA */

/* ========================================================================== */
