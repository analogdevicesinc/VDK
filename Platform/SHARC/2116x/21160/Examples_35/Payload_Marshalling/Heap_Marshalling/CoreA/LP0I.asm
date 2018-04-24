/* =============================================================================
 *
 *  Description: This is the Interrupt Service Routine called LP0I_Entry
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"

.section/dm seg_dmda;
/* declare external globals */

/* declare locals */

.section/pm seg_pmco;

/******************************************************************************
 * LP0I_Entry
 */

/* ISR entry point */
.GLOBAL     LP0I_Entry;
LP0I_Entry:

/* Insert Assembly Code which needs to run with interrupts disabled HERE */

    BIT CLR LIRPTL 0x00010000;   /* Mask out the interrupt */
    BIT SET MODE1 0x00001000;    /* Re-enable interrupts */

/* Insert Assembly Code which can run with interrupts enabled HERE */
	VDK_ISR_ACTIVATE_DEVICE_(kLink0);
	
    RTI;
.LP0I_Entry.end:

/* ========================================================================== */
