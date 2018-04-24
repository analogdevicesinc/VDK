/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Memory allocation for a Thread's constructor
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Stack and context allocation.  This is specific to SHARC processors
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#include "DebugInfo.h"
#include "VDK_API.h"
#include "VDK_Macros.h"
#include <stdlib.h>
#include "Heap.h"
#if (VDK_INSTRUMENTATION_LEVEL_==2)
#include "VDK_StackMeasure.h"
#endif

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

namespace VDK {
extern void (*g_MessageQueueCleanup)(VDK::MessageQueue *);
}
/*
 * Called from the thread's constructor to allocate memory for:
 *  - The stack(s)
 *  - The context save area(s)
 *  - The debug info area (conditionally)
 */

void
VDK::Thread::AllocateThreadMemory(
    const ThreadTemplate &tplate,
	ThreadCreationBlock	&tcb
)
{
    // Allocation memory for the stacks and context save area.
    // The stack pointers are adjusted b/c the stacks grow TOWARDS 0x0000,
    // so the beginning of the stack pointers want to be at at the end of
    // the arrays.

	// Claim space for both stack and context save area.
	//   |   <- stack   | Context save area  -> |
	// if the malloc fails, it will be reported by DidConstructOkay()
	// ...also for the two thread local storage arrays...
    
	

    // for the idle thread everything comes from the same heap. For the others,
    // there are different ones for the stack/s and context/s
	unsigned int stack_heap_index = g_Heaps[tplate.stack1Heap].m_Index;
	unsigned int context_heap_index = g_Heaps[tplate.context1Heap].m_Index;
	unsigned int thread_struct_heap_index = g_Heaps[tplate.threadStructHeap].m_Index;

    if (m_UserAllocatedStack)
    {
        // If we end up here, the user gave us a pointer for the thread stack.
        // Store the end of the memory allocated as the stack pointer.
        m_StackP = tcb.stack_pointer + m_StackSize - 1;
    } 
    else 
    {
        m_StackP = (unsigned int *)heap_malloc( stack_heap_index, sizeof(int) * m_StackSize );
        if (m_StackP != 0) 
        {
	   	    m_StackP += m_StackSize - 1;
	    }
    }
    

	unsigned int cswitchSize = TMK_QueryContextRecordSize();
	unsigned int *contextP = (unsigned int *)heap_malloc( context_heap_index, sizeof(int) * cswitchSize );

#if (VDK_INSTRUMENTATION_LEVEL_==2)
	m_DebugInfoP = (DebugInfo*) heap_malloc(thread_struct_heap_index, sizeof(DebugInfo));
	
	// Fill the stack memory to check the stack usage
	// Note: currently it measures only the primary stack,i.e., m_StackP 
	//
	FillMemory(m_StackP,m_StackSize,STACK_FILL_VALUE);
#endif /* VDK_INSTRUMENTATION_LEVEL_==2 */

// This should be put in once the IDDE can support the Messages Allowed field
	if (tplate.messagesAllowed) {
		void* queue_ptr = heap_malloc(thread_struct_heap_index, sizeof (VDK::MessageQueue));
		m_MessageQueuePtr= new (queue_ptr) VDK::MessageQueue;
	}
	
	/**
	 * Test whether or not construction is okay...
	 */
    if ((m_StackP == 0) || (contextP == 0))
        s_didConstructOK = false;
    else if (tplate.messagesAllowed && MessageQueue() == 0)
        s_didConstructOK = false;
// We allow priority to be 0 as the idle thread is also constructed with this
#if (VDK_INSTRUMENTATION_LEVEL_==2)
    else if ((Priority() > kPriority1) || (Priority() < 0) || (m_StackSize <= 0) || (m_DebugInfoP ==0))
        s_didConstructOK = false;
#elif (VDK_INSTRUMENTATION_LEVEL_==1)
    else if ((Priority() > kPriority1) || (Priority() < 0) || (m_StackSize <= 0))
        s_didConstructOK = false;
#endif /* VDK_INSTRUMENTATION_LEVEL_ */
	else
		s_didConstructOK = true;

	if (s_didConstructOK)
	{
		TMK_InitThread(
			this,    // the thread to be initialised
		    tcb.thread_priority,
            RunFunc,   // thread run function
            m_StackP,
			m_StackSize,
			contextP
		);
	}
	else
	{
        // In order to free the context area in FreeThreadMemory, the pointer
        // is obtained using a call to TMK_DeInitThread. If we are not calling
        // TMK_InitThread, then we must free the context area here while we
        // still have the pointer.
		if (NULL != contextP)
		{
			heap_free(context_heap_index, contextP);
		}
	}
}



/*
 * Called from the thread's destructor
 *
 */

void
VDK::Thread::FreeThreadMemory(const ThreadTemplate &tplate)
{
	void *contextP  = NULL;

	// We only want to de-init the thread if it was successfully
	// initialised.
	//
    if (NULL != m_StackP)
    {
		TMK_DeInitThread(this, &contextP);	

        // Free up the stack pointer, only if the user did not allocate the stack
        if (!m_UserAllocatedStack)
        {
		    unsigned int heap_index =g_Heaps[tplate.stack1Heap].m_Index;
            m_StackP -= (m_StackSize -1);
		    heap_free(heap_index,m_StackP); // Deallocates the stack
        }
	}

	// Free the context save area
	if (contextP) {
	    unsigned int heap_index =g_Heaps[tplate.context1Heap].m_Index;
		heap_free(heap_index,contextP);
	}

	// Free the debug info area
	if (m_DebugInfoP) {
		unsigned int heap_index =g_Heaps[tplate.threadStructHeap].m_Index;
		heap_free(heap_index,m_DebugInfoP);
	}
	if (m_MessageQueuePtr) {
		unsigned int heap_index =g_Heaps[tplate.threadStructHeap].m_Index;
		(*g_MessageQueueCleanup)(m_MessageQueuePtr);  // call destructor indirectly
		heap_free(heap_index,m_MessageQueuePtr);
	}
}



