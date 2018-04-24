/* =============================================================================
 *
 *   Description: This is the Interrupt Service Routine called %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"
.file_attr prefersMemNum="30";
.file_attr prefersMem="internal";
.file_attr ISR;

.section/doubleany data1;
/* declare external globals */

/* declare locals */

.section/doubleany program;

/******************************************************************************
 * %Name%
 */

/* ISR entry point */
.GLOBAL     %Name%;
%Name%:

/* Insert Your Assembly Code HERE for nesting disabled */

    [--SP] = RETI; /* Pushing the RETI register re-enables nested interrupts */

/* Insert Your Assembly Code HERE for nesting enabled */

    RETI = [SP++]; /* restore pushed RETI contents prior to RTI */

    RTI;
.%Name%.end:

/* ========================================================================== */
