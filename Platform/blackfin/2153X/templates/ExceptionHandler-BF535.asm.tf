/* =============================================================================
 *
 *  Description: This is the exception handler.
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
.extern _ExceptionPanic;

/* declare locals */

.section/doubleany L1_code;

/******************************************************************************
 * UserExceptionHandler
 */

/* User Exception Handler entry point */
.GLOBAL     UserExceptionHandler;
UserExceptionHandler:


#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000164 "store to load
    // forwarding in write through mode". This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_wt_fix option
    SSYNC;
#endif

    /**
     * Handle the exception...
     *
     * Not all hardware exceptions reach UserExceptionHandler.
     * VDK reserves the user exception 0, which is handled by the VDK exception
     * handler.
     *
     * Certain exceptions are handled by the runtime libraries cplb manager.
     * Exceptions currently passed by VDK to the cplb manager:
     *   0x23 Data access CPLB protection violation
     *   0x26 Data CPLB miss
     *   0x2C Instruction CPLB miss
     *
     * It is your responsibility to handle any other exception (user or system)
     * here.
     */

    /**
     * If any exception is handled any registers used either
     * directly or indirectly must be saved and restored.
     * We do not save and restore the registers used in the call to
     * ExceptionPanic because the application will halt and not continue
     */

    /**
     * Store EXCAUSE in R2 as the value for the panic
     * The .message directive removes an informational message related to
     * one cycle stall
     */

    .message/suppress 1056 for 2 lines;
    R2 = SEQSTAT;
    R2 <<= 26;
    R2 >>= 26;

    R1.H = HI(_VDK_kUnhandledExceptionError_);
    R1.L = LO(_VDK_kUnhandledExceptionError_);
    R0 = _VDK_kUnhandledException_;
    CALL.X _ExceptionPanic;
	RTX;

#if 0
    // Workaround for ADSP-BF535 caching anomaly number 05000165 "data cache
    // dirty bit set when a load-miss-fill is in progress. This code is not
    // required for applications which do not enable caching.
    // If this code needs to be in, the file should be assembled with the
    // -wb_fix option
    NOP;
    NOP;
#endif

.UserExceptionHandler.end:

/* ========================================================================== */
