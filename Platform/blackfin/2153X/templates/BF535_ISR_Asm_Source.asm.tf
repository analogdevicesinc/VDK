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
#if 0
    // Workaround for ADSP-BF535 caching anomaly, which can cause the cache to
    // become inconsistent with memory if an interrupt occurs while stores are
    // pending. Flushing the pipeline here will prevent this. This code is not
    // required for applications which do not enable caching.
    SSYNC;
#endif

/* Insert Your Assembly Code HERE for nesting disabled */

    [--SP] = RETI; /* Pushing the RETI register re-enables nested interrupts */

/* Insert Your Assembly Code HERE for nesting enabled */

    RETI = [SP++]; /* restore pushed RETI contents prior to RTI */

#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000164 "store to load
    // forwarding in write through mode". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the 
    // -wb_wt_fix option
    SSYNC;
#endif
    RTI;
#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000165 "data cache 
    // dirty bit set when a load-miss-fill is in progress". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the 
    // -wb_fix option
    NOP;
    NOP;
#endif
.%Name%.end:

/* ========================================================================== */
