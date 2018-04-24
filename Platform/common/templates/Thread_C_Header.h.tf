/* =============================================================================
 *
 *  Description: This is a C++ to C Thread Header file for Thread %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _%Name%_H_
#define _%Name%_H_

#ifndef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)
#endif

#include "VDK.h"
#include <stdint.h>

#ifndef _MISRA_RULES
#pragma diag(pop)
#endif

#ifdef __ECC__	/* for C/C++ access */
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __cplusplus
void %Name%_InitFunction(void**, VDK::Thread::ThreadCreationBlock const *);
#else
void %Name%_InitFunction(void** inPtr, VDK_ThreadCreationBlock const * pTCB);
#endif
void %Name%_DestroyFunction(void** inPtr);
int32_t  %Name%_ErrorFunction(void** inPtr);
void %Name%_RunFunction(void** inPtr);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* __ECC__ */

#ifdef __cplusplus
#include <new>

class %Name%_Wrapper : public VDK::Thread
{
public:
    %Name%_Wrapper(VDK::ThreadCreationBlock &t)
        : VDK::Thread(t)
    { %Name%_InitFunction(&m_DataPtr, &t); }

    ~%Name%_Wrapper()
    { %Name%_DestroyFunction(&m_DataPtr); }

    int ErrorHandler()
    { 
      return %Name%_ErrorFunction(&m_DataPtr);
     }

    void Run()
    { %Name%_RunFunction(&m_DataPtr); }

    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock &t)
    { return new (t) %Name%_Wrapper(t); }
};

#endif /* __cplusplus */
#endif /* _%Name%_H_ */

/* ========================================================================== */
