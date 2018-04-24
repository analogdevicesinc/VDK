/* =============================================================================
 *
 *  Description: This is the Interrupt Service Routine called %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "VDK.h"

#pragma file_attr("prefersMemNum=30")
#pragma file_attr("prefersMem=internal")
#pragma file_attr("ISR")

/* 
 * define VDK_REENTRANT_ISR macro if nested interrupts are allowed
 * define VDK_SAVE_RESTORE_40_BITS macro if saving 40 bits of registers 
 *   is required 
 * define VDK_SAVE_RESTORE_SIMD_40_BITS macro if saving 40 bits of registers 
 *   and SIMD registers is required. Note that 32-bits of the SIMD registers 
 *   are saved with pragmas interrupt_complete and interrupt_complete_nesting
 * define VDK_SAVE_RESTORE_LOOP_STACK if it is required to save the loop stack
 *   and empty it before executing the interrupt's body, then restore it before
 *   returning.
 *   
 */

#if defined (VDK_SAVE_RESTORE_LOOP_STACK)
#pragma flush_restore_loop_stack
#endif

#if defined (VDK_REENTRANT_ISR)
  #pragma interrupt_complete_nesting
  #if defined (VDK_SAVE_RESTORE_40_BITS)
    #pragma save_restore_40_bits
  #elif defined (VDK_SAVE_RESTORE_SIMD_40_BITS)
    #pragma save_restore_simd_40_bits
  #endif
#else /* !defined VDK_REENTRANT_ISR */
  #pragma interrupt_complete
  #if defined (VDK_SAVE_RESTORE_40_BITS)
    #pragma save_restore_40_bits
  #elif defined (VDK_SAVE_RESTORE_SIMD_40_BITS)
    #pragma save_restore_simd_40_bits
  #endif
#endif

void %Name%(void) {
/* Insert your ISR code here */
}
/* ========================================================================== */
