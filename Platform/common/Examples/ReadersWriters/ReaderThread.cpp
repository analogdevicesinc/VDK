/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread ReaderThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "ReaderThread.h"
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

#define BUFFER_SIZE 16   /* Reader and writer buffer sizes */
#define TIME_OUT 1000    /* Semaphore timeout */

#define PRINT_DATA(who,id,state)\
do                                                                  \
{                                                                   \
    printf( "%s-%d is %s\n" , (char*)who, (int)id, (char *)state);  \
    fflush(stdout);                                                 \
} while(0)

int g_ReadCount=0,g_Count=0;
int g_WriterBuffer[BUFFER_SIZE];  /* Writer threads writes data in to it */
int g_ReaderBuffer[BUFFER_SIZE]; /* Reader threads read data in to it */
//

static bool g_ReadErrorFlag=false; /* Set in error handler for this thread */


/******************************************************************************
 *  ReaderThread Constructor
 */
 
ReaderThread::ReaderThread(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

	/* TODO - Put code to be executed when this thread has just been created HERE */

}

/******************************************************************************
 *  ReaderThread Destructor
 */
 
ReaderThread::~ReaderThread()
{
	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */

	/* TODO - Put code to be executed just before this thread is destroyed HERE */

}

/******************************************************************************
 *  ReaderThread Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
ReaderThread::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */

    return new (tcb) ReaderThread(tcb);
}

/******************************************************************************
 *  ReaderThread Run Function (ReaderThread's main{})
 */
 
void
ReaderThread::Run()
{	
	do
	{
		/* Get access to the read counter */
		VDK::PendSemaphore(kProtectReadCount,TIME_OUT);
				g_ReadCount +=1;
		if (g_ReadCount == 1)
			VDK::PendSemaphore(kProtectDataBase,TIME_OUT);  
		/* Allow other readers to go */
		VDK::PostSemaphore(kProtectReadCount);	
		/* Protect Read count as multiple readers may be reading at the same time */
		VDK::PendSemaphore(kProtectReadCount,TIME_OUT);

		g_ReaderBuffer[g_Count - 1] = g_WriterBuffer[g_Count - 1];
		PRINT_DATA("Reader",VDK::GetThreadID(),"--Reading DataBase");
		g_ReadCount -= 1;

		if(g_ReadCount == 0) /* if last reader then allow writer to go*/
			VDK::PostSemaphore(kProtectDataBase);		
					
		VDK::PostSemaphore(kProtectReadCount); /* Allow other readers */

		if(g_ReadErrorFlag || g_Count == BUFFER_SIZE)
			break;

		VDK::Yield();
		
	}while(!g_EndFlag);

	/* TODO - Put the thread's exit from "main" HERE */
}

/******************************************************************************
 *  ReaderThread Error Handler
 */
 
int
ReaderThread::ErrorHandler()
{
	/* TODO - Put this thread's error handling code HERE */

	/* The default ErrorHandler (called below) kills the thread */
	g_ReadErrorFlag = true;
	return (VDK::Thread::ErrorHandler());
}

