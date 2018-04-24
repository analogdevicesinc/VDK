/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for AD1836
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <cdefBF533.h>
#include <ccblkfn.h>
#include <sysreg.h>
#include "AD1836.h"
#include "Talkthrough.h"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* preprocessor macros */

#define MAX_DEVICES 1
#define READ_MODE   (1 << 0)
#define WRITE_MODE  (1 << 1)
#define DIM(x) (sizeof(x)/sizeof((x)[0]))

/* globals */

/* statics */

// array for registers to configure the ad1836
// names are defined in "Talkthrough.h"
static short sCodec1836TxRegs[] =
{									
	DAC_CONTROL_1	| 0x010, // 16-bit, 48kHz
	DAC_CONTROL_2	| 0x000,
	DAC_VOLUME_0	| 0x3ff,
	DAC_VOLUME_1	| 0x3ff,
	DAC_VOLUME_2	| 0x3ff,
	DAC_VOLUME_3	| 0x3ff,
	DAC_VOLUME_4	| 0x3ff,
	DAC_VOLUME_5	| 0x3ff,
	ADC_CONTROL_1	| 0x100, // High-pass filter on,48KHz
	ADC_CONTROL_2	| 0x020, // 16-bit
	ADC_CONTROL_3	| 0x000
};

/* externs */

/* class variables */

volatile unsigned short AD1836::c_vDma1IrqStatus[MAX_DEVICES];
volatile unsigned short AD1836::c_vDma2IrqStatus[MAX_DEVICES];
volatile short *AD1836::c_nextRxAddr;
volatile short  AD1836::c_nextRxCount;
volatile short *AD1836::c_nextTxAddr;
volatile short  AD1836::c_nextTxCount;

//--------------------------------------------------------------------------//
// Function:	init1836()													//
//																			//
// Description:	This function sets up the SPI port to configure the AD1836. //
//				The content of the array pRegWords is sent to the codec.	//
//																			//
//--------------------------------------------------------------------------//
static void init1836(
	bool           reset,
	short vRegWords[],
	int            numRegWords
)
{
	// Reset of the codec is optional
	if (reset)
	{
		// Toggle bit 0 of Port A low to reset the AD1836
		*pFlashA_PortA_Data = 0x00;
		*pFlashA_PortA_Data = 0x01;
		
		// Wait for the AD1836 to recover from reset
		for (int i=0; i<0xf000; i++);
	}

	// Configure the SPI port for control communication with the AD1836
	*pSPI_FLG = FLS4;                       // Select SPI slave device 4
	*pSPI_BAUD = 16;                        // Set baud rate SCK = HCLK/(2*SPIBAUD) SCK = 2MHz
	*pSPI_CTL = TIMOD_DMA_TX | SIZE | MSTR; // DMA write, 16-bit data, MSB first, SPI Master
	
	// Set up DMA5 to transmit to the AD1836 over the SPI port
	*pDMA5_PERIPHERAL_MAP = 0x5000;        // Map DMA5 to SPI
	*pDMA5_CONFIG         = WDSIZE_16;     // 16-bit transfers
	*pDMA5_START_ADDR     = vRegWords;     // Start address of data buffer
	*pDMA5_X_COUNT        = numRegWords;   // DMA inner loop count
	*pDMA5_X_MODIFY       = sizeof(short); // Inner loop address increment
	
	// Enable DMA5
	*pDMA5_CONFIG = WDSIZE_16 | DMAEN;

	// Enable the SPI port
	*pSPI_CTL = TIMOD_DMA_TX | SIZE | MSTR | SPE;
	
	// Wait until dma transfers for SPI are finished 
	while (DMA_RUN & *pDMA5_IRQ_STATUS);
	
	 // Wait for 2 successive lows on TXS, then wait for SPIF to go low
	while ((*pSPI_STAT & TXS) || (*pSPI_STAT & TXS));
	while (*pSPI_STAT & SPIF);
	
	// Disable the SPI port
	*pSPI_CTL = 0x0000;
}

/******************************************************************************
 * Dispatch Function for the AD1836 Device Driver
 *
 * This function is called for all device-driver operations.
 * It "fans-out" to a specific function for each of the supported
 * operations.
 */
 
void*
AD1836::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
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
void AD1836::Init (const VDK::DispatchUnion &inUnion)
{
	// Get the instance number from the boot I/O object initializer
	m_devNo = *((long *)inUnion.Init_t.pInitInfo);
	
	// Initialize the driver's member variables.
	m_sampleBytes = 2;
	m_readFlags  = 0;
	m_writeFlags = 0;
	m_DevFlagRead  = VDK::CreateDeviceFlag();
	m_DevFlagWrite = VDK::CreateDeviceFlag();
	m_vReadCurrBuffCount  = 0;
	m_vReadNextBuffCount  = 0;
	m_vWriteCurrBuffCount = 0;
	m_vWriteNextBuffCount = 0;

	// Initialize and enable asynchronous memory banks in the
	// External Bus Interface Unit so that Flash A can be accessed.
	*pEBIU_AMBCTL0	= 0x7bb07bb0;
	*pEBIU_AMBCTL1	= 0x7bb07bb0;
	*pEBIU_AMGCTL	= 0x000f;

	// Set bit 0 or flash port A to output
	*pFlashA_PortA_Dir = 0x1;

	// Set Sport0 RX (DMA1) and TX (DMA2) interrupt priority to 2 = IVG9 
	*pSIC_IAR1 = (*pSIC_IAR1 & 0xfffff00f) | 0x00000220;
	
	// enable Sport0 RX and TX interrupts
	*pSIC_IMASK |= 0x00000600;
}


