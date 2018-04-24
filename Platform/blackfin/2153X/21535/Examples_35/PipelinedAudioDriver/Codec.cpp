/* ===================================================================
 *
 *    AC97.cpp
 *
 * ===================================================================*/

#include "VDK.h"
#include <defBF535.h>
#include "InitCodec.h"

#define SetIop(addr, val) {(*(volatile short *) addr) = (val); asm volatile("ssync;");}
#define GetIop(addr) (*(volatile short *) addr)
#define SetIopL(addr, val) {(*(volatile long *) addr) = (val); asm volatile("ssync;");}
#define GetIopL(addr) (*(volatile long  * ) addr)

extern volatile unsigned AC97Cmd;
extern short txFrameBuffer[];
extern short rxFrameBuffer[];

unsigned int iCodecRegs[] =	// array for codec registers
{						// names are defined in "InitCodec.h"
		SERIAL_CONFIGURATION,	0xFF80,
		SERIAL_CONFIGURATION,	0x9900,		// use 16 bit slots 	
		RESET,					0xffff,		// codec reset
		MASTER_VOLUME,			0x0000,		// 0 dB attenuation
		HEADPHONES_VOLUME,		0x0000,		// +6 dB gain = speaker + phone out
		MASTER_VOLUME_MONO,		0x8000,		// channel muted
		PC_BEEP,				0x8000,		// channel muted
		PHONE_VOLUME,			0x8008,		// channel muted, 0 dB Att.
		MIC_VOLUME,				0x8008,		// channel muted, 0 dB att.
		LINE_IN_VOLUME,			0x8808,		// mute, 0dB left, 0dB right
		CD_VOLUME,				0x8808,		// mute, 0dB left, 0dB right
		VIDEO_VOLUME,			0x8808,		// mute, 0dB left, 0dB right
		AUX_VOLUME,				0x8808,		// mute, 0dB left, 0dB right
		PCM_OUT_VOLUME,			0x0F0F,		// -34.5dB left, -34.5dB right
		RECORD_SELECT_CONTROL,	0x0404,		// select Line_in_L, Line _in_R
		RECORD_GAIN,			0x0000,		// 0dB left, 0dB right
		GENERAL_PURPOSE,		0x0000,		// Mic1, Mix, PHAT off, pre-3d
		THREE_D_CONTROL,		0x0000,		// depth = 0%
		SUBSECTION_READY,		0x0000,		// power on all sections
		EXTENDED_AUDIO_CTL,		0x0000,		// variable rate enabled
		JACK_SENSE,				0x0000,		
		MISC_CONTROL_BITS,		0x0404
};

static void setupSPort()
{
	SetIop(SPORT0_MTCS0, 0xffff);	SetIop(SPORT0_MTCS1, 0x0); 
	SetIop(SPORT0_MTCS2, 0x0);		SetIop(SPORT0_MTCS3, 0x0); 
	SetIop(SPORT0_MTCS4, 0x0);		SetIop(SPORT0_MTCS5, 0x0); 
	SetIop(SPORT0_MTCS6, 0x0);		SetIop(SPORT0_MTCS7, 0x0);

	SetIop(SPORT0_MRCS0, 0xffff);	SetIop(SPORT0_MRCS1, 0x0);
	SetIop(SPORT0_MRCS2, 0x0);		SetIop(SPORT0_MRCS3, 0x0); 
	SetIop(SPORT0_MRCS4, 0x0);		SetIop(SPORT0_MRCS5, 0x0); 
	SetIop(SPORT0_MRCS6, 0x0);		SetIop(SPORT0_MRCS7, 0x0);
	
	SetIop(SPORT0_MCMC1, 0x23);
	SetIop(SPORT0_MCMC2, 0x0c);

	SetIop(SPORT0_RFSDIV, 0x00ff);
	SetIop(SPORT0_RX_CONFIG, 0x03e0);
	SetIop(SPORT0_TX_CONFIG, 0x01e0);

// enable SPORT TX autobuffer based DMA
	short * txFrameBufferPtr = txFrameBuffer; //done to avoid a compiler warning
	SetIop(SPORT0_CONFIG_DMA_TX, 0x10);  // DMA Configuration
	SetIop(SPORT0_START_ADDR_LO_TX, (unsigned)txFrameBufferPtr);
	SetIop(SPORT0_START_ADDR_HI_TX, (unsigned)((unsigned long)txFrameBuffer >> 16));
	SetIop(SPORT0_COUNT_TX, 32);
	SetIop(SPORT0_CONFIG_DMA_TX, 0x11);  // DMA Configuration

// enable SPORT RX autobuffer based DMA 
	short * rxFrameBufferPtr = rxFrameBuffer; //done to avoid a compiler warning
	SetIop(SPORT0_CONFIG_DMA_RX, 0x10);  // DMA Configuration
	SetIop(SPORT0_START_ADDR_LO_RX, (unsigned)rxFrameBufferPtr);
	SetIop(SPORT0_START_ADDR_HI_RX, (unsigned)((unsigned long)rxFrameBuffer >> 16));
	SetIop(SPORT0_COUNT_RX, 32);
	SetIop(SPORT0_CONFIG_DMA_RX, 0x17);  // DMA Configuration
}


