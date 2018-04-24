/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: History buffer functions
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#if (VDK_INSTRUMENTATION_LEVEL_==2)

#include <VDK_History.h>
#include <VDK_API.h>
#include <VDK_AsmMacros.h>
#pragma file_attr("HistoryAndStatus")

namespace VDK {
	extern HistoryBuffer 		g_History;			// in vdk.cpp
}


extern "C" void UserHistoryLog(const VDK::HistoryEnum,const int,const VDK::ThreadID);

#ifdef  __ADSP21000__
  void SharcUserHistoryLog(const VDK::HistoryEnum,const int,const VDK::ThreadID)
  {
        asm(".EXTERN _UserHistoryLog;");
        asm("CALL _UserHistoryLog;");
  }
#endif







#endif /* VDK_INSTRUMENTATION_LEVEL==2 */



