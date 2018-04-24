// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for source code formatting
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


#include <VDK_ISR_API.h>
#include "asm_offsets.h"
#include "ADI_attributes.h"
.file_attr ISR;
.file_attr HistoryAndStatus;


#if VDK_INSTRUMENTATION_LEVEL_ == 2

///////////////////////////////////////////////////////////////////////////////
// Add an entry to the history buffer
///////////////////////////////////////////////////////////////////////////////

// declare some variables we are going to need
.segment/DM     seg_dmda;
.var            _history_I2__3VDK;
.var            _history_Enum__3VDK;
.var            _history_Value__3VDK;
.var            _history_ASTAT__3VDK;
.var            _history_PCTOP__3VDK;

.var			VDK_History_I13;
.var			VDK_History_M13;
.var			VDK_History_R0;
.var			VDK_History_PCTOP;

.extern         _g_History__3VDK;
.endseg;


#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM     seg_pmco;
#endif

.IMPORT "VDK_History.h";
.EXTERN STRUCT HistoryBuffer _g_History__3VDK; 
.extern         _UserHistoryLog;
.extern         TMK_GetUptime;

#ifdef __SIMDSHARC__
.SWF_OFF;				// Code has been hand-checked for SWF anomaly cases
#endif

.global         VDK_ISRAddHistoryEvent;
.global         _VDK_History;
.global         _VDK_ASM_History;

.segment/DM     seg_dmda;
.var g_pfVDKHistoryLogging = VDK_ISRAddHistoryEvent;
.global g_pfVDKHistoryLogging;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM     seg_pmco;
#endif

VDK_ISRAddHistoryEvent:
	
	/* Add a history event to the history buffer.
	 *
	 * On entry: 
	 *		Interrupts are globally disabled	
	 *		R0 contains the current Tick
	 *		R12 contains the threadID	
	 *		R4 contains the HistoryEnum
	 *		R8 contains the history Value
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
	
	// Call UserHistoryLog and save off some variables we're gonna trample
	// using the delay slots
 	DM(_history_I2__3VDK) = I2;
	DM(_history_ASTAT__3VDK) = ASTAT;
	DM(_history_PCTOP__3VDK) = PCSTK;
	POP PCSTK;
	
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so
    // changing code to a simple call
	// Call UserHistoryLog and save some vars in the delay slots.
	DM(_history_Enum__3VDK) = R4;
    DM(_history_Value__3VDK) = R8;
	CALL _UserHistoryLog;
    
    // No more calls needed, so restore the PC stack
    PUSH PCSTK;
    PCSTK = DM(_history_PCTOP__3VDK);
	
    // Find the right place in memory to point
    R4 = DM(_g_History__3VDK + OFFSETOF(HistoryBuffer, m_CurrentEvent));
#if __VDK_SIZEOF_HistoryEvent_ == 4
    R4 = LSHIFT R4 BY 2;
#else
#error The size of HistoryEvent/Struct is no longer 4 (now SIZEOF_HistoryEvent)
#endif
    R8 = DM(_g_History__3VDK + OFFSETOF(HistoryBuffer, m_Events));
    R8 = R8 + R4;

    // Save the ThreadID - stored in R12
    I2 = R8;
    DM(OFFSETOF(HistoryStruct, m_ThreadID), I2) = R12;
	
	// The event time is already stored in R0
    DM(OFFSETOF(HistoryStruct, m_Time), I2) = R0;

    // Save the event type
    R4 = DM(_history_Enum__3VDK);
    DM(OFFSETOF(HistoryStruct, m_Type), I2) = R4;

    // Save the event value
    R4 = DM(_history_Value__3VDK);
    DM(OFFSETOF(HistoryStruct, m_Value), I2) = R4;

    // Increment the event count
    R8 = DM(_g_History__3VDK + OFFSETOF(HistoryBuffer, m_CurrentEvent));
    R8 = R8 + 1;
    R4 = DM(_g_History__3VDK + OFFSETOF(HistoryBuffer, m_NumEvents));
    R4 = R4 - R8;
    IF LE R8 = R8 XOR R8;
    DM(_g_History__3VDK + OFFSETOF(HistoryBuffer, m_CurrentEvent)) = R8;

    // Return
    ASTAT = DM(_history_ASTAT__3VDK);
    I2 = DM(_history_I2__3VDK);
    
    RTS(DB);
    R4 = DM(_history_Enum__3VDK);
    R8 = DM(_history_Value__3VDK);

.VDK_ISRAddHistoryEvent.end:

.endseg;


#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM     seg_pmco;
#endif
/* _VDK_History 
* 
* ISR safe, intermediate step in the history 
* logging process. This function passes through arguments
* to the history logging mechanism, called by
* dereferencing a global subroutine pointer.
*
* For Blackfin and TigerSHARC this subroutine is callable from C and asm
* but due to the difference in calling conventions between C and asm 
* for SHARC we have two intermediate subroutines - one for C and one for asm. 
* This is the C function.
* 
* Three args should be passed to this subroutine, c-style:
* void VDK_History( HistoryEnum enum, int value, ThreadID threadID); 
*		enum 		= R4
*		value_		= R8
*		threadID_	= R12
*
* This function/subroutine:
* - As it is only called from C, we're only going to save and
*		restore what we need to.
* - disables interrupts
* - gets the current Tick (calls GetUptime)
* - dereferences the history logging pointer
* - re-enables interrupts
* 
* 
*/
_VDK_History:

	// C Prologue
	MODIFY (I7, 0xFFFFFFFE);
	
	R1 = MODE1;		// ***R1 not saved and restored***
	// Disable interrupts.
	// Make sure these instructions are executed sequentually
	JUMP(PC, .SH_INT_DISABLED_1) (DB);
	BIT CLR MODE1 IRPTEN;
	NOP;
