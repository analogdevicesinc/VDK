#include "VDK.h"

.file_attr prefersMemNum="30";
.file_attr prefersMem=internal;
.file_attr ISR;
.file_attr HistoryAndStatus;

.section/doubleany L1_code;

.global 		_CustomHistoryLog;
_CustomHistoryLog:

    /* Add a history event to the history buffer.
     *
     * On entry:
     *      Interrupts are globally disabled
     *      R3 contains the current Tick
     *      R2 contains the threadID
     *      R1 contains the history Value
     *      R0 contains the HistoryEnum
     *
     */

	// Save off any registers used by this subroutine
	[--SP] = RETS;
	[--SP] = R3;
	// P1 can be modified if the call gets transformed onto a long jump
	[--SP] = P1; 
	
	// R3 currently contains the VDK::Tick count for this event
	// Replace this value with the current cycle count
	R3 = CYCLES;
	
	// Log the event
	CALL.X VDK_ISRAddHistoryEvent;
		
	// Restore any registers used by this subroutine
	P1 = [SP++];
	R3 = [SP++];
	RETS = [SP++];
	
	// Return
	RTS;
._CustomHistoryLog.end:
