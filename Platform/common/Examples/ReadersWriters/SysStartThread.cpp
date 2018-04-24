/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread SysStartThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "SysStartThread.h"
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

bool g_EndFlag;
#define NUM_READER_THREADS 2

/******************************************************************************
 *  SysStartThread Constructor
 */
 
SysStartThread::SysStartThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

	/* TODO - Put code to be executed when this thread has just been created HERE */

}

/******************************************************************************
 *  SysStartThread Destructor
 */
 
SysStartThread::~SysStartThread()
{
	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */

	/* TODO - Put code to be executed just before this thread is destroyed HERE */

}

/******************************************************************************
 *  SysStartThread Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
SysStartThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

    return new (tcb) SysStartThread(tcb);
}

/******************************************************************************
 *  SysStartThread Run Function (SysStartThread's main{})
 */
 
void
SysStartThread::Run()
{
	g_EndFlag = false;
	VDK::CreateThread(kWriterThread);
		
	for(int i=0; i < NUM_READER_THREADS; i++)
		VDK::CreateThread(kReaderThread);

	VDK::SetPriority(kBootSysStartThread,VDK::kPriority7);

    printf("--------Finished--------\n");
    fflush(stdout);
	g_EndFlag = true;

}

/******************************************************************************
 *  SysStartThread Error Handler
 */
 
int
SysStartThread::ErrorHandler()
{
	/* TODO - Put this thread's error handling code HERE */

	/* The default ErrorHandler (called below) kills the thread */
	return (VDK::Thread::ErrorHandler());
}

