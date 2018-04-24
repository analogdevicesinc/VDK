/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for header file formatting
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

#ifndef IDLETHREAD_H_
#define IDLETHREAD_H_

#include "TMK.h"

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "KernelPanic.h"


namespace VDK
{

    class IdleThread : public VDK::Thread
    {
    public:
        /**
         * Idle Thread constructor...
         *
         * cannot make the constructor private b/c it actually NEEDS to be
         * instantiated.
         */
        IdleThread(ThreadCreationBlock &inTCB);

        /**
         * Idle Thread destructor
         * we need this in IdleThread.cpp so we don't get the vtable with the
         * Run function which customers can replace
         */
        ~IdleThread(); 

        /**
         * The public run function
         */
        virtual void Run();

        /** 
         * The creation function.
         *
         * Also sets up g_IdleThreadPtr
         */
        static VDK::Thread* Create(ThreadCreationBlock    &inTCB);

        /**
         * Add a thread to the list of threads that should be deleted
         */
        void    AddToKillList(VDK::Thread *inThreadP)
                    { TMK_AppendThreadToList(&m_threadList, inThreadP); }

		bool  	KillThreadsInPurgatory();

    protected:
	TMK_Lock            m_lock;
        TMK_ThreadList      m_threadList;
    };

    extern VDK::IdleThread   *g_IdleThreadPtr;

} // namespace VDK



#endif /* IDLETHREAD_H_ */



