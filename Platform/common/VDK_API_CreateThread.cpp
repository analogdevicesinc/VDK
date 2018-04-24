/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public CreateThread function
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_History.h"
#include "VDK_Macros.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("ThreadCreation")
#pragma file_attr("SignalCreation")
#pragma file_attr("FuncName=CreateThread")
#pragma file_attr("FuncName=CreateThreadEx")


namespace VDK
{
    extern ThreadTemplate     g_ThreadTemplates[];         // in vdk.cpp
    extern ThreadTable        g_ThreadTable;
    extern unsigned int       kNumThreadTemplates;         
    extern unsigned int       g_localNode;

    //
    // This is the function that does the real work of creating a
    // thread. It makes no reference to g_ThreadTemplates, and so can
    // be used to instantiate thread types that don't have an entry in
    // the thread templates array (e.g. routing threads).
    //
    ThreadID
    CreateThreadInternal( ThreadCreationBlock *inOutTCB )
    {
        const ThreadType inTemplateID = inOutTCB->template_id;
        Thread *pThread = NULL;
        const ThreadTemplate &tplate = *inOutTCB->pTemplate;

        // Don't want to get context-switched out after we have
        // obtained a valid ThreadID
        TMK_SuspendScheduling();

        // Setup the ThreadCreationBlock
        inOutTCB->thread_id = g_ThreadTable.GetNextObjectID();
        
        // If we have an invalid ThreadID, we are in error
        // When the pointer to the thread hasn't been setup, the
        // error is handled below.
        if (inOutTCB->thread_id != UINT_MAX)
        {
            // Merge the 5-bit node ID into the thread ID
            inOutTCB->thread_id = static_cast<ThreadID>(inOutTCB->thread_id | ((g_localNode & 0x1F) << 9));
            
            // Create the thread
            pThread = tplate.createFunction(*inOutTCB);

            if (NULL != pThread)
            {
                // Check that we've actually created the thread
                if (pThread->DidConstructOkay(*inOutTCB->pTemplate))
                {
                    // if so, add the thread to the appropriate tables

                    HISTORY_(VDK::kThreadCreated, pThread->ID(), GetThreadID());

                    TMK_AcquireMasterKernelLock();
                    HISTORY_(VDK::kThreadStatusChange, pThread->runStatus, pThread->ID()); // log previous status
#pragma suppress_null_check
                    TMK_Thread *pTmkThread = pThread;
                    TMK_MakeThreadReady(pTmkThread);
                    g_ThreadTable.AddObject(pThread);
                    TMK_ReleaseMasterKernelLock();
                }
                else
                {
                    // Delete the thread if we couldn't allocate the stack, context switch, etc.
                    pThread->~Thread();
                    _heap_free(g_Heaps[tplate.threadStructHeap].m_Index, pThread);
                    pThread = NULL;
                }
            }
        }

        // Re-enable scheduler
        TMK_ResumeScheduling();

        // If we failed to create the thread correctly, return an error
        if (NULL == pThread)
        {
            DispatchThreadError(kThreadCreationFailure, inTemplateID);
            return static_cast<ThreadID> (UINT_MAX);
        }

        // Otherwise return the ThreadID
        return pThread->ID();
    }

    //
    // This is the CreateThreadEx() API function, but now it's mainly a wrapper
    // around CreateThreadInternal(), above.
    //
    ThreadID
    CreateThreadEx( ThreadCreationBlock *inOutTCB )
    {
        if NERRCHK(IS_ISR_LEVEL ()) 
            KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());    

        SystemError errorCode = kNoError; 
        ThreadCreationBlock internalTCB;
        // Copy in the values from the TCB passed in by the user...
        memcpy(&internalTCB, inOutTCB, sizeof(ThreadCreationBlock));
#ifndef __ADSPTS__
        // TigerSHARC does not support CreateThread with stack pointer
        internalTCB.stack_pointer = NULL;
#endif

        const ThreadType inTemplateID = internalTCB.template_id;

        if (inTemplateID == -1) {
        if ERRCHK(internalTCB.pTemplate==0) {
            errorCode = kUnknownThreadType;
        }
        } 
        else {
        if ERRCHK(inTemplateID < kNumThreadTemplates) {
            internalTCB.pTemplate = &g_ThreadTemplates[inTemplateID];
        }
        else {
            errorCode = kUnknownThreadType;
        }
        }
        if ERRCHK(errorCode == kNoError) {
            if (0 == internalTCB.thread_stack_size)
            internalTCB.thread_stack_size = internalTCB.pTemplate->stackSize;

            if (0 == internalTCB.thread_priority)
            internalTCB.thread_priority = internalTCB.pTemplate->priority;

            inOutTCB->thread_id = CreateThreadInternal (&internalTCB);
            return (inOutTCB->thread_id);

        }
        else {
        DispatchThreadError(errorCode, inTemplateID);
            return static_cast<ThreadID> (UINT_MAX);
        }
    }

    //
    // This is the main API CreateThread() function, but now it's just a wrapper
    // around CreateThreadInternal(), above.
    //
    ThreadID
    CreateThread( const ThreadType inTemplateID )
    {
        if NERRCHK(IS_ISR_LEVEL ()) 
            KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());    

        ThreadCreationBlock tcb;

        if ERRCHK(inTemplateID < kNumThreadTemplates) {
            tcb.template_id       = inTemplateID;
            tcb.thread_stack_size = g_ThreadTemplates[inTemplateID].stackSize;
            tcb.thread_priority   = g_ThreadTemplates[inTemplateID].priority;
            tcb.user_data_ptr     = NULL;
#ifndef __ADSPTS__
            tcb.stack_pointer     = NULL;
#endif
            tcb.pTemplate         = &g_ThreadTemplates[inTemplateID];

            return CreateThreadInternal (&tcb);
        }
        else {
            DispatchThreadError(kUnknownThreadType, inTemplateID);
            return static_cast<ThreadID> (UINT_MAX);
        }
    }

} // namespace VDK

