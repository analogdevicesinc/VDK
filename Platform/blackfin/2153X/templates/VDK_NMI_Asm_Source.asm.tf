/* =============================================================================
 *
 *   Description: This is the NMI Service Routine called %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"

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
    // Workaround for ADSP-BF533, ADSP-BF532, ADSP-BF531, ADSP-BF561
    // MMR anomaly 05000157 .A killed 32-bit System MMR write will lead to the
    // next system MMR access thinking it should be 32-bit. Loading any other
    // MMR first (CHIPID in this case) will prevent the problem.
    [--SP] = P0;
    [--SP] = R0;
    P0.H = 0xffc0;
    P0.L = 0x0014;
    R0 = [P0];
    R0 = [SP++];
    P0 = [SP++];
#endif
#if 0
    // Workaround for ADSP-BF535 caching anomaly, which can cause the cache to
    // become inconsistent with memory if an interrupt occurs while stores are
    // pending. Flushing the pipeline here will prevent this. This code is not
    // required for applications which do not enable caching.
    SSYNC;
#endif

/* Insert Your Assembly Code HERE */
/* Do *not* call any VDK functions or macros */

#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000164 "store to load
    // forwarding in write through mode". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_wt_fix option
    SSYNC;
#endif
    RTN;
#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000165 "data cache
    // dirty bit set when a load-miss-fill is in progress. This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_fix option
    NOP;
    NOP;
#endif
.%Name%.end:

/* ========================================================================== */
