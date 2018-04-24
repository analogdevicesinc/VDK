/* =============================================================================
 *
 *  Description: This is a C++ to C Thread Header file for Thread PhilosopherThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#ifndef _PhilosopherThread_H_
#define _PhilosopherThread_H_

#ifndef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)
#endif

#include "VDK.h"

#ifndef _MISRA_RULES
#pragma diag(pop)
#endif

#ifdef __ECC__  /* for C/C++ access */
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __cplusplus
void PhilosopherThread_InitFunction(void**, VDK::Thread::ThreadCreationBlock const *);
#else
void PhilosopherThread_InitFunction(void** inPtr, VDK_ThreadCreationBlock const * pTCB);
#endif
void PhilosopherThread_DestroyFunction(void** inPtr);
int  PhilosopherThread_ErrorFunction(void** inPtr);
void PhilosopherThread_RunFunction(void** inPtr);
#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __ECC__ */

#ifdef __cplusplus
#include <new>

class PhilosopherThread_Wrapper : public VDK::Thread
{
public:
	PhilosopherThread_Wrapper(VDK::Thread::ThreadCreationBlock &t)
		: VDK::Thread(t)
	{ PhilosopherThread_InitFunction(&m_DataPtr, &t); }

	~PhilosopherThread_Wrapper()
	{ PhilosopherThread_DestroyFunction(&m_DataPtr); }

	int ErrorHandler()
	{ return PhilosopherThread_ErrorFunction(&m_DataPtr); }

	void Run()
	{ PhilosopherThread_RunFunction(&m_DataPtr); }

	static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock &t)
	{ return new (t) PhilosopherThread_Wrapper(t); }
};

#endif /* __cplusplus */
#endif /* _PhilosopherThread_H_ */

/* ========================================================================== */
