/* =============================================================================
 *
 *   Description: This is the Interrupt Service Routine called EVT_IVG12_Entry
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"
#include <defBF561.h>

#ifndef COREB
#define SIC_ISR0   SICA_ISR0		// SIC Interrupt Status register 0
#define SIC_ISR1   SICA_ISR1      	// SIC Interrupt Status register 1
#define SIC_IMASK0 SICA_IMASK0      // SIC Interrupt Mask register 0
#define SIC_IMASK1 SICA_IMASK1      // SIC Interrupt Mask register 1
#else
#define SIC_ISR0   SICB_ISR0		// SIC Interrupt Status register 0
#define SIC_ISR1   SICB_ISR1      	// SIC Interrupt Status register 1
#define SIC_IMASK0 SICB_IMASK0      // SIC Interrupt Mask register 0
#define SIC_IMASK1 SICB_IMASK1      // SIC Interrupt Mask register 1
#endif

#if 0
#define HI(x) (((x) >> 16) & 0xFFFF)
#define LO(x) ( (x)        & 0xFFFF)
#endif

.section/doubleany data1;
/* declare external globals */

/* declare locals */

.section/doubleany program;

/******************************************************************************
 * EVT_IVG12_Entry
 */

/* ISR entry point */
.GLOBAL     EVT_IVG12_Entry;
EVT_IVG12_Entry:
#if 0
    // Workaround for ADSP-BF535 caching anomaly, which can cause the cache to
    // become inconsistent with memory if an interrupt occurs while stores are
    // pending. Flushing the pipeline here will prevent this. This code is not
    // required for applications which do not enable caching.
    SSYNC;
#endif

#if 0
    // Workaround for ADSP-BF535 CLI anomaly, which may cause the ISR to be
    // entered with interrupts already disabled. This workaround is only
    // needed if the ISR is to support nesting (i.e. interruption by
    // higher-priority interrupts).
    [--SP] = R7;
    [--SP] = ASTAT;

    // Check if IMASK is zero to detect the anomaly condition.
    CLI R7;
    CC = R7 == 0;
    IF !CC JUMP .imaskNotZero;

    // IMASK is currently zero, so we need to defer the ISR by
    // re-raising the interrupt and returning. The ISR will be re-entered
    // as soon as interrupts are re-enabled (at thread level) by an STI
    // instruction. Note that we don't need to do STI R7 as this would
    // be a no-op since R7 is zero.

    // If this ISR is for a core interrupt, then the interrupt number must
    // be explicitly re-raised here before returning. Conversely, if the
    // ISR is for an off-core (i.e. system) interrupt then the interrupt
    // is still active, hence the RAISE instruction is not required and
    // can be removed.
    RAISE 12 /* replace this comment with core interrupt number */;

    ASTAT = [SP++];
    R7 = [SP++];
    RTI;

.imaskNotZero:  // not an anomalous condition, so we can continue
    STI R7;
    ASTAT = [SP++];
    R7 = [SP++];

    [--SP] = RETI; /* Pushing the RETI register re-enables nested interrupts */

/* Insert Your Assembly Code HERE for nesting enabled */

    RETI = [SP++];
#else

/* Insert Your Assembly Code HERE for nesting disabled */
	[--SP] = (R7:0,P5:0);
	R0 = ASTAT;
	
	P5.L = LO(SIC_ISR1);
	P5.H = HI(SIC_ISR1);
	R7 = [P5];
	
	CC = BITTST(R7, 25);
	
	if !CC JUMP .notIDMA0;
#ifdef COREA
	P4.L = LO(IMDMA_S0_IRQ_STATUS);
	P4.H = HI(IMDMA_S0_IRQ_STATUS);
#else
	P4.L = LO(IMDMA_D0_IRQ_STATUS);
	P4.H = HI(IMDMA_D0_IRQ_STATUS);
#endif
	R6 = 0x1;
	R5 = W[P4](Z);
	
	CC = BITTST(R5,0);
	if !CC JUMP .notIDMA0;
	
	W[P4] = R6;

	VDK_ISR_ACTIVATE_DEVICE_(kIMDMA0);
	
.notIDMA0:
	CC = BITTST(R7, 26);
	
	if !CC JUMP .notIDMA1;
	
#ifdef COREA
	P4.L = LO(IMDMA_D1_IRQ_STATUS);
	P4.H = HI(IMDMA_D1_IRQ_STATUS);
#else
	P4.L = LO(IMDMA_S1_IRQ_STATUS);
	P4.H = HI(IMDMA_S1_IRQ_STATUS);
#endif
	R6 = 0x1;
	R5 = W[P4](Z);
	
	CC = BITTST(R5,0);
	if !CC JUMP .notIDMA1;
	
	W[P4] = R6;

	VDK_ISR_ACTIVATE_DEVICE_(kIMDMA1);
	
.notIDMA1:
	ASTAT = R0;
	(R7:0,P5:0) = [SP++];
#endif
    RTI;
.EVT_IVG12_Entry.end:

/* ========================================================================== */
