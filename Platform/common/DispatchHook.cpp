/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"
#include "VDK_History.h"
#pragma file_attr("OS_Internals")

namespace VDK {

TMK_PFDispatch g_pfDispatchLoggingChain;

void DispatchLoggingHookFn(void)
{
    // We've just transitioned into kernel level, so log this as an event
    HISTORY_(VDK::kThreadSwitched, (VDK::ThreadID)VDK_KERNEL_LEVEL_, GetThreadID());

    // Chain to the kernel-level dispatcher/scheduler
    (*g_pfDispatchLoggingChain)();

    // We're on our way back to the original thread, i.e. no re-
    // -scheduling has taken place.
    HISTORY_(VDK::kThreadSwitched, GetThreadID(), (VDK::ThreadID)VDK_KERNEL_LEVEL_);
}


}	// namespace VDK
