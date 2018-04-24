/* =============================================================================
 *
 *  Description: This is a C++ implementation for Thread Status
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "Status.h"
#include <new>
#include <stdio.h>

#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")

// An enumeration to help us identify different catagories of status info
enum statusInfo {THREADS, MESSAGES, SEMAPHORES, MEMPOOLS};


/******************************************************************************
 *  statusData Data Structure
 *
 *	This data sructure is designed to hold a large amount of information 
 *	regarding the current state of a VDK project. 
 * 
 */
typedef struct statusData
{
    // Thread Data
    int noOfThreads;
    VDK::ThreadID threadID[VDK_kMaxNumThreads+1];
    char * threadName[VDK_kMaxNumThreads+1];
    VDK::Ticks runTotalTime[VDK_kMaxNumThreads+1];
    VDK::ThreadStatus threadState[VDK_kMaxNumThreads+1];
    	
	// Message Data
	int noOfMessages;
	VDK::MessageID messageID[VDK_kMaxNumActiveMessages];
    VDK::MessageDetails msgDetails[VDK_kMaxNumActiveMessages];
	VDK::PayloadDetails msgPayloadDetails[VDK_kMaxNumActiveMessages];
    
	// Semaphore Data
	int noOfSemaphores;
	VDK::SemaphoreID semaphoreID[VDK_kMaxNumActiveSemaphores];
	int count[VDK_kMaxNumActiveSemaphores];
	VDK::Ticks period[VDK_kMaxNumActiveSemaphores];
	unsigned int maxCount[VDK_kMaxNumActiveSemaphores];
	
	// Memory Pool Data
	int noOfMemPools;
	VDK::PoolID memPoolID[VDK_kMaxNumActiveMemoryPools+1];
	unsigned int blocks[VDK_kMaxNumActiveMemoryPools+1];
	void* address[VDK_kMaxNumActiveMemoryPools+1];
} statusData;

statusData status;

/******************************************************************************
 *  retrieveStatusInformation Function
 *
 *	This function will accept a pointer to a 'statusData' structure and then
 *	fill it with a large amount of Status Information obtained using the VDK
 *	APIs provided.
 *
 */
void retrieveStatusInformation()
{
    // Get thread status information
	//VDK::ThreadID threads[VDK_kMaxNumThreads+1];
	status.noOfThreads = VDK::GetAllThreads(status.threadID,VDK_kMaxNumThreads+1);
		
	// Retrieve and store all relevant Thread Status information
	for(int i=0; i < status.noOfThreads; i++)
	{
   	    // Retrieve the Thread Type and Total Run Time values
	    VDK::GetThreadTemplateName(status.threadID[i],&status.threadName[i]);
	    VDK::GetThreadTickData(status.threadID[i],0,0,0,&status.runTotalTime[i]);
	    
	    // Get the current Thread State
	    status.threadState[i] = VDK::GetThreadStatus(status.threadID[i]);
	}

	// Get message status information
    status.noOfMessages = VDK::GetAllMessages(status.messageID,VDK_kMaxNumActiveMessages);

    // Compile and store all the relevant message status information
	for(int i=0; i < status.noOfMessages; i++)
	{
	    VDK::GetMessageStatusInfo(status.messageID[i], &status.msgDetails[i], &status.msgPayloadDetails[i]);
	}
	    
	status.noOfSemaphores = VDK::GetAllSemaphores(status.semaphoreID, VDK_kMaxNumActiveSemaphores);

	for (int i = 0; i < status.noOfSemaphores; i++)
	{
	    VDK::GetSemaphoreDetails(status.semaphoreID[i], &status.period[i], &status.maxCount[i]);
	    status.count[i] = VDK::GetSemaphoreValue(status.semaphoreID[i]);
	}
	
	// Retrieve Message Status Information (+1 is to account for a potential extra
	// internal memory pool)
	status.noOfMemPools = 	VDK::GetAllMemoryPools(status.memPoolID, VDK_kMaxNumActiveMemoryPools+1);
	
	for (int i = 0; i < status.noOfMemPools; i++)
	    VDK::GetPoolDetails(status.memPoolID[i], &status.blocks[i], &status.address[i]);
		
}

/******************************************************************************
 *  printStatusInformation Function
 *
 *	This function takes two arguments, the first is a pointer to the data 
 *	structure which contains the status information and the second is an
 *	interger value which specifies the type of data that the function should 
 *  display. For the second argument one of the following enumerations can be 
 *	used: THREADS, MESSAGES, SEMAPHORES and MEMPOOLS.
 *
 */
void printStatusInformation(int statusType)
{
    switch (statusType)
    {
        case THREADS:
			printf("Thread Information (Total Runtime)\n");
			printf("----------------------------------\n");
		    for(int i=0; i < status.noOfThreads; i++)
		   	    printf("ID:%d %s\t- %d Ticks\n",status.threadID[i],status.threadName[i],status.runTotalTime[i]);
			printf("----------------------------------\n\n");
			break;
			
		case MESSAGES:
			printf("Message Payload Information\n");
			printf("-------------------\n");
		    for(int i=0; i < status.noOfMessages; i++)
		   	    printf("ID:%x\tType: %d\tSize:%d\tAddress:%x\n",status.messageID[i],
		   	    										(status.msgPayloadDetails[i]).type,
		   	    										(status.msgPayloadDetails[i]).size,
		   	    										(status.msgPayloadDetails[i]).addr);
			printf("-------------------\n\n");
			break;
			
		case SEMAPHORES:
			printf("Semaphore Information\n");
			printf("---------------------\n");
		    for(int i=0; i < status.noOfSemaphores; i++)
		   	    printf("ID:%d\tCount:%d\t\tMax Count:%d\n",status.semaphoreID[i],
		   	    									status.count[i],status.maxCount[i]);
			printf("---------------------\n\n");
			break;
			
		case MEMPOOLS:
			printf("Memory Pool Information\n");
			printf("-----------------------\n");
		    for(int i=0; i < status.noOfMemPools; i++)
		   	    printf("ID:%d\tAddress:%x\tBlocks:%d\n",status.memPoolID[i],
		   	    										status.address[i],status.blocks[i]);
			printf("-----------------------\n\n");
			break;
			
		default:
			printf("Unknown Catagory %d - Will not print\n", statusType);
    }
}

/******************************************************************************
 *  Status Run Function (Status's main{})
 */

void
Status::Run()
{
    while (1)
    {
		VDK::PendSemaphore(kPeriodic, 0);	// Can place breakpoint here to halt
											// the application after each update.
		retrieveStatusInformation();
		printStatusInformation(THREADS);
	}
}

/******************************************************************************
 *  Status Error Handler
 */
 
int
Status::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below)  raises
     * a kernel panic and stops the system */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  Status Constructor
 */
 
Status::Status(VDK::Thread::ThreadCreationBlock &tcb)
    : VDK::Thread(tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  Status Destructor
 */
 
Status::~Status()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  Status Externally Accessible, Pre-Constructor Create Function
 */
 
VDK::Thread*
Status::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    	return new (tcb) Status(tcb);
}

/* ========================================================================== */
