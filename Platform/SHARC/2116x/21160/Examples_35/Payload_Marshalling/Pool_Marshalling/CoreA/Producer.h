/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread Producer
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _Producer_H_    
#define _Producer_H_

#include "VDK.h"

class Producer : public VDK::Thread 
{
public:
    Producer(VDK::Thread::ThreadCreationBlock&);
    virtual ~Producer();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _Producer_H_ */

/* ========================================================================== */
