/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for 
 *               the AD1871 ADC on the ADSP-BF537 EZ-Kit board.
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <cdefBF537.h>
#include <ccblkfn.h>
#include <sysreg.h>
#include "AD1871.h"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* preprocessor macros */

// SPORT0 word lengths
#define SLEN_8	0x0007
#define SLEN_16	0x000F
#define SLEN_24	0x0017
#define SLEN_32	0x001f

#define MAX_DEVICES 1
#define READ_MODE   (1 << 0)
#define WRITE_MODE  (1 << 1)
#define DIM(x) (sizeof(x)/sizeof((x)[0]))

/* globals */

/* statics */

/* externs */

/* class variables */

volatile unsigned short AD1871::c_vDmaIrqStatus[MAX_DEVICES];
volatile short *AD1871::c_nextAddr;
volatile short  AD1871::c_nextCount;

/******************************************************************************
 * Dispatch Function for the AD1871 Device Driver
 *
 * This function is called for all device-driver operations.
 * It "fans-out" to a specific function for each of the supported
 * operations.
 */
 
void*
AD1871::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    int    ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
        	Init(inUnion);
            break;
        case VDK::kIO_Activate:  
        	Activate(inUnion);
            break;
        case VDK::kIO_Open:      
        	ret_val = Open(inUnion);
            break;
        case VDK::kIO_Close:     
        	ret_val = Close(inUnion);
            break;
        case VDK::kIO_SyncRead:      
        	ret_val = SyncRead(inUnion);
            break;
        case VDK::kIO_SyncWrite:     
        	ret_val = -1;
            break;
        case VDK::kIO_IOCtl:
        	ret_val = IOCtl(inUnion);
            break;
        default:
                /* Put invalid DeviceDispatchID code HERE */
            break;
    }
    return reinterpret_cast<void *>(ret_val);
}


//
// This function runs during VDK startup (i.e. before any threads are
// running) and performs the initialization necessary both for the
// driver object and for the hardware itself.
//
void AD1871::Init (const VDK::DispatchUnion &inUnion)
{
	// Get the instance number from the boot I/O object initializer
	m_devNo = *((long *)inUnion.Init_t.pInitInfo);
	
	// Initialize the driver's member variables.
	m_sampleBytes = 2;
	m_readFlags  = 0;
	m_DevFlagRead  = VDK::CreateDeviceFlag();
	m_vReadCurrBuffCount  = 0;
	m_vReadNextBuffCount  = 0;

	// enable Sport0 RX interrupt
	*pSIC_IMASK |= 0x00000020;
}


//
// This function is responsible for the setup of the device
// driver each time a thread calls VDK::OpenDevice() on it.
//
// The control string for the driver may specify a number of
// flags to control the initialisation and operation of the
// device:-
//
//	R  - open for reading (audio input)
//	8  - 8 bits per sample, each stored as a signed char/byte
//	16 - 16 bits per sample, each stored as a signed short
//	24 - 24 bits per sample, stored right-justified in an
//       unsigned int with no sign-extension
//
// The sample rate is fixed at 48KHz as this is hardwired by
// the EZ-Kit board.
//
// The device may only be opened for reading - by
// specifying "R" in the control string - SyncWrite() calls to
// this device will fail.
//  
int AD1871::Open (const VDK::DispatchUnion &inUnion)
{
	unsigned mode = 0;
	
	// Set up the default mode, 16-bits/sample
	m_slen = SLEN_16;
	m_sampleBytes = 2;
	m_wdSize = WDSIZE_16;

	// Parse the mode string
	char *pStr = inUnion.OpenClose_t.flags;

	while (*pStr)
	{
		switch (*pStr)
		{
			default:
				return -1;	// unknown option
			case 'R': case 'r':
				mode |= READ_MODE;
				break;
			case '8':  // 8-bit samples
				m_slen = SLEN_8;
				m_sampleBytes = 1;
				m_wdSize = WDSIZE_8;
				break;					
			case '1':  // 16-bit samples
				if ('6' == pStr[1])
				{
					++pStr;
					m_slen = SLEN_16;
					m_sampleBytes = 2;
					m_wdSize = WDSIZE_16;
				}
				break;					
			case '2':  // 24-bit samples
				if ('4' == pStr[1])
				{
					++pStr;
					m_slen = SLEN_24;
					m_sampleBytes = 4;
					m_wdSize = WDSIZE_32;
				}
				break;					
			case ' ': case '\t': case ',':
				break;		// ignore punctuation and whitespace
		}
		++pStr;
	}

	// The driver only allows one open for reading and one for writing
	if (mode & m_readFlags & READ_MODE)
		return -1; 			// already open in this mode

	// Store the open mode in the device handle
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(mode);

	// reset sport0 tx
	*pSPORT0_RCR1 = 0xFFF0;

	// Sport0 receive configuration
#ifdef EZKIT_1_0
	// DSP is clock master for the Sport
	*pSPORT0_RCR1 = RFSR | LRFS | RCKFE | IRFS | IRCLK;   // MSB first, Active Low, Int. CLK, Int. Frame sync
	*pSPORT0_RCR2 = m_slen | RSFSE;                       // Stereo frame sync enable
	*pSPORT0_RCLKDIV= 19;
	*pSPORT0_RFSDIV = 31;
#else // BF537 EzKit 1.1 or later (default)
	// DSP is clock slave for the Sport
	*pSPORT0_RCR1 = RFSR | LRFS | RCKFE ;   // Ext. CLK, Ext. Frame sync, MSB first, Active Low
	*pSPORT0_RCR2 = m_slen | RSFSE;         // Stereo frame sync enable
#endif

	m_readBuffers = 0;
	m_readFlags = mode;

	return 0;
}

