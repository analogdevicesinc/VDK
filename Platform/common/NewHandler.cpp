/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <VDK_API.h>
#include "IdleThread.h"
#include "KernelPanic.h"
extern VDK::IdleThread *g_IdleThreadPtr;

namespace VDK {
    void NewHandler(){
	if (!g_IdleThreadPtr -> KillThreadsInPurgatory()) KernelPanic(VDK::kThreadError,VDK::kNewFailure,0);
    }
}
