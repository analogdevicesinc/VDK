/* =============================================================================
 *
 *  Description: This is the user exception handler.
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "VDK.h"

#include <sys/exception.h>
#include <builtins.h>
#include <sysreg.h>

#pragma file_attr("prefersMemNum=30")
#pragma file_attr("prefersMem=internal")
#pragma file_attr("ISR")




/******************************************************************************
 * UserExceptionHandler
 */

extern "asm" {
#pragma section("L1_code")
EX_EXCEPTION_HANDLER(UserExceptionHandler)
{
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

	ExceptionPanic(VDK::kUnhandledException,VDK::kUnhandledExceptionError,sysreg_read(reg_SEQSTAT) & 0x3F);
}
}


/* ========================================================================== */

