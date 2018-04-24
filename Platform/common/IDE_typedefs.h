// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#ifndef __IDE_TYPEDEF_H__
#define __IDE_TYPEDEF_H__

#include "VDK_Thread.h"
#include "Semaphore.h"
#include "Message.h"
#include "IDElement.h"
#include "VDK_MemPool.h"
#include <asm_offsets.h>


typedef VDK::MemoryPool				IDE_MemoryPool;
typedef VDK::Thread 				IDE_Thread;
typedef VDK::Semaphore 				IDE_Semaphore;
typedef VDK::Message 				IDE_Message;
typedef VDK::DeviceFlag 			IDE_DeviceFlag;
typedef VDK::IDElement				IDE_Element;
typedef VDK::MsgChannel				IDE_MsgChannel;
typedef VDK::ThreadStatus			IDE_ThreadStatus;
typedef VDK::Priority				IDE_Priority;


// Pointer types
//
IDE_Thread*  			IDE_ThreadPtr;
IDE_Semaphore*			IDE_SemaphorePtr;
IDE_Message*			IDE_MessagePtr;
IDE_Element* 			IDE_IDElementPtr;
IDE_MemoryPool*			IDE_MemoryPoolPtr;
IDE_DeviceFlag* 		IDE_DeviceFlagPtr;
IDE_MsgChannel*			IDE_MsgChannelPtr;
IDE_ThreadStatus*		IDE_ThreadStatusPtr;
IDE_Priority*			IDE_PriorityPtr;

// ID Types 
//
typedef VDK::SemaphoreID 	IDE_SemaphoreID;
typedef VDK::EventID	 	IDE_EventID;
typedef VDK::EventBitID	 	IDE_EventBitID;
typedef VDK::DeviceFlagID 	IDE_DeviceFlagID;
typedef VDK::PoolID 		IDE_MemoryPoolID;
typedef VDK::ThreadID 		IDE_ThreadID;

IDE_SemaphoreID*		IDE_SemaphoreIDPtr;
IDE_EventID*			IDE_EventIDPtr;
IDE_EventBitID*			IDE_EventBitIDPtr;
IDE_DeviceFlagID*		IDE_DeviceFlagIDPtr;
IDE_MemoryPoolID*		IDE_MemoryPoolIDPtr;
IDE_ThreadID*			IDE_ThreadIDPtr;

// Below variable is used  ONLY by IDDE to determine the
// m_PendingThreads offset.
//
// Note: if threads inheritance order changes check for any
// possible errors.
// TODO: check if any other way to solve
//
namespace VDK
{
int g_Sem_ThreadBaseOffset =  __VDK_SIZEOF_IDElement_; 
}


//(IDE_Thread*)(VDK::g_ThreadTable.IDTableMgr.m_IDTable[n].m_ObjectPtr

#endif /* __IDE_TYPEDEF_H__ */


