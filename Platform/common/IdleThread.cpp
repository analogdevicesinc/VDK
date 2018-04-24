/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/*
 *=============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for source code formatting
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The idle thread is the lowest priority threads, and runs when no
 *	other threads exist or when all other threads are blocked.  The idle thread
 *	does not have an ID (not entered in g_ThreadTable) and cannot be destroyed.
 *	During idle time, the system destroys any threads whose destruction has been
 *	postponed.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include <stdlib.h>

#include "IdleThread.h"
#include "SlowIdle.h"
#include "Heap.h"
#include <new>
#include "KernelPanic.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("IdleThread")

namespace VDK{
	extern IdleThread    *g_IdleThreadPtr;
	extern ThreadTemplate g_IdleThreadTplate;

	extern void CleanupThread(Thread *pThread);

/** ----------------------------------------------------------------------------
 * FUNC   : IdleThread::IdleThread(ThreadCreationBlock &inTCB)
 *
 * DESC   : Constructor for IdleThread
 *
 * PARAMS : inTCB -- just passed up the class hierarchy to Thread.
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
IdleThread::IdleThread(ThreadCreationBlock &inTCB)
    : Thread(inTCB)
{
	TMK_InitThreadList(&m_threadList);
	TMK_InitGlobalInterruptLock(&m_lock, TMK_KERNEL_LOCK_PRECEDENCE+1);
	g_IdleThreadPtr = this;
}


/** ----------------------------------------------------------------------------
 * FUNC   : IdleThread::~IdleThread()
 *
 * DESC   : Destructor for IdleThread
 *
 * PARAMS : None
 * 
 * RETURNS: N/A
 */ // -------------------------------------------------------------------------
IdleThread::~IdleThread()
{
}

/** ----------------------------------------------------------------------------
 * FUNC   : IdleThread::CreateIdleThread(ThreadCreationBlock    &inTCB)
 *
 * DESC   : Function that actually calls the IdleThread constructor
 *
 * PARAMS : inTCB -- the TCB that is passed to IdleThread's constructor
 *          
 * RETURNS: a pointer to the thread.
 */ // -------------------------------------------------------------------------
Thread*
IdleThread::Create(ThreadCreationBlock    &inTCB)
{
	void* ptr =_heap_malloc(g_Heaps[g_IdleThreadTplate.threadStructHeap].m_Index, sizeof(IdleThread));
    return ::new (ptr) IdleThread(inTCB);
}



/** ----------------------------------------------------------------------------
 * FUNC   : IdleThread::KillThreadsInPurgatory()
 *
 * DESC   : Delete the memory of threads for which DestroyThread() has been called.
 *          
 * PARAMS : N/A
 * 
 * RETURNS: bool : True if thread deleted
 */ // -------------------------------------------------------------------------
bool
IdleThread::KillThreadsInPurgatory()
{
    bool retval = false;

	TMK_AcquireMasterKernelLock();

    // while we have threads to delete, delete them
    Thread  *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&m_threadList));

    if (NULL != pThread && -1 != pThread->ID())
    {
		CleanupThread(pThread);
    	retval = true;
    }

	TMK_ReleaseMasterKernelLock();

	return retval;
}

} // namespace VDK

