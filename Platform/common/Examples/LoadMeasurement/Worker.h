/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Worker
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Worker_H_    
#define _Worker_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Worker : public VDK::Thread 
{
public:
    Worker(VDK::Thread::ThreadCreationBlock&);
    virtual ~Worker();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
    void generateWorkParamaters();

private:
	int m_runTime;
	int m_sleepTime;
};

#endif /* _Worker_H_ */

/* ========================================================================== */
