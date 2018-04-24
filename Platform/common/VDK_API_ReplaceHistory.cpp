/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description:  Implementation of public API ReplaceHistorySubroutine that
 *   allows customers to have their own history functions.
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"

#include "VDK_API.h"
#include "VDK_History.h"
#include "VDK_AsmMacros.h"
#include "vdk_macros.h"
#include "KernelPanic.h"

#pragma file_attr("HistoryAndStatus")

#if (VDK_INSTRUMENTATION_LEVEL_==2)
namespace VDK
{
    extern "asm" VDK::HistoryLogging g_pfVDKHistoryLogging;

    void
    ReplaceHistorySubroutine(VDK::HistoryLogging newRoutine)
    {
        // Globally disabling interrupts. I'm not sure that this is needed,
        // but adding it to make sure.
        TMK_Lock histLock;
        TMK_LockState state = TMK_AcquireGlobalInterruptLock(&histLock);

        g_pfVDKHistoryLogging = newRoutine;

        TMK_ReleaseGlobalInterruptLock(&histLock, state);
    }
}
#endif
