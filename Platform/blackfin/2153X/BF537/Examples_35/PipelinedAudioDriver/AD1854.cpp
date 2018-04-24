/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for
 *               the AD1854 DAC on the ADSP-BF537 EZ-Kit board.
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <cdefBF537.h>
#include <ccblkfn.h>
#include <sysreg.h>
#include "AD1854.h"

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

extern "C" void InitFlags();

/* class variables */

volatile unsigned short AD1854::c_vDmaIrqStatus[MAX_DEVICES];
volatile short *AD1854::c_nextAddr;
volatile short  AD1854::c_nextCount;

/******************************************************************************
 * Dispatch Function for the AD1854 Device Driver
 *
 * This function is called for all device-driver operations.
 * It "fans-out" to a specific function for each of the supported
 * operations.
 */
 
void*
AD1854::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
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
        	ret_val = -1;
            break;
        case VDK::kIO_SyncWrite:     
        	ret_val = SyncWrite(inUnion);
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
void AD1854::Init (const VDK::DispatchUnion &inUnion)
{
	// Get the instance number from the boot I/O object initializer
	m_devNo = *((long *)inUnion.Init_t.pInitInfo);
	
	// Initialize the driver's member variables.
	m_sampleBytes = 2;
	m_writeFlags = 0;
	m_DevFlagWrite = VDK::CreateDeviceFlag();
	m_vWriteCurrBuffCount = 0;
	m_vWriteNextBuffCount = 0;

	InitFlags();
	
	// enable Sport0 TX interrupt
	*pSIC_IMASK |= 0x00000040;
}


//
// This function is responsible for the setup of the device
// driver each time a thread calls VDK::OpenDevice() on it.
//
// The control string for the driver may specify a number of
// flags to control the initialisation and operation of the
// device:-
//
//	W  - open for writing (audio output)
//	8  - 8 bits per sample, each stored as a signed char/byte
//	16 - 16 bits per sample, each stored as a signed short
//	24 - 24 bits per sample, stored right-justified in an
//       unsigned int with no sign-extension
//
// The sample rate is fixed at 48KHz as this is hardwired by
// the EZ-Kit board.
//
// The device may only be opened for writing - by
// specifying "W" in the control string - SyncRead() calls to
// this device will fail.
//  
int AD1854::Open (const VDK::DispatchUnion &inUnion)
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
			case 'W': case 'w':
				mode |= WRITE_MODE;
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

	// The driver can only be opened once at at time
	if (mode & m_writeFlags & WRITE_MODE)
		return -1; 			// already open

	// Store the open mode in the device handle
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(mode);

	// reset sport0 tx
	*pSPORT0_TCR1 = 0xFFF0;

	// Sport0 transmit configuration
#ifdef EZKIT_1_0
	// DSP is clock master for the Sport
	*pSPORT0_TCR1 = TFSR | LTFS | TCKFE | ITFS | ITCLK | DITFS; // Int. CLK & Frame sync, MSB first, Active Low
	*pSPORT0_TCR2 = m_slen | TSFSE;                             // Stereo frame sync enable
	*pSPORT0_TCLKDIV= 19;
	*pSPORT0_TFSDIV = 31;
#else // BF537 EzKit 1.1 or later (default)
	// DSP is clock slave for the Sport
	*pSPORT0_TCR1 = TFSR | LTFS | TCKFE | DITFS;   // Int. CLK & Frame sync, MSB first, Active Low
	*pSPORT0_TCR2 = m_slen | TSFSE;                // Stereo frame sync enable
#endif

	m_writeBuffers = 0;
	m_writeFlags = mode;

	return 0;
}

int AD1854::Close (const VDK::DispatchUnion &inUnion)
{
	// Retrieve the open mode from the device handle
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.OpenClose_t.dataH);

	if (mode != (mode & (m_writeFlags)))
		return -1;

	VDK::DispatchUnion outUnion;
	outUnion.ReadWrite_t.dataH    = inUnion.OpenClose_t.dataH;
	outUnion.ReadWrite_t.timeout  = 0;
	outUnion.ReadWrite_t.dataSize = 0;
	outUnion.ReadWrite_t.data     = NULL;

	SyncWrite(outUnion); // wait for any current write to complete
	m_writeFlags = 0;

	*inUnion.OpenClose_t.dataH = 0;
	
	return 0;
}


