/* =============================================================================
 *
 *   Description: This is the Interrupt Service Routine called EVT_IVG8_Entry
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *
 * ===========================================================================*/

#include "VDK.h"
#include <defBF535.h>

#define HI(x) ((x) >> 16)
#define LO(x) ((x) & 0xFFFF)

// Byte offsets within the AC97 frame
#define PHASE 0
#define REG   2
#define CMD   4
#define LEFT  6
#define RIGHT 8

.file_attr prefersMemNum="30";
.file_attr prefersMem="internal";
.file_attr ISR;

.section/doubleany data1;
/* declare external globals */
.align 4;
.byte4 _g_TxCount_AD1885;
.byte4 _g_TxPtr_AD1885;
.byte4 _g_RxCount_AD1885;
.byte4 _g_RxPtr_AD1885;
.byte4 _AC97Cmd;
.global _g_TxCount_AD1885;
.global _g_TxPtr_AD1885;
.global _g_RxCount_AD1885;
.global _g_RxPtr_AD1885;
.global _AC97Cmd;

/* declare locals */

.section/doubleany data1; // should be dmadata if caching in use
/* declare buffers */
.align 2;
.byte2 _rxFrameBuffer[32];
.byte2 _txFrameBuffer[32];
.global _rxFrameBuffer;
.global _txFrameBuffer;

.section program;

/******************************************************************************
 * EVT_IVG8_Entry
 */

/* ISR entry point */
.GLOBAL     EVT_IVG8_Entry;
EVT_IVG8_Entry:
	[--SP] = RETI; /* Pushing the RETI register re-enables nested interrupts */
	[--SP] = (R7:0,P5:0);
	[--SP] = ASTAT;

	// Access and update the SPORT0 interrupt status registers,
	// P5 used throughout the ISR to address SPORT0 MMRs
	P5.H = HI(SPORT0_IRQSTAT_RX);
	P5.L = LO(SPORT0_IRQSTAT_RX);
	R7 = 0x1;				// mask to clear the completion interrupt
	R6 = W[P5];				// get the RX interrupt status
	
	// If it isn't a completion interrupt then we do nothing
	CC = BITTST(R6, 0);		// check RX completion bit 
	if !CC JUMP .doTx;

	W[P5] = R7;				// clear the RX completion interrupt
	
	// Find out which half of the Rx DMA buffer is currently
	// being transferred by the engine. We will read from the
	// other half.
	P3.H = HI(SPORT0_COUNT_RX);
	P3.L = LO(SPORT0_COUNT_RX);
	R5 = W[P3](Z);			// R5 = current RX DMA count
	R4.L = _rxFrameBuffer;
	R4.H = _rxFrameBuffer;
	R3 = R4;
	R3 += 32;
	R2 = 16;
	CC = R5 <= R2 (IU);
	IF !CC R4 = R3;
	P4 = R4;				// P4 = Rx frame pointer (pFrame)
	
	// Check if there's space in the input buffer
	P3.L = _g_RxCount_AD1885;
	P3.H = _g_RxCount_AD1885;
	P2.L = _g_RxPtr_AD1885;
	P2.H = _g_RxPtr_AD1885;
	P1 = [P2];				// P1 = g_RxPtr
	R7 = [P3];				// R7 = g_RxCount
	CC = R7 == 0;			// if g_RxCount == 0
	if CC JUMP .doTx;		// then skip Rx processing
	
	R0 = W[P4 + LEFT](Z);	// R0 = pFrame->LEFT
	R1 = W[P4 + RIGHT](Z);	// R1 = pFrame->RIGHT
	W[P1++] = R0;			// *g_RxPtr++ = left sample
	W[P1++] = R1;			// *g_RxPtr++ = right sample
	[P2] = P1;				// store g_RxPtr
	
	R7 += -1;				// decrement g_RxCount
	CC = AZ;				// check for g_RxCount == 0
	[P3] = R7;				// store the incremented g_RxCount
	
	if !CC JUMP .doTx;		// if g_RxCount != 0 then skip
	
	// Activate the device driver if g_RxCount == 0
	VDK_ISR_ACTIVATE_DEVICE_(kCodec0);

.doTx:
	// Find out which half of the Tx DMA buffer is currently
	// being transferred by the engine. We will write to the
	// other half.
	P3.H = HI(SPORT0_COUNT_TX);
	P3.L = LO(SPORT0_COUNT_TX);
	R5 = W[P3](Z);			// R5 = current TX DMA count
	R4.L = _txFrameBuffer;
	R4.H = _txFrameBuffer;
	R3 = R4;
	R3 += 32;
	R2 = 16;
	CC = R5 <= R2 (IU);
	IF !CC R4 = R3;
	P4 = R4;				// P4 = Rx frame pointer (pFrame)
		
	R4 = 0x8000(Z);     	// R4 = tags, 'valid' bit set in every frame
	P3.L = _g_TxCount_AD1885;
	P3.H = _g_TxCount_AD1885;
	P2.L = _g_TxPtr_AD1885;
	P2.H = _g_TxPtr_AD1885;
	P1 = [P2];				// P1 = g_TxPtr
	R7 = [P3];				// R7 = g_TxCount
	CC = R7 == 0;			// if g_TxCount == 0
	if CC JUMP .chkCmd;		// then skip Tx processing

	R0 = W[P1++];			// R0 = *pTxPtr++
	R1 = W[P1++];			// R1 = *pTxPtr++
	BITSET(R4, 11);			// set the left sample tag bit
	BITSET(R4, 12);			// set the right sample tag bit
	W[P4 + LEFT] = R0;		// pFrame->LEFT = R0
	W[P4 + RIGHT] = R1;		// pFrame->RIGHT = R0
	[P2] = P1;				// store the incremented g_TxPtr
	
	R7 += -1;				// decrement g_TxCount
	CC = AZ;				// check for g_TxCount == 0
	[P3] = R7;				// store g_TxCount
	
	if !CC JUMP .chkCmd;	// if g_TxCount != 0 then skip
	
	// Activate the device driver if g_TxCount == 0
	VDK_ISR_ACTIVATE_DEVICE_(kCodec0);

.chkCmd:
	// Check for a command word to go to the codec
	P3.L = _AC97Cmd;
	P3.H = _AC97Cmd;
	R7 = [P3];				// R7 = AC97Cmd
    CC = R7 == 0;			// if AC97Cmd == 0			
	IF CC JUMP .allDone;	// then skip command processing
	
	BITSET(R4, 14);			// set reg and val tag bits
	BITSET(R4, 13);
	W[P4 + REG]	= R7;		// pFrame->REG is low word of AC97Cmd
	R7 >>= 16;
	W[P4 + CMD] = R7;		// pFrame->CMD is high word of AC97Cmd
	R7 = 0;
	[P3] = R7;				// AC97Cmd = 0

.allDone:	
	W[P4 + PHASE] = R4;		// frame->PHASE = tags
	
	ASTAT = [SP++];
	(R7:0,P5:0) = [SP++];
	RETI = [SP++];
    RTI;
.EVT_IVG8_Entry.end:

/* ========================================================================== */
