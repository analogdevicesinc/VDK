/* =============================================================================
 *
 *   Description: This is the Interrupt Service Routine called EVT_IVG9_Entry
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"
#include <defBF533.h>

#define DEV_NO 0

.file_attr prefersMemNum="30";
.file_attr prefersMem="internal";
.file_attr ISR;

.section/doubleany data1;

#define RX_IRQ_STATUS DMA1_IRQ_STATUS
#define TX_IRQ_STATUS DMA2_IRQ_STATUS

/* declare external globals */
.extern  _c_vDma1IrqStatus__6AD1836;
.extern  _c_vDma2IrqStatus__6AD1836;
.extern  _c_nextRxAddr__6AD1836;
.extern  _c_nextRxCount__6AD1836;
.extern  _c_nextTxAddr__6AD1836;
.extern  _c_nextTxCount__6AD1836;

/* declare locals */

.section/doubleany program;

/******************************************************************************
 * EVT_IVG9_Entry
 *
 * This is the interrupt service routine for the IVG9 (priority level 2)
 * interrupt. It services interrupts from the SPort0 Rx and Tx DMA channels
 * (DMA1 and DMA2 in this case) on behalf of the AD1836 device driver.
 *
 * In line with VDK recommendations, the ISR does the minimum work
 * necessary before handing-off to the device driver itself. That work
 * includes:
 *
 * - identifying the source of the interrupt from amongst the various
 *   possible sources which are mapped to this interrupt level
 *
 * - reading any "short-lived" status information from the hardware
 *   and storing it where it can be retrieved by the device driver
 *
 * - clearing the interrupt source on the hardware, so that the interrupt
 *   is de-asserted
 *
 * - activating the device driver to continue processing of the event
 *   at Kernel level
 *
 * If other interrupt sources are required to share this interrupt level then
 * the isr must be extended with further test-and-service blocks to detect
 * those interrupt sources and respond appropriately.
 *
 */

/* ISR entry point */
.GLOBAL     EVT_IVG9_Entry;
EVT_IVG9_Entry:
	[--SP] = (R7:6,P5:4); // save working registers
	[--SP] = ASTAT;       // preserve arithmetic status
	[--SP] = RETI;        // enable nested interrupts

	// Check the irq status of DMA1, saving the status in
	// the global variable _dma1_irq_status
	P5.L = LO(RX_IRQ_STATUS);
    P5.H = HI(RX_IRQ_STATUS);
    P4.L = _c_vDma1IrqStatus__6AD1836;
    P4.H = _c_vDma1IrqStatus__6AD1836;
    R7 = W[P5](Z);				// read the DMA1_IRQ_STATUS MMR
    W[P4 + DEV_NO] = R7;        // save the value to the global
    
    CC = BITTST(R7,0);  // test whether the DMA_DONE flag is set
    
    if !CC JUMP .notRXint;    // if not then skip over
    R6 = 1;						// write 1 to the DMA_DONE bit
    W[P5] = R6;                 // to clear it (and the interrupt)

    // Set up the next transfer
	P5.L = LO(DMA1_START_ADDR);
    P5.H = HI(DMA1_START_ADDR);
    P4.L = _c_nextRxAddr__6AD1836;
    P4.H = _c_nextRxAddr__6AD1836;
    R7 = W[P5+4](Z); // read DMA1_CONFIG
    
    R6 = [P4];    // read AD1836::c_nextRxAddr
    [P5] = R6;    // write DMA3_START_ADDR
        
    P4.L = _c_nextRxCount__6AD1836;
    P4.H = _c_nextRxCount__6AD1836;
    R6 = W[P4](Z);    // read AD1836::c_nextRxCount
    
    CC = R6 == 0;
    IF CC JUMP .noRXBuff;
    
    W[P5+12] = R6; // write DMA3_X_COUNT

    R6 = 0;
    W[P4] = R6;      // zero AD1871::c_nextCount
       
    BITSET(R7, 0);   // set the DMAEN bit
    W[P5+4] = R7;    // write to DMA3_CONFIG
    
.noRXBuff:    
   
    // Activate the device driver to service the DMA completion
    VDK_ISR_ACTIVATE_DEVICE_(kCodec0);
    
.notRXint:
    // Check the irq status of DMA2, saving the status in
	// the global variable _dma2_irq_status
    P5.L = LO(TX_IRQ_STATUS);
    P5.H = HI(TX_IRQ_STATUS);
    P4.L = _c_vDma2IrqStatus__6AD1836;
    P4.H = _c_vDma2IrqStatus__6AD1836;
    R7 = W[P5](Z);				// read the DMA2_IRQ_STATUS MMR
    W[P4 + DEV_NO] = R7;        // save the value to the global
    
    CC = BITTST(R7,0);  // test whether the DMA_DONE flag is set
    
    if !CC JUMP .notTXint;    // if not then skip over
    R6 = 1;						// write 1 to the DMA_DONE bit
    W[P5] = R6;                 // to clear it (and the interrupt)

    // Set up the next transfer
	P5.L = LO(DMA2_START_ADDR);
    P5.H = HI(DMA2_START_ADDR);
    P4.L = _c_nextTxAddr__6AD1836;
    P4.H = _c_nextTxAddr__6AD1836;
    R7 = W[P5+4](Z); // read DMA4_CONFIG
    
    R6 = [P4];       // read AD1854::c_nextAddr
    [P5] = R6;       // write DMA4_START_ADDR
        
    P4.L = _c_nextTxCount__6AD1836;
    P4.H = _c_nextTxCount__6AD1836;
    R6 = W[P4](Z);   // read AD1836::c_nextTxCount
    
    CC = R6 == 0;
    IF CC JUMP .noTXBuff;
    
    W[P5+12] = R6;   // write DMA3_X_COUNT

    R6 = 0;
    W[P4] = R6;      // zero AD1871::c_nextCount

	BITSET(R7, 0);   // ensure that the DMAEN bit is set
    W[P5+4] = R7;    // write to DMA4_CONFIG
    
.noTXBuff:    
   
    // Activate the device driver to service the DMA completion
    // (it's ok if this is done twice in the isr)
    VDK_ISR_ACTIVATE_DEVICE_(kCodec0);
    
.notTXint:
	// restore registers and return
	RETI = [SP++];
    ASTAT = [SP++];
	(R7:6,P5:4) = [SP++];
    RTI;
.EVT_IVG9_Entry.end:

/* ========================================================================== */
