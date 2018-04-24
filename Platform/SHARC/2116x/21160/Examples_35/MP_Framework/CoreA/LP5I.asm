/* =============================================================================
 *
 *  Description: This is the Interrupt Service Routine called LP5I_Entry
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"

.file_attr prefersMemNum="30";
.file_attr prefersMem=internal;
.file_attr ISR;
.section/dm/doubleany seg_dmda;
/* declare external globals */

/* declare locals */

.section/pm/doubleany seg_pmco;

/******************************************************************************
 * LP5I_Entry
 */

/* ISR entry point */
.GLOBAL     LP5I_Entry;
LP5I_Entry:

/* Insert Assembly Code which needs to run with interrupts disabled HERE */

    BIT CLR LIRPTL 0x00200000;   /* Mask out the interrupt */
    BIT SET MODE1 0x00001000;    /* Re-enable interrupts */

/* Insert Assembly Code which can run with interrupts enabled HERE */
	VDK_ISR_ACTIVATE_DEVICE_(kLink5);

    RTI;
.LP5I_Entry.end:

/* ========================================================================== */
