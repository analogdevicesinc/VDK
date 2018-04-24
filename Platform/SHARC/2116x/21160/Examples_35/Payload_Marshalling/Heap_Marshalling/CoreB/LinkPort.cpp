/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for LinkPort
 *
 * -----------------------------------------------------------------------------
 *  Comments: This driver is intended to support multiprocessor messaging,
 *  and provided only the factlities required for that purpose. In particular,
 *  the device is never expected to be closed.
 *  It is a prototype and is not intended to be a fully-functional, general-
 *  -purpose linkport driver.
 *
 * ===========================================================================*/

#include <def21160.h>
#include <cdef21160.h>
#include <sysreg.h>
#include <builtins.h>
#include <ctype.h>

#include "LinkPort.h"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

// Flag bits
#define READ_MODE		(1 << 0)
#define WRITE_MODE		(1 << 1)

struct DMARegs
{
	void        *II; // Memory address
	unsigned int IM; // Modify value
	unsigned int C;  // Transfer count
	unsigned int CP; // Chaining pointer
	unsigned int GP; // General purpose register
	unsigned int DB;
	unsigned int DA;
};

// Transfer Control Block, used for chained DMA.
// For some reason the fields are in the reverse order
// in memory from what you would expect.
//
struct TCB
{
	unsigned int GP; // General purpose register
	unsigned int CP; // Chaining pointer
	unsigned int C;  // Transfer count
	unsigned int IM; // Modify value
	void        *II; // Memory address
};

//
// Lookup table for linkbuffer number->DMA channel address mapping
// Link buffers 0-5 use DMA channels 4-9
static volatile DMARegs *vDMA[6] =
{
	(volatile DMARegs *)pII4, // DMA Channel 4 register base address (Link 0)
	(volatile DMARegs *)pII5, // DMA Channel 5 register base address (Link 1)
	(volatile DMARegs *)pII6, // DMA Channel 6 register base address (Link 2)
	(volatile DMARegs *)pII7, // DMA Channel 7 register base address (Link 3)
	(volatile DMARegs *)pII8, // DMA Channel 8 register base address (Link 4)
	(volatile DMARegs *)pII9  // DMA Channel 9 register base address (Link 5)
};

// Workaround for ADSP-21160 anomaly 43: "Link port buffers and buffer
// status bits may not behave properly upon RESET affecting initial
// link port data transmission".
//
// Workaround description:
// "The execution of a simple core driven link port loopback transfer
// between any two link port buffers will properly clear the LBUFx
// status for the two link buffers. Internal loopback operation also
// prevents the transmitting link port from driving data out to
// another connected external link port receiver. After the link port
// loopback completes, the link port can then be reprogrammed to
// operate in the required mode of transfer."
//
static void flushLinkBuffers()
{
	static bool buffersFlushed = false;

	if (!buffersFlushed)
	{
		buffersFlushed = true;
	}

	// Enable 3 buffers as transmitters and 3 as receivers without any
	// DMA or Interrupt service routines
	*pLCTL0 = 0x00902409;
	*pLCTL1 = 0x00100401;

	// Establish loopback mode transfer between them (i.e., assign one
	// transmit buffer and one receive buffer to the same link port).
	*pLAR = 0x11088;

	// Write 2 dummy words to each transmit buffer.
	*pLBUF0 = 0;
	*pLBUF1 = 0;
	*pLBUF2 = 0;
	*pLBUF0 = 0;
	*pLBUF1 = 0;
	*pLBUF2 = 0;

	// Give enough time for data transfer to complete.
	static int dummy;
	volatile int *pVolDummy = &dummy;
	for (int i = 0; i < 50; i++)
		*pVolDummy = 0;

	// Read two words from each receiver.
	unsigned d0A = *pLBUF3;
	unsigned d1A = *pLBUF4;
	unsigned d2A = *pLBUF5;
	unsigned d0B = *pLBUF3;
	unsigned d1B = *pLBUF4;
	unsigned d2B = *pLBUF5;

	// After the internal loopback mode transfer completes, disable
	// the link ports.
	*pLCTL0 = 0x0;
	*pLCTL1 = 0x0;

	// Restore the default buffer->port mapping.
	*pLAR   = 0x2C688;

	// Clear the pending interrupts in LIRPTL
	asm("BIT CLR LIRPTL 0x0000003F;");
}

