/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread B
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _B_H_    
#define _B_H_

#include "VDK.h"

class B : public VDK::Thread 
{
public:
    B(VDK::Thread::ThreadCreationBlock&);
    virtual ~B();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _B_H_ */

/* ========================================================================== */
