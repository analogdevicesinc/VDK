/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Header for the history buffer
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Implements the history buffer, and time-stamped log that may be
 *  read by the IDDE for trace displays, load plots, etc.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef   VDK_HISTORY_H_
#define   VDK_HISTORY_H_



	// These structs are going to be used as the basis of VDK history logging. 
	// Along with C++ access, we will also be IMPORTing the structures into 
	// assembly files for each processor, so we have to be careful with the 
	// structure's member types and the files we include within this first part 
	// of the header.
	typedef struct HistoryStruct 
    {
		int		m_Time;
		int		m_ThreadID;
        int		m_Type;
        int		m_Value;
    } HistoryStruct;

	typedef struct HistoryBuffer 
    {
		unsigned int   	m_NumEvents;
        unsigned int	m_CurrentEvent;
		HistoryStruct	*m_Events;
    } HistoryBuffer;


	// We do not want the following to be visible to an assembly IMPORT.
#ifdef __cplusplus

#include "TMK.h"

#include "VDK_API.h"

#if (VDK_INSTRUMENTATION_LEVEL_==2)

extern "C" void VDK_History(const VDK::HistoryEnum, const int, const VDK::ThreadID);

#define HISTORY_(event_, value_, threadid_) VDK_History(event_, value_, threadid_)

#else

#define HISTORY_(event_, value_, threadid_) do { /* Do Nothing */ } while(0)

#endif /* VDK_INSTRUMENTATION_LEVEL_==2 */

#endif /* __cplusplus*/

#endif /*  VDK_HISTORY_H_ */


