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
 */

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

#if 0
/* Workaround for ADSP-2137x anomaly 09000015 
 * Incorrect Popping of stacks possible when exiting IRQx/Timer 
 * Interrupts with DB modifiers
 *
 * This anomaly applies only to interrupts IRQ0, IRQ1, IRQ2 and TMZHI. 
 * If the ISR you are creating is for IRQ0, IRQ1, IRQ2 and TMZHI please
 * uncomment this pragma. For any other ISRs the pragma is not required.
 *
 * This anomaly affects all silicon revisions to date (0.0).
 * If your silicon is a later revision than the mentioned above, please
 * check your anomaly sheet to see if that revision is affected.
 */ 
#pragma no_db_return
#endif
void %Name%(void) {
/* Insert your ISR code here */
}
/* ========================================================================== */
