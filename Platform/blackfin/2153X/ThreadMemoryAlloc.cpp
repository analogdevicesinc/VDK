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
 *  Comments: Stack and context allocation.  This is specific to some processor
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

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

/*
 * Called from the thread's constructor to allocate memory for:
 *  - The stack(s), including allowance for context save
 *  - The debug info area (conditionally)
 */

void
VDK::Thread::AllocateThreadMemory(
    const ThreadTemplate &tplate,
	ThreadCreationBlock	&tcb
)
{
    unsigned int heap_index;
    if (m_UserAllocatedStack)
    {
        // If we end up here, the user gave us a pointer for the thread stack.
        // Store the end of the memory allocated as the stack pointer.
        m_StackP = tcb.stack_pointer + m_StackSize - 1;
    }
    else
    {

        // Allocation memory for the stacks and context save area.
        // The stack pointers are adjusted b/c the stacks grow TOWARDS 0x0000,
        // so the beginning of the stack pointers want to be at at the end of
        // the arrays.
    
	    // Claim space for the combined stack and context save area.
	    //   |   <- stack   | Context save area  -> |
	    // if the malloc fails, it will be reported by DidConstructOkay()
	    m_StackSize += TMK_QueryContextRecordSize()/4;
    
        // for the idle thread everything comes from the same heap. For the others,
        // there are different ones for the stack/s and context/s
    
	    heap_index = g_Heaps[tplate.stack1Heap].m_Index;
        m_StackP = (unsigned int *)heap_malloc( heap_index, sizeof(int) * m_StackSize );

        if (m_StackP != 0) 
        {
	   	    m_StackP += m_StackSize - 1;
	    }
    }
    
#if (VDK_INSTRUMENTATION_LEVEL_==2)
	heap_index = g_Heaps[tplate.threadStructHeap].m_Index;
	m_DebugInfoP = (DebugInfo*) heap_malloc(heap_index,sizeof(DebugInfo));
	
	// Fill the stack memory to check the stack usage
	// Note: currently it measures only the primary stack,i.e., m_StackP 
	//
		FillMemory(m_StackP,m_StackSize,STACK_FILL_VALUE);
#endif /* VDK_INSTRUMENTATION_LEVEL_==2 */
        if (tplate.messagesAllowed) {
	    heap_index = g_Heaps[tplate.threadStructHeap].m_Index;
		void* queue_ptr = heap_malloc(heap_index, sizeof (VDK::MessageQueue));
                m_MessageQueuePtr= new (queue_ptr) VDK::MessageQueue;
	}
	
	/**
	 * Test whether or not construction is okay...
	 */
    if (m_StackP == 0)
        s_didConstructOK = false;
    else if (tplate.messagesAllowed && MessageQueue() == 0)
    	s_didConstructOK = false;
// the priority can now be zero as we construct the idle thread with this APIs
// also
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
            m_StackP - (m_StackSize - 1)
			);
	}
}



/*
 * Called from the thread's destructor
 *
 */

void
Thread::FreeThreadMemory(const ThreadTemplate &tplate)
{
	// We only want to de-init the thread if if was successfully
	// initialised.
	//
    if (NULL != m_StackP)
    {
		TMK_DeInitThread(this);	

        // Free up the stack pointer, only if the user did not allocate their own stack
        if (!m_UserAllocatedStack)
        {
		    unsigned int heap_index = g_Heaps[tplate.stack1Heap].m_Index;
            m_StackP -= (m_StackSize -1);
		    heap_free(heap_index,m_StackP);	
        }
	}

	// Free the debug info area
	if (m_DebugInfoP) {
		unsigned int heap_index = g_Heaps[tplate.threadStructHeap].m_Index;
		heap_free(heap_index, m_DebugInfoP); 
	}

	if (m_MessageQueuePtr) {
		unsigned int heap_index = g_Heaps[tplate.threadStructHeap].m_Index;
		(*g_MessageQueueCleanup)(m_MessageQueuePtr);  // call destructor indirectly
		heap_free(heap_index,m_MessageQueuePtr);
	}
}

} // namespace VDK

