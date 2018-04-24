/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread A
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _A_H_    
#define _A_H_

#include "VDK.h"

class A : public VDK::Thread 
{
public:
    A(VDK::Thread::ThreadCreationBlock&);
    virtual ~A();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _A_H_ */

/* ========================================================================== */
