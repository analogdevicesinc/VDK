/* =============================================================================
 *
 *  Description: This is the C implementation for Thread PhilosopherThread
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * 
 * ===========================================================================*/

/* Get access to any of the VDK features & datatypes used */
#include "VDK.h"
#include <stdio.h>
#include "PhilosopherThread.h"

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

typedef struct PhilosopherData
{
    char *name;
    VDK_SemaphoreID left, right;
    VDK_EventBitID  full;
} PhilosopherData;

const PhilosopherData philosopherInstanceData[5] =
{
    {"1", kChopStick5, kChopStick1, kPhilosopher1Full},
    {"2", kChopStick1, kChopStick2, kPhilosopher2Full},
    {"3", kChopStick2, kChopStick3, kPhilosopher3Full},
    {"4", kChopStick3, kChopStick4, kPhilosopher4Full},
    {"5", kChopStick4, kChopStick5, kPhilosopher5Full}
};


void PrintState(char* name,char* state);

void PrintState(char* name,char* state) {                                                      
    printf("Philosopher-%s is %s at time %d\n",name,state,VDK_GetUptime());   
    fflush(stdout);                                                           
}

void
PhilosopherThread_RunFunction(void **inPtr)
{
  int chopsticksAvailable;
  const PhilosopherData *pData = (const PhilosopherData *)*inPtr;

  /* We need to wait for everbody else */
   
  /* philosopher is thinking now */
  PrintState(pData->name,"THINKING");

  /* philosopher collects both left and right chopsticks, if not available she will wait */

  /* philosopher is hungry */
  PrintState(pData->name,"HUNGRY");

  do {
    /* make sure no other thread takes control */
    VDK_PushUnscheduledRegion();
    /* if the chopsticks are available then acquire them atomically */
    chopsticksAvailable =  ((VDK_GetSemaphoreValue(pData->left)) && (VDK_GetSemaphoreValue(pData->right)));

    if (chopsticksAvailable != 0 ) /* Both chopsticks are available */
    {  
      PrintState(pData->name,"Grabbing both");      
      /* philosopher takes her left and right chopsticks  */
      VDK_PendSemaphore(pData->left,0U);  
      VDK_PendSemaphore(pData->right,0U); 

      /* let other threads take control now */
      VDK_PopUnscheduledRegion();
    } else {
      /* give someone else a chance */
      VDK_PopUnscheduledRegion();
      VDK_Yield();
    }
  } while ( chopsticksAvailable == 0 );

  /* philosopher is eating now */
  PrintState( pData->name,"EATING"); 
  VDK_Sleep( 3U ); /* Just to simulate eating */
	
  PrintState( pData->name,"DONE"); 

  /* once she finishes she will put both the chopsticks back on the table */
  /* philosopher puts both the chopsticks on the table */
  VDK_PostSemaphore(pData->left);  
  VDK_PostSemaphore(pData->right); 

  /* let the System control thread know that I'm done */
  VDK_SetEventBit(pData->full);

  PrintState( pData->name,"LEAVING");

  /* Check if all threads have finished to print a message */
  if (VDK_GetEventValue(kAllPhilosophersFull) == true)
  {
  	VDK_PushUnscheduledRegion();
	printf("---------- All Done ----------\n");
	fflush(stdout);
	VDK_PopUnscheduledRegion();

  }

  /* The semaphore never gets posted. The thread will be pending forever */
  VDK_PendSemaphore(kAllDone,0U);  
}

int
PhilosopherThread_ErrorFunction(void **inPtr)
{
	/* Put this thread's error handling code HERE */
	/* we ignore this error because we know we getting while checking the
	 * value of a semaphore in an unscheduled region */
	if (VDK_GetLastThreadError() == VDK_kDbgPossibleBlockInRegion) {
		return VDK_kDbgPossibleBlockInRegion;
	}
	else {
		/* The default ErrorHandler goes to KernelPanic */
  		VDK_CThread_Error(VDK_GetThreadID());
		return 0;
	}
}

void
PhilosopherThread_InitFunction(void **inPtr, VDK_ThreadCreationBlock const *pTCB)
{
	/* Put code to be executed when this thread has just been created HERE */

    int instanceNum = *((int *)pTCB->user_data_ptr);

    *inPtr = (void*) (&philosopherInstanceData[instanceNum - 1]);

	/* This routine does NOT run in new thread's context.  Any non-static thread
	 *   initialization should be performed at the beginning of "Run()."
	 */
}

void
PhilosopherThread_DestroyFunction(void **inPtr)
{
	/* Put code to be executed when this thread is destroyed HERE */

	/* This routine does NOT run in the thread's context.  Any VDK API calls
  	 *   should be performed at the end of "Run()."
	 */
}


