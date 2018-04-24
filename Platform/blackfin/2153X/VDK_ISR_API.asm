// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The APIs used from the interrupt domain
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * ===========================================================================*/


#include <VDK_ISR_API.h>
#include <sys/platform.h>

#include "asm_offsets.h"
.file_attr ISR;
.file_attr HistoryAndStatus;

#if (VDK_INSTRUMENTATION_LEVEL_==2)

.IMPORT "VDK_History.h";
.EXTERN STRUCT HistoryBuffer _g_History__3VDK; 

///////////////////////////////////////////////////////////////////////////////
.SECTION VDK_ISR_code;
.global         VDK_ISRAddHistoryEvent;
.global			_VDK_History;
.extern			TMK_GetUptime; // assembly entry point - preserves all regs

.SECTION data1;
.var g_pfVDKHistoryLogging = VDK_ISRAddHistoryEvent;
.global g_pfVDKHistoryLogging;

.SECTION VDK_ISR_code;
.ALIGN 2;
_VDK_ISRAddHistoryEvent:
VDK_ISRAddHistoryEvent:

	/*
	 * On entering this subroutine all interrupts are disabled and the args are
	 * in the following registers:
	 *
	 *		enum_ 		= R0
	 *		value_		= R1
	 *		threadID_	= R2
	 *		Tick 		= R3
	 * 
	 *
	 *	The algorithm is:
	 *	{
	 *	    g_History.m_Events[g_History.m_CurrentEvent].m_Time = VDK::GetUptime();
	 *	    g_History.m_Events[g_History.m_CurrentEvent].m_ThreadID = inID;
	 *	    g_History.m_Events[g_History.m_CurrentEvent].m_Type = inHEvent,
	 *	    g_History.m_Events[g_History.m_CurrentEvent].m_Value = inValue;
	 *	    ++g_History.m_CurrentEvent;
	 *	    if (g_History.m_CurrentEvent >= g_History.m_NumEvents)
	 *	        g_History.m_CurrentEvent = 0;
	 *	 }
	 */


    // Save off some variables we're gonna trample. If we change the registers
    // we push onto the stack, we should be aware that the offsets were we get
    // the thread ID, enumeration and value should be changed too
	[--SP] = ASTAT;
	[--SP] = (R7:5,P5:3); // These two instructions decrement SP by 28 bytes
	
    // Find the index into the array: R7 = (g_History.m_CurrentEvent)
	P3.L = _g_History__3VDK;
	P3.H = _g_History__3VDK;
	R7 = [P3 + OFFSETOF(HistoryBuffer,m_CurrentEvent)];
	
	// Index by that amount into the array of entries...
	// i.e. set P5 = g_History.m_Events[g_History.m_CurrentEvent]
#if __VDK_SIZEOF_HistoryStruct_ == 16
    R6 = R7 << 4; // Leave R7 with the current event number
#else
#error The size of HistoryStruct is no longer 16 (now SIZEOF_HistoryStruct)
#endif
	P5 = [P3 + OFFSETOF(HistoryBuffer,m_Events)];
	P4 = R6;
	P5 = P5 + P4;

    // Save the event time: Tick = R3 on entry
	[P5 + OFFSETOF(HistoryStruct,m_Time)] = R3;

    // Save the ThreadID. threadID_	= R2 on entry
	[P5 + OFFSETOF(HistoryStruct,m_ThreadID)] = R2;

    // Save the event type. enum_ = R0 on entry
	[P5 + OFFSETOF(HistoryStruct,m_Type)] = R0;

    // Save the event value. value_	= R1 on entry
	[P5 + OFFSETOF(HistoryStruct,m_Value)] = R1;

    // Increment the event count
	// Note:  P3 == &g_History and R7 == m_CurrentEvent num 
	R7 += 1;
	R6 = [P3 + OFFSETOF(HistoryBuffer,m_NumEvents)];

	// if ( R7 >= R6 ) R7=0;
	CC = R6 <= R7;
	R6 = 0;
	IF CC R7 = R6;

	// Write the incremented value of m_CurrentEvent_
	[P3 + OFFSETOF(HistoryBuffer,m_CurrentEvent)] = R7;

    // Restore some trampled variables
	(R7:5,P5:3) = [SP++];
	ASTAT = [SP++];
	
	[--SP] = RETS;
	CALL.X _UserHistoryLog;
	RETS = [SP++];

    // Return
	RTS;
.VDK_ISRAddHistoryEvent.end:
._VDK_ISRAddHistoryEvent.end:


/* _VDK_History 
* C-callable, ISR safe, intermediate step in the history 
* logging process. This function passes through arguments
* to the history logging function, that is called by
* dereferencing a global subroutine pointer.
*
* 
* Three args will be passed, c-style:
* void VDK_History( HistoryEnum enum, int value, ThreadID threadID); 
*		enum 		= R0
*		value_		= R1
*		threadID_	= R2
*
* This subroutine:
* - disables interrupts
* - gets the current Tick (calls GetUptime)
* - dereferences the history logging pointer
* - re-enables interrupts
*/
_VDK_History:

	// Now prepare to write the history event, save regs which may be trashed
	[--SP] = R4; // used to save interrupt state
	[--SP] = P1; // Can be trashed with calls
	[--SP] = P0; // Used to dereference g_pfVDKISRHistoryLogging
	[--SP] = RETS; // Will be trashed with the calls below
	[--SP] = R3; // Used to pass the Tick count
	[--SP] = R0; // Save the HistoryEnum (trashed with GetUptime)

	// Get the pointer to the history logging function
	P0.L = LO(g_pfVDKHistoryLogging);				
	P0.H = HI(g_pfVDKHistoryLogging);	
	P0 = [P0];
				
	CLI R4; // Disable interrupts

	// Push the arguments and call the logging function
	CALL.X TMK_GetUptime; // Get the last uptime ticks value into R3
	R3 = R0;
	R0 = [SP++]; 
	
	CALL (P0);	// Call the head of the ISR history logging chain.
	
	STI R4; // Re-enable interrupts
		
	// Restore trashed registers
	R3	= [SP++];
	RETS= [SP++];
	P0	= [SP++];
	P1	= [SP++]; 
	R4	= [SP++];
	RTS; 

._VDK_History.end:
  
#endif    /* VDK_INSTRUMENTATION_LEVEL_) */


