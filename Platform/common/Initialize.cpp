/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The main entry point
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The entry point for the entire VDK framework. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include <climits>

#include "TMK.h"
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "IdleThread.h"
#include "Heap.h"
#include "KernelPanic.h"
#include "BuildNumber.h"
#pragma file_attr("Startup")


//------------------------------------------------------------------
//  globals:
// 
extern "C" void InitialisePlatform();

namespace VDK
{
    extern VersionStruct g_VDKVersion;
    PanicCode	g_KernelPanicCode = kNoPanic;
    SystemError 	g_KernelPanicError = kNoError;
    int		g_KernelPanicValue = 0;
    int		g_KernelPanicPC = 0;
    int 		g_remainingTicks=0;
    IdleThread *g_IdleThreadPtr;
    bool    g_VdkIsInitialized= 0;
    extern ThreadTemplate g_IdleThreadTplate;

    extern TMK_Lock      g_CriticalRegionLock;
    extern TMK_LockState g_CriticalRegionState;

    extern ThreadTemplate                   g_ThreadTemplates[];
    extern unsigned int             kNumThreadTemplates;
    extern int kNumHeaps;
    void NewHandler(void);

    // VDK configuration variables set in GUI
    extern int 				kNumBootThreads;
    extern BootThreadInfo	g_BootThreadInfo[];
    extern unsigned int     g_localNode;

    extern int        g_CriticalRegionCount;
    extern int        g_SchedulerRegionCount;