/********************************************************************************/
/***** InitCodec()															*****/
/***** This function resets the codec, enables SPORT0 and sends all values 	*****/
/***** in the array iCodecRegs[] to the codec. Different configurations can	*****/
/***** be set up by modifiying the values in this array.					*****/
/********************************************************************************/
void InitCodec(void)
{
	int iCounter;
	long tmp;

	//wait for frame valid flag from codec
	while((rxFrameBuffer[0] & 0x8000) == 0)			// test first bit in input frame
	{
		 asm volatile("Flushinv[%0];"
		: 
		: "a" (&(rxFrameBuffer[0]))
		:); 
	}
	// configure codec
	iCounter = 0;
	while(iCounter < sizeof(iCodecRegs) / sizeof(int))
	{
		if(AC97Cmd == 0)
		{
			tmp = (unsigned long)iCodecRegs[iCounter+1]<<16;
			AC97Cmd = tmp +  (unsigned long)iCodecRegs[iCounter];
			iCounter += 2;
		}
	} 
}

void InitAC97()
{
	int i, val;
	long ChipID;
	int iCounter;

	txFrameBuffer[0] = 0xE000;	// program CODEC for Slot 16 mode
	txFrameBuffer[1] = 0x7400;
	txFrameBuffer[2] = 0xFF80;
	for (i = 3; i < 16; i++) txFrameBuffer[i] = 0;
	AC97Cmd = 0;
	
	rxFrameBuffer[0] = 0x0000;	// codec not ready while this is still zero

	setupSPort();

	ChipID = GetIopL(CHIPID);	// get Chip revision
	if(0 == (ChipID & 0xF0000000))		// check upper nybble
	{							// if zero rev.= 0.1 u. 0.2
		val = GetIop(SIC_IMASK);
		val &= 0xFFFFFFEF; // clear SPORT0 flag = enable IRQ
		SetIopL(SIC_IMASK, val); 
	}
	else {
		val = GetIop(SIC_IMASK); // revision 1.0 or later
		val |= 0x10; // clear SPORT0 flag = enable IRQ
		SetIopL(SIC_IMASK, val); 
	}
	VDK::SetInterruptMaskBits(0x100); // unmask SPORT0 RX - IRQ

		// reset codec
	SetIop(FIO_DIR, GetIop(FIO_DIR) | 0x800F); // set flag direction

	SetIop(FIO_FLAG_S, 0x800C);
	for(iCounter = 0; iCounter < 0xffff; iCounter++){
		asm volatile("nop;");
	}

	SetIop(FIO_FLAG_C, 0x800B);	 // reset codec
	for(iCounter = 0; iCounter < 0xffff; iCounter++){ // wait for (at least) 1 us
		asm volatile("nop;");
	}

	SetIop(FIO_FLAG_S, 0x8004);	 // release reset from codec
	for(iCounter = 0; iCounter < 0xfffff; iCounter++){ // delay to recover from reset
		asm volatile("nop;");
	}
	
	asm volatile("ssync;");
	// enable sport0 transmitter
	SetIop(SPORT0_RX_CONFIG, GetIop(SPORT0_RX_CONFIG) | 0x0001);
	SetIop(SPORT0_TX_CONFIG, GetIop(SPORT0_TX_CONFIG) | 0x0001);  // set sport0 transmitter enable bit
}
