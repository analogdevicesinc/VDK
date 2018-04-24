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

#include "VDK.h"

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

