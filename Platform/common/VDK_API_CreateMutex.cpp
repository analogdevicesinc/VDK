/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the Create/DestroyMutex functions
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: This APIs are not used by the RTL so will only be linked in
 *  if customers use mutexes (unlike the other mutex functions that will be
 *  linked in if I/O is used)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "ThreadTable.h"
#include "KernelPanic.h"
#include "VDK_History.h"

#include "VDK_RMutex.h"
#ifndef __ADSPTS__
// TS does not like suppressing this warning
#pragma diag(suppress:1994:" multicharacter character literal potential portability problem  ")
#endif

#pragma file_attr("OS_Component=Mutex")
#pragma file_attr("Mutex")
#pragma file_attr("MutexCreation")
#pragma file_attr("SignalCreation")
#pragma file_attr("FuncName=CreateMutex")

namespace VDK {
/****************************************************************
 * VDK::CreateMutex
 *
 * Allocate (and initialise) a mutex on the system heap.
 ****************************************************************/
MutexID CreateMutex(void)
{
    Mutex *pMutex = static_cast<Mutex*>(malloc(sizeof(Mutex)));

#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (IS_ISR_LEVEL ())
        KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
#endif

    if (NULL != pMutex) {
        InitMutex(pMutex, sizeof(Mutex), true);
        HISTORY_(VDK::kMutexCreated, reinterpret_cast<int>(pMutex), GetThreadID(
));
        return static_cast<MutexID>(reinterpret_cast<unsigned int>(pMutex));
    }
    else  {
#if (VDK_INSTRUMENTATION_LEVEL_>0)
        DispatchThreadError(kMutexCreationFailure, sizeof(Mutex));
#endif
        return static_cast<MutexID>(UINT_MAX);
    }


}

/****************************************************************
 * VDK::DestroyMutex
 *
 * Deinitialise and free a mutex that was created on the heap
 * by CreateMutex().
 ****************************************************************/
void DestroyMutex( MutexID inMutexID)
{
    Mutex *pMutex = reinterpret_cast<Mutex *>(inMutexID);
    SystemError errorCode =kNoError;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (IS_ISR_LEVEL ())
        KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
#endif

    TMK_SuspendScheduling(); // Freeze so two threads cannot be destroying
                                   // the same mutex
    if ERRCHK(MTX_TST(pMutex) ) {
        if (pMutex->level == 0 && TMK_IsThreadListEmpty(&pMutex->pendingThreads)
)  {

            DeInitMutex(pMutex);
            free (pMutex);
            HISTORY_(VDK::kMutexDestroyed, inMutexID, GetThreadID());
       }
       else {
            errorCode = kMutexDestructionFailure;
       }
    }
#if (VDK_INSTRUMENTATION_LEVEL_>0)
    // the argument was NULL
    else {
        errorCode = kInvalidMutexID;
    }
#endif

    TMK_ResumeScheduling(); 

#if (VDK_INSTRUMENTATION_LEVEL_>0)
   if (errorCode != kNoError)
        DispatchThreadError(errorCode, inMutexID);
#endif

}
}; // namespace VDK