//
// This function implements pipelined 'write' behaviour for the AD1854 device.
//
// On entry, the m_writeBuffers member variable will always contain either
// zero or one. One means that a pipelined write (started by the previous call)
// is in progress, so we will block until it completes. Zero means that no
// transfer is in progress, so we will simply start the new transfer and return.
//
// Note that although m_writeBuffers may be set to 2 immediately prior to
// blocking on the device flag, it will always have been decremented (in 
// Activate()) before the device flag is posted, so that m_writeBuffers will
// always be 0 or 1 when we leave this function.
//
int AD1854::SyncWrite(const VDK::DispatchUnion &inUnion)
{
	// Retrieve the open mode from the device handle
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);

	if (0 == (mode & WRITE_MODE))  // must be open for reading
		return -1;

	// Store the SyncWrite arguments in instance variables
	m_vWriteNextBuffStart = (short *)inUnion.ReadWrite_t.data;
	m_vWriteNextBuffCount = inUnion.ReadWrite_t.dataSize / m_sampleBytes;

	// TBD. We should flush and invalidate the data cache over the range of
	// the data buffer here, as DMA bypasses the cache. Until this is added,
	// the driver will not be safe to use with data caching enabled.

	// Disable interrupts
	VDK::PushCriticalRegion();

	// If m_writeBuffers is non-zero then output is
	// currently underway from the previous buffer...
	if (0 != m_writeBuffers) // m_writeBuffers == 1
	{
		// ...in which case we have to block until it completes.
		
		// If the buffer size is zero then it's a dummy write which
		// just blocks until the current write finishes, hence we
		// don't count it as a pending buffer.
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			m_writeBuffers = 2;
			
			// Set up globals for the ISR to swap in the
			// new buffer
			c_nextAddr  = m_vWriteNextBuffStart;
			c_nextCount = m_vWriteNextBuffCount;
		}
		
		// Block on the device flag (this re-enables interrupts)			
		VDK::PendDeviceFlag(m_DevFlagWrite, inUnion.ReadWrite_t.timeout);
	}
	else // m_writeBuffers is zero
	{
		// ...otherwise we need to start the transfer by setting up DMA4

		// If the buffer size is zero then we don't do anything
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			// Setup the first DMA transfer in the sequence
			m_writeBuffers = 1;
			*pDMA4_START_ADDR = m_vWriteNextBuffStart; // Start address of data buffer
			*pDMA4_X_COUNT = m_vWriteNextBuffCount;    // DMA inner loop count
			*pDMA4_X_MODIFY	= m_sampleBytes;           // Inner loop address increment
	
			// Enable DMA for output, with the required word size and interrupt on completion
			*pDMA4_CONFIG = m_wdSize | DI_EN | DMAEN;
			
			// enable Sport0 TX
			*pSPORT0_TCR1 |= TSPEN;
		}
		
		// Re-enable interrupts
		VDK::PopCriticalRegion();
	}

	// Now reap the result of the previous operation
	int result = m_vWriteCurrBuffCount * m_sampleBytes;
	
	// and move the "next" transfer details to the "current" variables
	m_vWriteCurrBuffCount = m_vWriteNextBuffCount;
	m_vWriteCurrBuffStart = m_vWriteNextBuffStart;
	m_vWriteNextBuffCount = 0;
	m_vWriteNextBuffStart = NULL;

	return result;
}


//
// There is no IOCtl functionality at present for this device.
// In future it may support updates to the codec control registers,
// but this is unlikely as the current configuration of the EZ-Kit
// board does not permit access to these registers.
//
int AD1854::IOCtl    (const VDK::DispatchUnion &inUnion)
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
// sources are active. We do this by looking at the c_vDmaIrqStatus
// variable, which holds the contents of the Tx dma irq
// status registers at the time of the most recent IVG9
// interrupt (and prior to the interrupts being cleared).
//
void AD1854::Activate (const VDK::DispatchUnion &inUnion)
{
	// Check DMA4 status (serial port 0 Tx)
	if (DMA_DONE & c_vDmaIrqStatus[m_devNo]) // output buffer has emptied
	{
		if (2 == m_writeBuffers) // then there is another buffer waiting
		{
			// Start the next DMA transfer
			m_writeBuffers = 1;
		}
		else
		{			
			// No more buffers, disable Sport0 TX
			*pDMA4_CONFIG = m_wdSize | DI_EN;  // Restart DMA4
			*pSPORT0_TCR1 &= ~TSPEN;
			m_writeBuffers = 0;
		}

		// Post Tx device flag
		VDK::PostDeviceFlag(m_DevFlagWrite);
	}
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
