/* =============================================================================
 *
 *
 *   Description: This module implements the interrupt service routine which
 *				  is invoked whenever a CB7 (circular buffer 7 overflow)
 *				  interrupt occurs.
 *
 *				  The SHARC C runtime environment uses circular buffer 7 (i.e. 
 *				  registers I7, B7 and L7) to implement the C runtime stack,
 *				  and VDK context-switches these registers (amongst others)
 *				  between each thread's own stack.
 *
 *				  Any attempt by thread code to MODIFY the I7 (stack pointer)
 *				  register to a location outside the address range defined by
 *				  B7 (stack base) and L7 (stack length) will cause the I7 to
 *				  "wrap around" according to the normal DAG modulo addressing.
 *				  It will also cause the CB7 interrupt flag to be asserted.
 *
 *				  Normally this interrupt is masked (ignored) but when unmasked
 *				  (as in this example) it will invoke its service routine.
 *				  All the service routine does here is to Post the kOverflow
 *				  semaphore and return. Posting this semaphore will wake up
 *				  the high-priority 'Waiter' thread, which will then take some
 *				  appropriate action to deal with and/or report the erring thread.
 *
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
 * CB7I_Entry
 *
 * This is the handler for the CB7 (circular buffer 7 overflow) interrupt.
 *
 * It will be invoked whenever a thread function tries to allocate a stack
 * frame which extends below the base of the current stack (defined by the
 * B7 register).
 *
 * The only action taken is to post the 'Overflow' semaphore, this will
 * alert the high-priority Waiter thread which will then take any necessary
 * action.
 */

/* ISR entry point */
.GLOBAL     CB7I_Entry;
CB7I_Entry:
	/* 
	 * Mask off the CB7 interrupt, in case it remains asserted after
	 * the ISR returns (as on the 21160 simulator) which would cause
	 * the ISR to be continuously re-entered.
	 */
	BIT CLR IMASK CB7I;				    // mask the CB7 interrupt

	/* Post the overflow semaphore */
	VDK_ISR_POST_SEMAPHORE_(kOverflow);

	/* and return from the interrupt */
	BIT SET MODE1 IRPTEN;
    RTI;
.CB7I_Entry.end:

