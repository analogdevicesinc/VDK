/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"
#include "TMK.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("ErrorHandling")

extern "C" void VDK_CThread_Error(VDK::ThreadID);

namespace VDK {

extern ThreadTable         g_ThreadTable;

void VDK_CThread_Error(ThreadID inThreadID){
// imitate the behaviour of a C++ thread
	TMK_AcquireMasterKernelLock();
	VDK::Thread* pThread = g_ThreadTable.GetObjectPtr( inThreadID );
	TMK_ReleaseMasterKernelLock();
	KernelPanic(kThreadError, pThread->LastErrorType(), pThread->LastErrorValue());
}
}

