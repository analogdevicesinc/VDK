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
#if 1
    // Workaround for anomaly 05000283 
    // A system MMR write is stalled indefinitely when killed in a particular 
    // stage
    // The workaround is to reset the MMR logic with another killed MMR 
    // access that does not have other side effects on the application
    // No breakpoints should be placed before the wa_05000283_skip label
    // This anomaly affects all silicon revisions to date (0.2 0.3 0.4 0.5).
    // If your silicon is a later revision than those mentioned above, please 
    // check your anomaly sheet to see if that revision is affected.

    [--SP] = P0;
    [--SP] = R0;
    [--SP] = ASTAT;
    CC = R0 == R0;    // always true
    P0.H = 0xffc0;    // MMR space CHIPID
    P0.L = 0x0014;
    IF CC JUMP .wa_05000283_skip;  // always skip MMR access, however, 
                                   // MMR access will be fetched and killed

#if defined (__SILICON_REVISION__) && (__SILICON_REVISION__ > 0x3 || __SILICON_REVISION__ == 0xffff)                                  
    // Workaround for anomaly 05000428
    // Lost/Corrupted Write to L2 Memory Following Speculative Read by 
    // Core B from L2 Memory.
    // The workaround is to ensure that no potential L2 speculative read 
    // is performed within the shadow of a branch by Core B.
    // This anomaly affects BF561 silicon revisions 0.4 and 0.5. If your 
    // silicon is a later revision than those mentioned above, please 
    // check your anomaly sheet to see if that revision is affected.

    // This bogus MMR read to work around 05000283 will cause a 05000428 
    // warning. The warning is not valid as the read is from MMR space 
    // and not from L2 memory.
.MESSAGE/SUPPRESS 5517 FOR 1 LINES;  //Suppress 05000428 warning 
    R0 = [P0];      // Bogus MMR read to work around 05000283.  
    NOP;   // Two NOPs are required to protect the following from 
    NOP;   // speculative reads. 
#else
    R0 = [P0];      // Bogus MMR read to work around 05000283. 
#endif

.wa_05000283_skip:
    ASTAT = [SP++];
    R0 = [SP++];
    P0 = [SP++];
#endif

#if defined (__SILICON_REVISION__) && (__SILICON_REVISION__ < 0x5 || __SILICON_REVISION__ == 0xffff)
    // Workaround for anomaly 05000257.
    // Reload the loop counter registers to force the clearing of the loop
    // buffers, and hence prevent the wrong fetch address from being sent
    // to the instruction fetch unit if returning into a short hardware loop.

    // This anomaly affects silicon revisions prior to 0.5

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