// Function to write the appropriate link control bits for the
// specified link buffer. The bits are expected to be in bits 0-9
// of argument 'val'. The caaller may therefroe specify these using
// constants for link 0 (L0EN, L0TRAN, etc.).
//	
static inline void lctl_write (unsigned linkNo, unsigned int val)
{
	// Read the relevant link control register
	unsigned lctl = *((linkNo < 3) ? pLCTL0 : pLCTL1);
	unsigned bitpos = (linkNo % 3) * 10;   // position within register

	// Mask out the bits we are replacing
	lctl &= ~(0x3FF << bitpos);

	// Merge in the new bits
	lctl |= (val & 0x3FF) << bitpos;

	// Write back the link control register
	*((linkNo < 3) ? pLCTL0 : pLCTL1) = lctl;
}

// Sets up a simple DMA transfer for the specified link buffer. Only the
// buffer address and count are passed in as the other transfer parameters
// are constants.
//
static inline void setup_dma (unsigned linkNo, void *address, int count)
{
	// Write to the DMA registers
	vDMA[linkNo]->CP = 0;
	vDMA[linkNo]->GP = 0;
	vDMA[linkNo]->DB = 0;
	vDMA[linkNo]->II = address;
	vDMA[linkNo]->IM = 1;      // modify value
	vDMA[linkNo]->C  = count;
	// DA is initialised automatically when CP is written
}


/******************************************************************************
 * Dispatch function for the LinkPort Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
void*
LinkPort::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
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


/******************************************************************************
 * Init function for the LinkPort Device Driver
 *
 * Initializes instance variables.
 * 
 */
void LinkPort::Init (const VDK::DispatchUnion &inUnion)
{
	// Run the workaround code (once only)
	flushLinkBuffers();
	
	// Clear the flag bits
	m_flags = 0;

	// Create the deviceflags
	m_devFlag = VDK::CreateDeviceFlag();

	// Get the link number from the boot I/O object initializer
	m_linkNo = *((long *)inUnion.Init_t.pInitInfo);
}


/******************************************************************************
 * Open function for the LinkPort Device Driver
 *
 * Parses and validates the open mode and enables the linkport hardware.
 * Note that if no speed digit is specified in the open string then the
 * device will default to quarter-speed operation (clock divisor = 4) for
 * maximum reliability. The default transfer width is 8 bits and can be
 * overridden to use 4 bits by specifying 'N' (nybble) in the open string.
 */
int LinkPort::Open (const VDK::DispatchUnion &inUnion)
{
	// This device may only be open for reading *or* writing at a
	// given time. Any attempt to open for reading and writing
	// simultaneously is invalid. This applies both to a single open
	// (i.e. with "RW") or to separate opens (i.e. "split-open").
	//
	if (0 != m_flags)
		return -1; 			// already open

	unsigned mode  = 0;
	unsigned size  = L0DPWID; // default to 8-bit mode
	unsigned speed = 0;       // default to quarter speed

	// Parse the mode string to determine the open mode.
	char *pStr = inUnion.OpenClose_t.flags;

	while (*pStr)
	{
		switch (toupper(*pStr))
		{
			default:
				return -1;	// unknown option
			case 'R': case 'r':
				mode |= READ_MODE;
				break;
			case 'W': case 'w':
				mode |= WRITE_MODE;
				break;
		    case 'N': case 'n':
				size = 0;        // 4-bit mode flag for Tx and Rx
				break;
		    case 'B': case 'b':
				size = L0DPWID;  // 8-bit mode flag for Tx and Rx
				break;
		    case '1': case '2': case '3': case '4':
				speed = 0x3 & (*pStr - '0'); // range 0-3 (0 == '4')
				break;
			case ' ': case '\t': case ',':
				break;		// ignore punctuation and whitespace
		}
		++pStr;
	}

	if (mode != READ_MODE && mode != WRITE_MODE)
		return -1;

	// Read-specific initializations
	if (READ_MODE == mode)
	{
		// Enable the linkport for reading
		m_lctl = L0EN | size | speed;
	}

	// Write-specific initalizations
	if (WRITE_MODE == mode)
	{
		// Enable the linkport for writing
		m_lctl = L0EN | L0TRAN | size | speed;
	}

	lctl_write(m_linkNo, m_lctl);
	m_flags = mode;

	return 0;
}


