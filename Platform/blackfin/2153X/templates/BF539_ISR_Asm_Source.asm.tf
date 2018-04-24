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
#if defined (__SILICON_REVISION__) && (__SILICON_REVISION__ < 0x4 || __SILICON_REVISION__ == 0xffff)
     // Workaround for anomaly 05000283 
     // A system MMR write is stalled indefinitely when killed in a particular 
     // stage
     // The workaround is to reset the MMR logic with another killed MMR 
     // access that does not have other side effects on the application
     // No breakpoints should be placed before the wa_05000283_skip label

    // This anomaly affects silicon revisions prior to 0.4

    [--SP] = P0;
    [--SP] = R0;
    [--SP] = ASTAT;
     CC = R0 == R0;    // always true
    P0.H = 0xffc0;    // MMR space CHIPID
    P0.L = 0x0014;
     IF CC JUMP .wa_05000283_skip;  // always skip MMR access, however, 
                                    // MMR access will be fetched and killed
    R0 = [P0];     // bogus MMR read to work around the anomaly
.wa_05000283_skip:
    ASTAT = [SP++];
    R0 = [SP++];
    P0 = [SP++];
#endif

#if defined (__SILICON_REVISION__) && (__SILICON_REVISION__ < 0x3 || __SILICON_REVISION__ == 0xffff)
    // Workaround for ADSP-BF539 anomaly 05000257.
    // Reload the loop counter registers to force the clearing of the loop
    // buffers, and hence prevent the wrong fetch address from being sent
    // to the instruction fetch unit if returning into a short hardware loop.

    // This anomaly affects silicon revisions prior to 0.3

    [--SP] = LC0;
    [--SP] = LC1;
    LC1 = [SP++];
    LC0 = [SP++];
#endif

/* Insert Your Assembly Code HERE for nesting disabled */

    [--SP] = RETI; /* Pushing the RETI register re-enables nested interrupts */

/* Insert Your Assembly Code HERE for nesting enabled */

    RETI = [SP++]; /* restore pushed RETI contents prior to RTI */

    RTI;
.%Name%.end:

/* ========================================================================== */
