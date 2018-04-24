/* =============================================================================
 *
 *  Description: This is the Interrupt Service Routine called %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

/*
 * define VDK_SAVE_RESTORE_40_BITS macro before the header file inclusion if 
 * the VDK ISR macros which save 40 bits of registers are required in this
 * ISR
 */

#include "VDK.h"

.file_attr prefersMemNum="30";
.file_attr prefersMem=internal;
.file_attr ISR;
.section/dm/doubleany seg_dmda;

/* declare external globals */

/* declare locals */

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.section/sw/doubleany seg_swco;
#else
.section/pm/doubleany seg_pmco;
#endif

/******************************************************************************
 * %Name%
 */

/* ISR entry point */
.GLOBAL     %Name%;
%Name%:

#if 0
/*
   For the ADSP-2116x, ADSP-2126x, ADSP-213xx and ADSP-214xx processor 
   families the status stack is automatically pushed and popped for the 
   following interrupts : 
   IRQx, VIRPT and the Timer interrupt. For all other interrupts this should be 
   done manually in the ISR by commenting out the instructions below. If 
   interrupt nesting is used then note that (a) the instruction to enable 
   interrupts cannot be placed between the push and pop instructions as MODE1 
   is one of the registers that are saved and restored, (b) it must be ensured 
   that the status stack is not overflowed. See the relevant hardware reference
   manual for further details.
*/

    PUSH STS;
    NOP;
#endif

#if 0
/*
    It is recommended to switch to the alternate data registers to protect the
    primary registers from trashing of the extended 40-bit data. The secondary
    registers should never be in use for 40-bit computation and are therefore
    safe. Note that any of the secondary registers that are used in the ISR must
    still be saved, and restored prior to RTI, but it is only necessary to save
    them as 32bit.

    The original register selection will be restored either by the explicit POP STS
    or - for IRQ0-3 - by the automatic status stack pop performed by RTI. Note
    that it can't be assumed that the primary registers were selected at the point
    of interrupt.
*/


    BIT SET MODE1 (SRRFH | SRRFL); /* select secondary data registers */
    NOP;
#endif

/* Insert Assembly Code which needs to run with interrupts disabled HERE */

#if 0
/* 
    If the status stack was pushed earlier in the ISR it needs to be popped 
*/
    POP STS;
    NOP;
#endif
   
    BIT SET MODE1 0x00001000;    /* Re-enable interrupts */

/* Insert Assembly Code which can run with interrupts enabled HERE */

    RTI;
.%Name%.end:

/* ========================================================================== */
