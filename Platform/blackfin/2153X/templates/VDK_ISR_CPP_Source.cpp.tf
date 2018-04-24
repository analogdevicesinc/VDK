/* =============================================================================
 *
 *   Description: This is the Interrupt Service Routine called %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"

#include <sys/exception.h>
#pragma file_attr("prefersMemNum=30")
#pragma file_attr("prefersMem=internal")
#pragma file_attr("ISR")

/* define VDK_REENTRANT_ISR macro if nested interrupts are allowed. This
   definition can be done in a global project option if all interrupts allow
   nesting or in a particular interrupt file if there are any interrupts
   in the application that don't allow nesting */

#ifdef VDK_REENTRANT_ISR
EX_REENTRANT_HANDLER(%Name%)
#else
EX_INTERRUPT_HANDLER(%Name%) 
#endif
{
/* Insert your ISR code here */
}

/* ========================================================================== */