/******************************************************************************
 * Close function for the LinkPort Device Driver
 *
 * This is a messaging device driver, and so is never expected to be closed.
 * Therefore this function currently contains only the beginnings of what
 * would be needed to close the device down cleanly.
 */
int LinkPort::Close (const VDK::DispatchUnion &inUnion)
{
	if (READ_MODE == m_flags)
	{
		// need to disable reading on the linkport here
		m_flags = 0;
	}

	if (WRITE_MODE == m_flags)
	{
		// need to disable writing on the linkport here
		m_flags = 0;
	}
	
	return 0;
}


/******************************************************************************
 * SyncRead function for the LinkPort Device Driver
 *
 * Sets up a DMA transfer and blocks awaiting completion.
 */
int LinkPort::SyncRead(const VDK::DispatchUnion &inUnion)
{
	if (0 == (m_flags & READ_MODE))  // must be open for reading
		return -1;
	
	// Store the transfer arguments in instance variables
    m_dataSize = inUnion.ReadWrite_t.dataSize;
    m_pData = reinterpret_cast<unsigned long *>(inUnion.ReadWrite_t.data);

  	// Set up the Rx DMA for the transfer
  	//
    VDK::LogHistoryEvent((VDK::HistoryEnum)(VDK::kUserEvent+1), m_dataSize);

	VDK::PushCriticalRegion(); // Prevent Activate() from changing stuff
	
	// Write the setup data to the DMA engine
    setup_dma(m_linkNo, m_pData, m_dataSize);
	lctl_write(m_linkNo, m_lctl | L0DEN);     // enable DMA

	// Enable the linkport interrupt
	VDK::SetInterruptMaskBitsEx(LPISUMI, 1 << m_linkNo);

    // Wait for the DMA to complete, which will be serviced by Activate()
    VDK::PendDeviceFlag (m_devFlag, inUnion.ReadWrite_t.timeout);

	return inUnion.ReadWrite_t.dataSize;
}


/******************************************************************************
 * SyncWrite function for the LinkPort Device Driver
 *
 * Sets up a DMA transfer and blocks awaiting completion.
 */
int LinkPort::SyncWrite(const VDK::DispatchUnion &inUnion)
{
	if (0 == (m_flags & WRITE_MODE))  // must be open for reading
		return -1;

	// Store the transfer arguments in instance variables
    m_dataSize = inUnion.ReadWrite_t.dataSize;
    m_pData = reinterpret_cast<unsigned long *>(inUnion.ReadWrite_t.data);

  	// Set up the Tx DMA for the transfer
  	//
    VDK::LogHistoryEvent(VDK::kUserEvent, m_dataSize);
    
	VDK::PushCriticalRegion(); // Prevent Activate() from changing stuff

	// Write the setup data to the DMA engine
    setup_dma(m_linkNo, m_pData, m_dataSize);
	lctl_write(m_linkNo, m_lctl | L0DEN);     // enable DMA

	// Enable the linkport interrupt
	VDK::SetInterruptMaskBitsEx(LPISUMI, 1 << m_linkNo);

    // Wait for the DMA to complete, which will be serviced by Activate()
    VDK::PendDeviceFlag (m_devFlag, inUnion.ReadWrite_t.timeout);
    
	return inUnion.ReadWrite_t.dataSize;
}


/******************************************************************************
 * IOCtl function for the LinkPort Device Driver
 *
 * No IOCtl functionality at present
 * 
 */
int LinkPort::IOCtl    (const VDK::DispatchUnion &inUnion)
{
	return 0;
}


/******************************************************************************
 * Activate function for the LinkPort Device Driver
 *
 * Called at Kernel level following an activation from an ISR. Disables the
 * link buffer and posts the device flag to indicate DMA completion.
 */
void LinkPort::Activate (const VDK::DispatchUnion &inUnion)
{
	// Disable DMA
	lctl_write(m_linkNo, m_lctl);

	// Disable the linkport interrupt
	VDK::ClearInterruptMaskBitsEx(0, 1 << m_linkNo);

	// post device flag
	VDK::PostDeviceFlag(m_devFlag);
}

/* ========================================================================== */

