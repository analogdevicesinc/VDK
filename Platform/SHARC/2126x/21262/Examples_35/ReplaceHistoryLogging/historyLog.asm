#include "VDK.h"

.file_attr prefersMemNum="30";
.file_attr prefersMem=internal;
.file_attr ISR;
.file_attr HistoryAndStatus;

// declare variables we need
.segment/DM     seg_dmda;
.var            _history_Ticks;

.segment/PM     seg_pmco;
.extern			VDK_ISRAddHistoryEvent;

.global _CustomHistoryLog;

_CustomHistoryLog:
	
	/* Add a history event to the history buffer.
	 *
	 * On entry: 
	 *		Interrupts are globally disabled	
	 *		R0 contains the current Tick
	 *		R12 contains the threadID	
	 *		R4 contains the HistoryEnum
	 *		R8 contains the history Value
	 *
	 */

	// Save off any registers used by this subroutine
	DM(_history_Ticks) = R0;

	// R0 currently contains the VDK::Tick count for this event
	// Replace this value with the current cycle count
	R0 = EMUCLK;
	
	// Log the event
	CALL VDK_ISRAddHistoryEvent;
	
	// Restore any registers used by this subroutine
	R0 = DM(_history_Ticks);
	
	RTS;

._CustomHistoryLog.end:

.endseg;


