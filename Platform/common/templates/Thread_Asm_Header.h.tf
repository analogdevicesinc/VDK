/* =============================================================================
 *
 *  Description: This is a C++ to Assembly Thread Header file for Thread %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _%Name%_H_
#define _%Name%_H_

#include "VDK.h"

#ifdef __ECC__	/* for C/C++ access */
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

void %Name%_InitFunction(void**);
void %Name%_DestroyFunction(void**);
int  %Name%_ErrorFunction(void**);
void %Name%_RunFunction(void**);
#ifdef  __cplusplus
} /* end of extern "C" */

#endif /* __cplusplus */
#endif /* __ECC__ */

#ifdef __cplusplus
#include <new>

class %Name%_Wrapper : public VDK::Thread
{
public:
    %Name%_Wrapper(VDK::Thread::ThreadCreationBlock &t)
        : VDK::Thread(t)
    { %Name%_InitFunction(&m_DataPtr); }

    ~%Name%_Wrapper()
    { %Name%_DestroyFunction(&m_DataPtr); }

    int ErrorHandler()
    { return %Name%_ErrorFunction(&m_DataPtr); }

    void Run()
    { %Name%_RunFunction(&m_DataPtr); }

    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock &t)
    { return new (t) %Name%_Wrapper(t); }
};

#endif /* __cplusplus */
#endif /* _%Name%_H_ */

/* ========================================================================== */