    extern void KillThreadHookFn(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

    extern TMK_PFCSwitch g_pfKillThreadChain;

    extern TMK_PFCSwitch g_pfHistory_FirstTimeChain;
    extern TMK_PFCSwitch g_pfHistory_RuntimeChain;
    extern TMK_PFCSwitch g_pfHistory_InterruptChain;
    extern TMK_PFCSwitch g_pfHistory_KillThreadChain;

	extern TMK_PFDispatch g_pfDispatchLoggingChain;
	extern void DispatchLoggingHookFn(void);

    extern void History_FirstEntry(TMK_Thread *pOldThread, TMK_Thread *pNewThread);
    extern void History_Runtime   (TMK_Thread *pOldThread, TMK_Thread *pNewThread);
    extern void History_Interrupt (TMK_Thread *pOldThread, TMK_Thread *pNewThread);
    extern void History_KillThread(TMK_Thread *pOldThread, TMK_Thread *pNewThread);

    extern IMASKStruct g_InitialISRMask;

    extern void UserDefinedInterruptServiceRoutines();

    extern int kNumBootEntries;
    extern BootFuncPointers g_InitBootFunctionP[];
    extern "C" bool vdkMainMarker;
    bool vdkMainMarker = 0 ;

    static VDK::ThreadCreationBlock idleThreadTCB =
    {
		static_cast<VDK::ThreadType>(UINT_MAX), // template_id
		static_cast <VDK::ThreadID> (0),        // thread_id
		0,                                      // thread_stack_size
		static_cast<VDK::Priority>(0),          // thread_priority
		NULL,                                   // user_data_ptr
		&g_IdleThreadTplate                     // pTemplate
    };

	bool InitBootThreads() 
	{
		for (int i = 0; i < kNumBootThreads; ++i)
		{
			ThreadCreationBlock TCB;

			TCB.template_id       = (ThreadType) (g_BootThreadInfo[i].m_templatePtr - g_ThreadTemplates);
#if (VDK_INSTRUMENTATION_LEVEL_>0)
			if (TCB.template_id >= kNumThreadTemplates || 
				TCB.template_id < 0)
				KernelPanic (kBootError, kThreadCreationFailure,i);
#endif
			TCB.thread_stack_size = 0;
			TCB.thread_priority   = (VDK::Priority)0;
			TCB.user_data_ptr     = &g_BootThreadInfo[i].m_instanceNum;
			TCB.pTemplate       = g_BootThreadInfo[i].m_templatePtr;

			ThreadID bootThread = CreateThreadEx (&TCB);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
			// The +1 is there because thread zero is the idle thread
			if (bootThread != ((i + 1) | ((g_localNode & 0x1F) << 9)))
				KernelPanic(kBootError, kThreadCreationFailure,i+1);
#endif
		}
		return false;
	}


unsigned int
GetHeapIndex(HeapID id)
{
	if NERRCHK(IS_ISR_LEVEL()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (id >= kNumHeaps)
	DispatchThreadError( kInvalidHeapID, id);
#endif
    return g_Heaps[id].m_Index;
}


/**
 * Sets up VDK's interrupt handlers, etc. and creates
 * the Idle thread.
 */
void
Initialize()
{
    //
    // We enter main in a critical region
	// we need to use g_VDKVersion so it does not get removed with section
	// elimination because it is used by the IDDE in the VDK status window. 
	// This is easier than having to change every customer's LDF.

// if VDK has already been initialized, we simply return because we don't
//  want to give an error
    if (g_VdkIsInitialized) return;

	g_VDKVersion.mBuildNumber=VERSION_BUILD_NUMBER_;

// As we now use the RTL crt there are some things we need to do to mimic the
// behaviour of our old CRT. In some of the platforms (blackfin and TS) this
// requires setting up the interrupts.

    InitialisePlatform();
	UserDefinedInterruptServiceRoutines();
	// set VDK's handler for C++ heap allocation failures
    _STD set_new_handler(NewHandler);

    // Initialise heaps
	//
    for (int i = 0; i < kNumHeaps; i++)
	{
#ifdef __ADSP21000__
          g_Heaps[i].m_Index= _heap_lookup_name(g_Heaps[i].m_ID);
#else
          g_Heaps[i].m_Index= _heap_lookup(g_Heaps[i].m_ID);
#if __ADSPTS__
	// heap 0 is initialised by the runtime libs so we don't want to do 
        // anything to it
          if (g_Heaps[i].m_Index > 0 ) 
          {
             int ret_val = _heap_init (g_Heaps[i].m_Index);
#if VDK_INSTRUMENTATION_LEVEL_ >0
             if (ret_val) 
              KernelPanic(kBootError, kHeapInitialisationFailure,i);
#endif
	  }
#endif
#endif
#if VDK_INSTRUMENTATION_LEVEL_ >0
	  if (g_Heaps[i].m_Index == (unsigned int) -1) KernelPanic(kBootError, kHeapInitialisationFailure,i);
#endif
	}

    // Initialise the microkernel
    //
    TMK_Initialize(); // will overwrite previous interrupt assignments
	
	// Insert VDK-specific thread cleanup code into the killthread chain
	g_pfKillThreadChain = TMK_HookThreadSwitch (TMK_KILLTHREADHOOK, KillThreadHookFn);

#if (VDK_INSTRUMENTATION_LEVEL_==2)
	// We have to hook these (or at least the kill hook) after the call above, so
	// that History_KillThread gets called *before* KillThreadHookFn. This is necessary
	// because KillThreadHookFn frees the thread debug info, which History_KillThread
	// writes to.
	//
	g_pfHistory_FirstTimeChain  = TMK_HookThreadSwitch (TMK_FIRSTTIMEHOOK,  History_FirstEntry);
	g_pfHistory_RuntimeChain    = TMK_HookThreadSwitch (TMK_RUNTIMEHOOK,    History_Runtime);
	g_pfHistory_InterruptChain  = TMK_HookThreadSwitch (TMK_INTERRUPTHOOK,  History_Interrupt);
	g_pfHistory_KillThreadChain = TMK_HookThreadSwitch (TMK_KILLTHREADHOOK, History_KillThread);

	//g_pfDispatchLoggingChain = TMK_HookDispatch(DispatchLoggingHookFn);
#endif

    // The idle thread template comes from VDK.cpp now but we need to set its
    // create function

    g_IdleThreadTplate.createFunction    = VDK::IdleThread::Create;

    // Create the IdleThread
    //
    VDK::ThreadID IdleID = CreateThreadEx(&idleThreadTCB);


	// Set up the interrupt mask
	//
	g_CriticalRegionState = TMK_AcquireGlobalInterruptLock(&g_CriticalRegionLock);
	TMK_ChangeInterruptMaskBits(&g_CriticalRegionLock,
								&g_CriticalRegionState,
								 g_InitialISRMask,
#ifdef __ADSPBLACKFIN__
								0 // don't clobber any bits that are already set
#else
								~g_InitialISRMask
#endif
								);

    // Initialise the boot elements (threads, semaphores, mempools, messages,...)
    // Put the Periodic semaphores in the Periodic Queue
    //
    // The last two calls should be the setting the IMASK and the timer if it
    // is required. The timer will only be required when the timer interrupt
    // specified is the default one from the ini file. The IMASK is only 
    // required in the SHARC processors that have mask bits in LIRPTL and in
    // TS101 where there are some bits we need to set. The rest of the IMASK
    // is done by VDKGen in g_InitialISRMask
    //
    for (int i = 0; i < VDK::kNumBootEntries; i++)
    	(*(VDK::g_InitBootFunctionP[i]))(); 

	g_CriticalRegionCount = 0;
	g_VdkIsInitialized = true;

	TMK_ReleaseGlobalInterruptLock(&g_CriticalRegionLock, g_CriticalRegionState);
}


/**
 * Transfers control to the VDK scheduler and begins
 * running threads.
 * 
 * Never returns.
 */
void
Run()
{


	g_SchedulerRegionCount = 0;

    // The runtime library needs to know that we are done with the 
    // initialisation to be able to use their semaphore.
    vdkMainMarker = 1;

    // Start the microkernel scheduler
    TMK_Run();
}

} // namespace VDK

