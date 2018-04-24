/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is a C++ implementation for Thread MyThread
 *
 *   Created on:  Monday, May 14, 2001 - 02:11:15 PM
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "MyThread.h"
#include <new>

/******************************************************************************
 *  MyThread Run Function (MyThread's main{})
 */
 
void
MyThread::Run()
{
	VDK::DeviceDescriptor 	inDD = 0;
	VDK::ThreadID			kTheThread = 0;
	bool 					inDestroyNow = false;
	void 					*inCommand = 0;
	char 					*inParameters = "";
	int 					inVal = 0;
	int						inValue = 2;
	VDK::EventData 			inEventData = {0,0,0};
	VDK::Ticks 				inDelay = 0, inPeriod = 0;
	char 					*inFlags = "";
	VDK::Ticks 				inTimeout = 0;
	VDK::Ticks 				inSleepTicks = 0;
	char 					*outBuffer = "";
	unsigned int 			inSize = 0;
	VDK::PoolID				inPoolID;
	void 					*blkPtr;
    unsigned int 	        kBlkSz=16,kNumBlks=8;
	VDK::SemaphoreID	    inSemaphoreID;

	VDK::ClearEventBit(kTheEventBit);
	VDK::ClearThreadError();
	VDK::CloseDevice(inDD);
	VDK::CreateThread(kMyThread);
	VDK::DestroyThread(0, inDestroyNow);
	VDK::DeviceIOCtl (inDD, inCommand, inParameters);
	VDK::DispatchThreadError((VDK::SystemError) 0, inVal);
	VDK::FreeDestroyedThreads();
	VDK::GetEventBitValue(kTheEventBit);
	VDK::GetEventData(kTheEvent);
	VDK::GetEventValue(kTheEvent);
	VDK::GetLastThreadError();
	VDK::GetLastThreadErrorValue();
	VDK::GetPriority(kTheThread);
	VDK::GetSemaphoreValue(kTheSemaphore);
	VDK::GetThreadHandle();
	VDK::GetThreadID();
	VDK::GetThreadStatus(kTheThread);
	VDK::GetThreadType(kTheThread);
	VDK::GetUptime();
	VDK::GetVersion();
	VDK::LogHistoryEvent((VDK::HistoryEnum)0, inValue);
	VDK::LoadEvent(kTheEvent, inEventData);
	VDK::MakePeriodic(kTheSemaphore, inDelay, inPeriod);
	VDK::OpenDevice((VDK::DeviceID)0, inFlags);
	VDK::PendDeviceFlag(kTheDeviceFlag, inTimeout);
	VDK::PendEvent (kTheEvent, inTimeout);
	VDK::PendSemaphore(kTheSemaphore, inTimeout);
	VDK::PopCriticalRegion();
	VDK::PopNestedCriticalRegions();
	VDK::PopNestedUnscheduledRegions();
	VDK::PopUnscheduledRegion();
	VDK::PostDeviceFlag(kTheDeviceFlag);
	VDK::PostSemaphore(kTheSemaphore);
	VDK::PushCriticalRegion();
	VDK::PushUnscheduledRegion();
	VDK::RemovePeriodic(kTheSemaphore);
	VDK::ResetPriority(kTheThread);
	VDK::Sleep(inSleepTicks);
	VDK::SetEventBit(kTheEventBit);
	VDK::SetPriority(kTheThread, (VDK::Priority) 1);
	VDK::SetThreadError((VDK::SystemError) 0, inVal);
	VDK::SyncRead(inDD, outBuffer, inSize, inTimeout);
	VDK::SyncWrite(inDD, outBuffer, inSize, inTimeout);
	VDK::Yield();
	VDK::GetInterruptMask();
	VDK::SetInterruptMaskBits( inValue );
	VDK::ClearInterruptMaskBits( inValue );

	// Memory pool API's
	inPoolID = VDK::CreatePool(kBlkSz,kNumBlks,false);
	blkPtr = VDK::MallocBlock(inPoolID);
	VDK::FreeBlock(inPoolID,blkPtr);
	VDK::LocateAndFreeBlock(blkPtr);
	VDK::GetNumFreeBlocks(inPoolID);
	VDK::GetNumAllocatedBlocks(inPoolID);
	VDK::DestroyPool(inPoolID);	
	inPoolID = VDK::CreatePoolEx(kBlkSz,kNumBlks,false,1);

	// Dynamic creation of semaphores API's
	inSemaphoreID = VDK::CreateSemaphore(1,1,0,0);
	VDK::PendSemaphore(inSemaphoreID,0);
	VDK::PostSemaphore(inSemaphoreID);
	VDK::DestroySemaphore(inSemaphoreID);

	// Dynamic device flags
	VDK::DeviceFlagID devID = VDK::CreateDeviceFlag();
	VDK::DestroyDeviceFlag(devID);

	// Dynamic io objects
	//
	VDK::ioID inID = VDK::CreateIOObject(0);
	VDK::DestroyIOObject(inID);
	
	// Stack API
	VDK::GetThreadStackUsage(VDK::GetThreadID());
}

/******************************************************************************
 *  MyThread Error Handler
 */
 
int
MyThread::ErrorHandler()
{
    /* TODO - Put this thread's error handling code HERE */

    /* The default ErrorHandler (called below) kills the thread */
    return (VDK::Thread::ErrorHandler());
}

/******************************************************************************
 *  MyThread Constructor
 */
 
MyThread::MyThread(VDK::Thread::ThreadCreationBlock &t)
    : VDK::Thread(t)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    // TODO - Put code to be executed when this thread has just been created HERE

}

/******************************************************************************
 *  MyThread Destructor
 */
 
MyThread::~MyThread()
{
    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    // TODO - Put code to be executed just before this thread is destroyed HERE

}

/******************************************************************************
 *  MyThread Externally Accessable, Pre-Constructor Create Function
 */
 
VDK::Thread*
MyThread::Create(VDK::Thread::ThreadCreationBlock &t)
{
    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    return new (_STD nothrow) MyThread(t);
}