//
// This function is responsible for the setup of the device
// driver each time a thread calls VDK::OpenDevice() on it.
//
// The control string for the driver may specify a number of
// flags to control the initialisation and operation of the
// device. Some of these are global to the device and hence
// affect both the read and write sides, others may be
// specified (or not) independently for read and write.
//
//	R - open for reading (audio input)
//	W - open for writing (audio output)
//	F - force codec reset, only if device not already open
//	S - stereo mode, one channel pair (1L & 1R) - the default
//	Q - quadraphonic mode, two channel pairs (1L, 1R, 2L & 2R)
//
// At present the sample rate is fixed at 48KHz and the sample
// width at 16 bits. A future update to the driver will add
// control-string flags to enable 96KHz sampling and 20 or 24 bits
// sample width.
//
// The device may be opened for both reading and writing - by
// specifying "RW" in the control string - or may be opened
// separately for reading and writing, usually by different
// threads. The latter case is known as "split-open" and will
// normally be the most useful when dealing with streaming
// audio data.
//  
int AD1836::Open (const VDK::DispatchUnion &inUnion)
{
	unsigned mode = 0;
	bool forceReset = true;
	unsigned short xse = 0;
	
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
			case 'W': case 'w':
				mode |= WRITE_MODE;
				break;
			case 'F': case 'f':
				forceReset = true;
				break;
			case 'N': case 'n':
				forceReset = false;
				break;
			case 'Q': case 'q': // Quadraphonic (4 channels)
				xse = 0x100;    // TXSE/RXSE bit set
				break;
			case 'S': case 's': // Stereo (2 channels)
				xse = 0x0;      // TXSE/RXSE bit clear
				break;
			case ' ': case '\t': case ',':
				break;		// ignore punctuation and whitespace
		}
		++pStr;
	}

	// openMode is the current "overall" state of the driver
	unsigned openMode = m_readFlags | m_writeFlags;

	// The driver only allows one open for reading and one for writing
	if (mode & openMode & (READ_MODE | WRITE_MODE))
		return -1; 			// already open in this mode

	// Initialisation of the codec is only performed if the device is
	// not currently open.
	if (0 == openMode)  	// Not open yet
	{
		// Setup the codec's internal registers
		init1836(forceReset, sCodec1836TxRegs, DIM(sCodec1836TxRegs));
	}

	// Store the open mode in the device handle
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(mode);

	// Read-specific initialisations
	if (mode & READ_MODE)
	{
		// Sport0 receive configuration
		*pSPORT0_RCR1 = RFSR | LRFS | RCKFE;   // Ext. CLK, Ext. Frame sync, MSB first, Active Low
		*pSPORT0_RCR2 = SLEN_16 | RSFSE | xse; // 16-bit data, Stereo frame sync enable

		// Map DMA1 to Sport0 RX
		*pDMA1_PERIPHERAL_MAP = 0x1000;
		
		m_readBuffers = 0;
		m_readFlags = mode;
	}

	// Write-specific initialisations
	if (mode & WRITE_MODE)
	{
		// Sport0 transmit configuration
		*pSPORT0_TCR1 = TFSR | LTFS | TCKFE;   // Ext. CLK, Ext. Frame sync, MSB first, Active Low
		*pSPORT0_TCR2 = SLEN_16 | TSFSE | xse; // 16-bit data, Stereo frame sync enable
		
		// Map DMA2 to Sport0 TX
		*pDMA2_PERIPHERAL_MAP = 0x2000;

		m_writeBuffers = 0;
		m_writeFlags = mode;
	}

	return 0;
}

int AD1836::Close (const VDK::DispatchUnion &inUnion)
{
	// Retrieve the open mode from the device handle
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.OpenClose_t.dataH);

	if (mode != (mode & (m_readFlags | m_writeFlags)))
		return -1;

	VDK::DispatchUnion outUnion;
	outUnion.ReadWrite_t.dataH    = inUnion.OpenClose_t.dataH;
	outUnion.ReadWrite_t.timeout  = 0;
	outUnion.ReadWrite_t.dataSize = 0;
	outUnion.ReadWrite_t.data     = NULL;

	if (mode & READ_MODE)
	{
		SyncRead(outUnion); // wait for any current read to complete
		m_readFlags = 0;
	}

	if (mode & WRITE_MODE)
	{
		SyncWrite(outUnion); // wait for any current write to complete
		m_writeFlags = 0;
	}

	*inUnion.OpenClose_t.dataH = 0;
	
	return 0;
}


