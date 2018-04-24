/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Abstract base class for a schedulable thread of execution
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Thread is the base class for all threads of execution in the RTOS.
 *  The thread class contain all of the data structure required for pending on
 *  semaphores or events (with or without timeout), waiting in the time queue,
 *  waiting to run in the ready queue or executing.  The class provides two
 *  entry points: Run() and Error().  Run() is a pure virtual function.  Error()
 *  has a default implentation, but may be overridden by a user implementation.
 *  Threads are always accessed by pointers within the kernel, but always only
 *  by ThreadIDs from the public interface to avoid dangling references.  
 *
 *  Threads have two sets of doubly linked list pointers through multiple 
 *  inheritance of ListElement.  Through ThreadListElement, Thread inherits 
 *  pointers that it uses for insertion into the ready queue or for pending on
 *  a semaphore, event or fifo.  Through TimeListElement, Thread inherits a 
 *  second set of pointers that it uses for insertion onto the time queue for 
 *  sleep (pure delay) or for timeout in conjunction with pending.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <stdlib.h>

#include "TMK.h"

#include "VDK_Thread.h"
#include "DebugInfo.h"
#include "VDK_API.h"
#include "VDK_Macros.h"
#include "VDK_Abs_Device.h"
#include "Heap.h"
#include "KernelPanic.h"
#include "BitOps.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("SignalCreation")
#pragma file_attr("ThreadCreation")

namespace VDK {

    // Class static variable
    bool Thread::s_didConstructOK;
    extern ThreadTemplate g_ThreadTemplates[];


Thread::Thread(ThreadCreationBlock	&tcb)
    : m_DebugInfoP(0),
    m_BlockedOnIndex(0),
    m_LastErrorValue(0),
    m_DataPtr(0),
    m_TimeoutOccurred(false),
    m_ShouldDispatchError(true),
    m_LastErrorType(kNoError),
    m_MessageQueuePtr(NULL),
    m_StackP(0)
#if __ADSPTS__ 
    	,m_StackP2(0)
#endif
{
    TMK_InitTimeElement(this, Timeout);	

    m_TemplatePtr = tcb.pTemplate;
    m_InitialPriority = tcb.thread_priority;
    m_StackSize = tcb.thread_stack_size;
#ifdef __ADSPTS__
    m_StackSize2 = (*tcb.pTemplate).stackSize2;
#endif

#ifndef __ADSPTS__
    if (NULL == tcb.stack_pointer)
        m_UserAllocatedStack = false;
    else
        m_UserAllocatedStack = true;
#endif

    ID() = tcb.thread_id;

    // reset all of the slot values. m_SlotValue[num] is a void* but its
    // size should be the same as a char*

    memset(m_SlotValue, 0, sizeof(char*) * kMaxNumDataSlots);


    // Setup the variable that marks which DCBs are used
    m_DCBNotUsed = (1 << kNumDCBs) - 1;

    // AllocateThreadMemory is a private member function that allocates space
    // for the stack(s), the context saves area(s) and (conditionally) the 
    // debug info.  It is broken out into a separate file (ThreadMemoryAlloc.cpp)
    // so that different processors may have specialized versions that are
    // linked instead of the default by the make process.
    AllocateThreadMemory(*m_TemplatePtr, tcb);  // calls TMK_InitThread()

#if (VDK_INSTRUMENTATION_LEVEL_==2)
	if (NULL != m_DebugInfoP)
	{
		// Initialize the debug info structure
		memset(m_DebugInfoP, 0, sizeof(DebugInfo));

		// set creation time (Ticks & Cycles).
		m_DebugInfoP->m_tCreationTime = GetUptime();
		void * t2 = &(m_DebugInfoP->m_llCreationCycle[0]);
		GET_CYCLE_COUNT_( t2 );
	}
#endif /* VDK_INSTRUMENTATION_LEVEL_==2 */
}				 

#pragma param_never_null pTmkThread
void
Thread::RunFunc(TMK_Thread *pTmkThread)
{
#pragma suppress_null_check
    Thread *pSelf = static_cast<Thread*>(pTmkThread);
    
    pSelf->Run();  // virtual call

	DestroyThread(pSelf->ID(), false);
	
	// The following will never be executed, honest...
	KernelPanic(kInternalError, kInvalidThread, pSelf->ID());	
}


//   overloaded new operator so we can allocate from the heap we need to use.
//   The parameter is the templateID so the users don't really know where we
//   are getting information from (tidier) 

void * Thread::operator new (size_t allocate_size,  ThreadType templateID){
         return _heap_malloc(g_Heaps[g_ThreadTemplates[templateID].threadStructHeap].m_Index,allocate_size);
}

void * Thread::operator new (size_t allocate_size){
         return ::new char[allocate_size];
}

void * Thread::operator new (size_t allocate_size,  ThreadCreationBlock &tcb){
    if (tcb.template_id != -1)
         return _heap_malloc(g_Heaps[g_ThreadTemplates[tcb.template_id].threadStructHeap].m_Index,allocate_size);
     else
         return _heap_malloc(g_Heaps[tcb.pTemplate->threadStructHeap].m_Index,allocate_size);
}


}
