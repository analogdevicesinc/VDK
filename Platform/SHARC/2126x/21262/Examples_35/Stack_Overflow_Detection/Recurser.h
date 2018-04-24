/* =============================================================================
 *
 *   Description: This is a C++ header file for Thread Recurser
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _Recurser_H_    
#define _Recurser_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class Recurser : public VDK::Thread 
{
public:
    Recurser(VDK::Thread::ThreadCreationBlock&);
    virtual ~Recurser();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Recurser_H_ */