int AD1871::Close (const VDK::DispatchUnion &inUnion)
{
	// Retrieve the open mode from the device handle
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.OpenClose_t.dataH);

	if (mode != (mode & (m_readFlags)))
		return -1;

	VDK::DispatchUnion outUnion;
	outUnion.ReadWrite_t.dataH    = inUnion.OpenClose_t.dataH;
	outUnion.ReadWrite_t.timeout  = 0;
	outUnion.ReadWrite_t.dataSize = 0;
	outUnion.ReadWrite_t.data     = NULL;

	SyncRead(outUnion); // wait for any current read to complete
	m_readFlags = 0;

	*inUnion.OpenClose_t.dataH = 0;
	
	return 0;
}


//
// This function implements pipelined 'read' behaviour for the AD1871 device.
//
// On entry, the m_readBuffers member variable will always contain either
// zero or one. One means that a pipelined read (started by the previous call)
// is in progress, so we will block until it completes. Zero means that no
// transfer is in progress, so we will simply start the new transfer and return.
//
// Note that although m_readBuffers may be set to 2 immediately prior to
// blocking on the device flag, it will always have been decremented (in 
// Actvate()) before the device flag is posted, so that m_readBuffers will
// always be 0 or 1 when we leave this function.
//
int AD1871::SyncRead(const VDK::DispatchUnion &inUnion)
{
	// Retrieve the open mode from the device handle
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);

	if (0 == (mode & READ_MODE))  // must be open for reading
		return -1;

	// Store the SyncRead arguments in instance variables
	m_vReadNextBuffStart = (short *)inUnion.ReadWrite_t.data;
	m_vReadNextBuffCount = inUnion.ReadWrite_t.dataSize / m_sampleBytes;

	// TBD. We should flush and invalidate the data cache over the range of
	// the data buffer here, as DMA bypasses the cache. Until this is added,
	// the driver will not be safe to use with data caching enabled.
	
	// Disable interrupts
	VDK::PushCriticalRegion();

	// If m_readBuffers was non-zero on entry then input is
	// already underway into the previous buffer...
	if (0 != m_readBuffers) // m_readBuffers == 1
	{
		// ...in which case we have to block until it completes.
		
		// If the buffer size is zero then it's a dummy write which
		// just blocks until the current write finishes, hence we
		// don't count it as a pending buffer.
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			m_readBuffers = 2;
			
			// Set up globals for the ISR to swap in the
			// new buffer
			c_nextAddr  = m_vReadNextBuffStart;
			c_nextCount = m_vReadNextBuffCount;
		}
			
		// Block on the device flag (this re-enables interrupts)			
		VDK::PendDeviceFlag(m_DevFlagRead, inUnion.ReadWrite_t.timeout);
	}
	else // m_readBuffers is zero
	{
		// ...otherwise we need to start the transfer by setting up DMA3
		// If the buffer size is zero then we don't do anything
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			// Setup the first DMA transfer in the sequence
			m_readBuffers = 1;
			*pDMA3_START_ADDR = m_vReadNextBuffStart; // Start address of data buffer
			*pDMA3_X_COUNT = m_vReadNextBuffCount;    // DMA inner loop count
			*pDMA3_X_MODIFY	= m_sampleBytes;          // Inner loop address increment 
	
			// Enable DMA for input, with 16-bit transfers and interrupt on completion
			*pDMA3_CONFIG = WNR | m_wdSize | DI_EN | DMAEN;
			
			// enable Sport0 RX
			*pSPORT0_RCR1 |= RSPEN;
		}
		
		// Re-enable interrupts
		VDK::PopCriticalRegion();
	}
	
	// Now reap the result of the previous operation
	int result = m_vReadCurrBuffCount * m_sampleBytes;
	
	// and move the "next" transfer details to the "current" variables
	m_vReadCurrBuffCount = m_vReadNextBuffCount;
	m_vReadCurrBuffStart = m_vReadNextBuffStart;
	m_vReadNextBuffCount = 0;
	m_vReadNextBuffStart = NULL;

	return result;
}


//
// There is no IOCtl functionality at present for this device.
// In future it may support updates to the codec control registers.
//
int AD1871::IOCtl    (const VDK::DispatchUnion &inUnion)
{
	return 0;
}


//
// This function is invoked (initiated by the ISR) whenever the serial
// port Rx or Tx DMA reaches the end of one of its current buffer.
//
// Activate()'s job is to:
// - start the next DMA transfer if there is a buffer queued, else
//   turn off the serial port
// - unblock waiting threads
//
// First we have to determine which of the serial port interrupt
// sources are active. We do this by looking at the DMA3_irq_status
// and DMA3_irq_status variables, which hold the contents of the
// dma irq status registers at the time of the most recent IVG9
// interrupt (and prior to the interrupts being cleared.
//
void AD1871::Activate (const VDK::DispatchUnion &inUnion)
{
	// Check DMA3 status (serial port 0 Rx)
	if (DMA_DONE & c_vDmaIrqStatus[m_devNo]) // input buffer is full
	{
		if (2 == m_readBuffers) // then there is another buffer waiting
		{
			// Start the next DMA transfer
			m_readBuffers = 1;
		}
		else
		{
			// stop DMA3
			*pDMA3_CONFIG = WNR | m_wdSize | DI_EN;

			// No more buffers, disable Sport0 RX
			*pSPORT0_RCR1 &= ~RSPEN;
			m_readBuffers = 0;
		}

		// Post Rx device flag
		VDK::PostDeviceFlag(m_DevFlagRead);
	}
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
