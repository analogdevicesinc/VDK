/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread WriterThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "WriterThread.h"
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

extern bool g_EndFlag;         /* flag is set to exit reader/writer's */
static bool g_WriteErrorFlag=false; /* flag is set if there is any error in the system */
extern int g_Count;
#define BUFFER_SIZE 16
extern int g_WriterBuffer[];

#define PRINT_DATA(who,id,state)\
do                                                                  \
{                                                                   \
    printf( "%s-%d is %s\n" , (char*)who, (int)id, (char *)state);  \
    fflush(stdout);                                                 \
} while(0)



/******************************************************************************
 *  WriterThread Constructor
 */
 
WriterThread::WriterThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

	/* TODO - Put code to be executed when this thread has just been created HERE */

}

/******************************************************************************
 *  WriterThread Destructor
 */
 
WriterThread::~WriterThread()
{
	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */

	/* TODO - Put code to be executed just before this thread is destroyed HERE */

}

/******************************************************************************
 *  WriterThread Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
WriterThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

    return new (tcb) WriterThread(tcb);
}

/******************************************************************************
 *  WriterThread Run Function (WriterThread's main{})
 */
 
void
WriterThread::Run()
{
int counter=100; /*Some number for start writing in to data base */
	/* TODO - Put the thread's "main" Initialzation HERE */
	do
	{
		/* While writing do not allow any other writer or reader */
		VDK::PendSemaphore(kProtectDataBase,0);
		g_WriterBuffer[g_Count++] = counter++;
		PRINT_DATA("Writer",VDK::GetThreadID(),"--Updating Database");
		/* Release exclusive access to the data to other writer/reader's */
		VDK::PostSemaphore(kProtectDataBase); 
		
		if(g_WriteErrorFlag || g_Count == BUFFER_SIZE)
			break;
		VDK::Yield(); /* to give access to readers/writers */

	}while(!g_EndFlag); /* This flag is set to 0 by the sys start thread.*/

	/* TODO - Put the thread's exit from "main" HERE */
}

/******************************************************************************
 *  WriterThread Error Handler
 */
 
int
WriterThread::ErrorHandler()
{
	/* TODO - Put this thread's error handling code HERE */

	/* The default ErrorHandler (called below) kills the thread */
	g_WriteErrorFlag = true;
	return (VDK::Thread::ErrorHandler());
}