//
// This function implements pipelined 'read' behaviour for the AD1836 device.
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
int AD1836::SyncRead(const VDK::DispatchUnion &inUnion)
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
 			c_nextRxAddr  = m_vReadNextBuffStart;
 			c_nextRxCount = m_vReadNextBuffCount;
 		}
			
		// Block on the device flag (this re-enables interrupts)			
		VDK::PendDeviceFlag(m_DevFlagRead, inUnion.ReadWrite_t.timeout);
	}
	else // m_readBuffers is zero
	{
		// ...otherwise we need to start the transfer by setting up DMA1
		// If the buffer size is zero then we don't do anything
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			// Setup the first DMA transfer in the sequence
			m_readBuffers = 1;
			*pDMA1_START_ADDR = m_vReadNextBuffStart; // Start address of data buffer
			*pDMA1_X_COUNT = m_vReadNextBuffCount;    // DMA inner loop count
			*pDMA1_X_MODIFY	= m_sampleBytes;          // Inner loop address increment 
	
			// Enable DMA for input, with 16-bit transfers and interrupt on completion
			*pDMA1_CONFIG = WNR | WDSIZE_16 | DI_EN | DMAEN;
			
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
// This function implements pipelined 'write' behaviour for the AD1836 device.
//
// On entry, the m_writeBuffers member variable will always contain either
// zero or one. One means that a pipelined write (started by the previous call)
// is in progress, so we will block until it completes. Zero means that no
// transfer is in progress, so we will simply start the new transfer and return.
//
// Note that although m_writeBuffers may be set to 2 immediately prior to
// blocking on the device flag, it will always have been decremented (in 
// Actvate()) before the device flag is posted, so that m_writeBuffers will
// always be 0 or 1 when we leave this function.
//
int AD1836::SyncWrite(const VDK::DispatchUnion &inUnion)
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
 			c_nextTxAddr  = m_vWriteNextBuffStart;
 			c_nextTxCount = m_vWriteNextBuffCount;
 		}

		// Block on the device flag (this re-enables interrupts)			
		VDK::PendDeviceFlag(m_DevFlagWrite, inUnion.ReadWrite_t.timeout);
	}
	else // m_writeBuffers is zero
	{
		// ...otherwise we need to start the transfer by setting up DMA2

		// If the buffer size is zero then we don't do anything
		if (0 != inUnion.ReadWrite_t.dataSize)
		{
			// Setup the first DMA transfer in the sequence
			m_writeBuffers = 1;
			*pDMA2_START_ADDR = m_vWriteNextBuffStart; // Start address of data buffer
			*pDMA2_X_COUNT = m_vWriteNextBuffCount;    // DMA inner loop count
			*pDMA2_X_MODIFY	= m_sampleBytes;           // Inner loop address increment
	
			// Enable DMA for output, with 16-bit transfers and interrupt on completion
			*pDMA2_CONFIG = WDSIZE_16 | DI_EN | DMAEN;
			
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
// In future it may support updates to the codec control registers.
//
int AD1836::IOCtl    (const VDK::DispatchUnion &inUnion)
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
// sources are active. We do this by looking at the dma1_irq_status
// and dma2_irq_status variables, which hold the contents of the
// dma irq status registers at the time of the most recent IVG9
// interrupt (and prior to the interrupts being cleared.
//
void AD1836::Activate (const VDK::DispatchUnion &inUnion)
{
	// Check DMA1 status (serial port 0 Rx)
	if (DMA_DONE & c_vDma1IrqStatus[m_devNo]) // input buffer is full
	{
		if (2 == m_readBuffers) // then there is another buffer waiting
		{
			// The next DMA transfer has been started by the ISR
			m_readBuffers = 1;
		}
		else
		{			
			// No more buffers, disable Sport0 RX
			*pDMA1_CONFIG = WNR | WDSIZE_16 | DI_EN;  // disable DMA1	
			*pSPORT0_RCR1 &= ~RSPEN;                  // disable Sport0
			m_readBuffers = 0;
		}

		// Post Rx device flag
		VDK::PostDeviceFlag(m_DevFlagRead);
	}

	// Check DMA2 status (serial port 0 Tx)
	if (DMA_DONE & c_vDma2IrqStatus[m_devNo]) // output buffer has emptied
	{
		if (2 == m_writeBuffers) // then there is another buffer waiting
		{
			// The next DMA transfer has been started by the ISR
			m_writeBuffers = 1;
		}
		else
		{			
			// No more buffers, disable Sport0 TX
			*pDMA2_CONFIG = WDSIZE_16 | DI_EN;  // disable DMA1	
			*pSPORT0_TCR1 &= ~TSPEN;            // disable Sport0
			m_writeBuffers = 0;
		}

		// Post Tx device flag
		VDK::PostDeviceFlag(m_DevFlagWrite);
	}
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