.SH_INT_DISABLED_1:

	// Get the Uptime. Use the delay slots to save some used registers.
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so
    // changing code to a simple call
	I13 = DM(g_pfVDKHistoryLogging);
	CALL TMK_GetUptime ; // will only change r0
	
	// Call the head of the history logging chain.
	CALL (M13, I13);
	
	// Restore registers and interrupts.
	MODE1 = R1;
	
	// C Epilogue
	I12=DM(M7,I6);
	JUMP (M14,I12) (DB);
	I7=I6;
	I6=DM(0,I6);
	
._VDK_History.end:	

/* _VDK_ASM_History 
* 
* ISR safe, intermediate step in the history 
* logging process. This function passes through arguments
* to the history logging mechanism, called by
* dereferencing a global subroutine pointer.
*
* For Blackfin and TigerSHARC this subroutine is callable from C and asm
* but due to the difference in calling conventions between C and asm with SHARC 
* we have two subroutines - one for C and one for asm. 
* This is the ASM function.
* 
* Three args should be passed to this subroutine, c-style:
* void VDK_ASM_History( HistoryEnum enum, int value, ThreadID threadID); 
*		enum 		= R4
*		value_		= R8
*		threadID_	= R12
*/

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM     seg_pmco;
#endif
_VDK_ASM_History:
	
	R1 = MODE1;		// ***R1 not saved and restored***
	// Disable interrupts.
	// Make sure these instructions are executed sequentually
	JUMP(PC , .SH_INT_DISABLED_2) (DB);
	BIT CLR MODE1 IRPTEN;
	NOP;
.SH_INT_DISABLED_2:
	
	// We need to save the top of the PCSTK before making any further calls.
	// There is no danger of causing an underflow as this subroutine is called
	// so there will be at least 1 entry in the PCSTK.
	DM(VDK_History_PCTOP) = PCSTK;
	POP PCSTK;
	
	DM(VDK_History_M13) = M13;
	M13 = 0;

	// Save registers & get the Uptime.
    // Because of anomaly 15000011 on Phoenix, we cannot use call(db) so
    // changing code to a simple call
	DM(VDK_History_R0) = R0; // Used for the 4th 'argument'
	DM(VDK_History_I13) = I13; // Used to dereference the logging pointer
	CALL TMK_GetUptime ; // will only change r0
	
	I13 = DM(g_pfVDKHistoryLogging);
	
	// Call the head of the history logging chain.
	CALL (M13, I13);
	
	PUSH PCSTK;
	PCSTK = dm(VDK_History_PCTOP);
	
	// Restore registers and interrupt state.
	M13 = DM(VDK_History_M13);
	I13 = DM(VDK_History_I13);
	MODE1 = R1;
	R0 = DM(VDK_History_R0); // Using 1 cycle latency to restore R0
	
	// return
	RTS;
	
._VDK_ASM_History.end:	

.endseg;
#endif    


