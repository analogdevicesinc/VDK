/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <VDK_API.h>
#include "KernelPanic.h"

namespace VDK {
    void TerminateHandler(){
	KernelPanic(VDK::kThreadError,VDK::kUncaughtException,0);
    }
}
