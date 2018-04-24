/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for IMDMADevice
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "IMDMADevice.h"
#include <cdefBF561.h>

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* globals */

/* statics */

// [0] incremented before pend on device flag, [1] incremented after
unsigned IMDMA_DstCount[2] = {0, 0};
unsigned IMDMA_SrcCount[2] = {0, 0};

/* externs */

/* preprocessor macros */
#define READ_MODE		(1 << 0)
#define WRITE_MODE		(1 << 1)

#define DST_CONFIG(ch)        (*((0 == (ch)) ? pIMDMA_D0_CONFIG        : pIMDMA_D1_CONFIG))
#define DST_NEXT_DESC_PTR(ch) (*((0 == (ch)) ? pIMDMA_D0_NEXT_DESC_PTR : pIMDMA_D1_NEXT_DESC_PTR))
#define DST_START_ADDR(ch)    (*((0 == (ch)) ? pIMDMA_D0_START_ADDR    : pIMDMA_D1_START_ADDR))
#define DST_X_COUNT(ch)	      (*((0 == (ch)) ? pIMDMA_D0_X_COUNT       : pIMDMA_D1_X_COUNT))
#define DST_X_MODIFY(ch)      (*((0 == (ch)) ? pIMDMA_D0_X_MODIFY      : pIMDMA_D1_X_MODIFY))
#define DST_Y_COUNT(ch)	      (*((0 == (ch)) ? pIMDMA_D0_Y_COUNT       : pIMDMA_D1_Y_COUNT))
#define DST_Y_MODIFY(ch)      (*((0 == (ch)) ? pIMDMA_D0_Y_MODIFY      : pIMDMA_D1_Y_MODIFY))
#define DST_CURR_DESC_PTR(ch) (*((0 == (ch)) ? pIMDMA_D0_CURR_DESC_PTR : pIMDMA_D1_CURR_DESC_PTR))
#define DST_CURR_ADDR(ch)     (*((0 == (ch)) ? pIMDMA_D0_CURR_ADDR     : pIMDMA_D1_CURR_ADDR))
#define DST_CURR_X_COUNT(ch)  (*((0 == (ch)) ? pIMDMA_D0_CURR_X_COUNT  : pIMDMA_D1_CURR_X_COUNT))
#define DST_CURR_Y_COUNT(ch)  (*((0 == (ch)) ? pIMDMA_D0_CURR_Y_COUNT  : pIMDMA_D1_CURR_Y_COUNT))
#define DST_IRQ_STATUS(ch)	  (*((0 == (ch)) ? pIMDMA_D0_IRQ_STATUS    : pIMDMA_D1_IRQ_STATUS))
#define SRC_CONFIG(ch)        (*((0 == (ch)) ? pIMDMA_S0_CONFIG        : pIMDMA_S1_CONFIG))
#define SRC_NEXT_DESC_PTR(ch) (*((0 == (ch)) ? pIMDMA_S0_NEXT_DESC_PTR : pIMDMA_S1_NEXT_DESC_PTR))
#define SRC_START_ADDR(ch)    (*((0 == (ch)) ? pIMDMA_S0_START_ADDR    : pIMDMA_S1_START_ADDR))
#define SRC_X_COUNT(ch)	      (*((0 == (ch)) ? pIMDMA_S0_X_COUNT       : pIMDMA_S1_X_COUNT))
#define SRC_X_MODIFY(ch)      (*((0 == (ch)) ? pIMDMA_S0_X_MODIFY      : pIMDMA_S1_X_MODIFY))
#define SRC_Y_COUNT(ch)	      (*((0 == (ch)) ? pIMDMA_S0_Y_COUNT       : pIMDMA_S1_Y_COUNT))
#define SRC_Y_MODIFY(ch)      (*((0 == (ch)) ? pIMDMA_S0_Y_MODIFY      : pIMDMA_S1_Y_MODIFY))
#define SRC_CURR_DESC_PTR(ch) (*((0 == (ch)) ? pIMDMA_S0_CURR_DESC_PTR : pIMDMA_S1_CURR_DESC_PTR))
#define SRC_CURR_ADDR(ch)     (*((0 == (ch)) ? pIMDMA_S0_CURR_ADDR     : pIMDMA_S1_CURR_ADDR))
#define SRC_CURR_X_COUNT(ch)  (*((0 == (ch)) ? pIMDMA_S0_CURR_X_COUNT  : pIMDMA_S1_CURR_X_COUNT))
#define SRC_CURR_Y_COUNT(ch)  (*((0 == (ch)) ? pIMDMA_S0_CURR_Y_COUNT  : pIMDMA_S1_CURR_Y_COUNT))
#define SRC_IRQ_STATUS(ch)	  (*((0 == (ch)) ? pIMDMA_S0_IRQ_STATUS    : pIMDMA_S1_IRQ_STATUS))

#ifdef COREA
#define SIC_IMASK0 (*(pSICA_IMASK0)) // SIC Interrupt Mask register 0
#define SIC_IMASK1 (*(pSICA_IMASK1)) // SIC Interrupt Mask register 1
#else
#define SIC_IMASK0 (*(pSICB_IMASK0)) // SIC Interrupt Mask register 0
#define SIC_IMASK1 (*(pSICB_IMASK1)) // SIC Interrupt Mask register 1
#endif

//
// This code manages a spinlock which synchronizes the two cores
// for access to the D0 DMA engine. The lock is implemented according to
// Peterson's Solution and uses a "turn" variable and an "interest" array
// located in shared (L2) memory. This memory must be uncached.
//
namespace VDK
{
	extern unsigned int g_localNode;
};

#define claim_atomic_access() enter_region(VDK::g_localNode);
#define release_atomic_access() leave_region(VDK::g_localNode);
extern volatile int turn;
extern volatile int interested[2];
extern volatile short D0WasDisabled,S0EnabledCount;

static void enter_region(int node)
{
	int other = 1 - node;
	
	interested[node] = true;
	turn = node;
	while (turn == node && interested[other]) ; // spin
}

static void leave_region(int node)
{
	interested[node] = false;
}

// Due to silicon anomaly 05000149 (IMDMA S1/D1 channel may stall) the 
// following has to be done

// D0WasDisabled is set when D0 is forcibly disabled because there is
// no S0 data for it to receive and we don't want it to hang S1/D1
//
// S0EnabledCount is incremented each time Core A enables S0; Core B decrements
// it each time a D0 read is satisfied (ie when Core B gets the data, not when
// Core A thinks it has been sent)
//
// checkD0EngineState() is called every time the driver updates any of the
// IMDMA state (S0/S1/D0/D1). Its aim is to ensure that D0 is never enabled
// when there is no S0 data still to be delivered, since that hangs S1/D1. 
static void checkD0EngineState()
{
    int D0ActuallyEnabled = ((DMAEN & DST_CONFIG(0)) != 0);
	
	if (S0EnabledCount == 0 && D0ActuallyEnabled)
	{
        // all S0 data delivered : need to disable D0
		DST_CONFIG(0) &= ~DMAEN;
        D0WasDisabled = 1;
	}
	
	if (S0EnabledCount != 0 && !D0ActuallyEnabled && D0WasDisabled)
	{
        // new S0 data after D0 was forcibly disabled : re-enable D0
		DST_CONFIG(0) |= DMAEN;
        D0WasDisabled = 0;
	}
}


/******************************************************************************
 * Dispatch Function for the IMDMADevice Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
IMDMADevice::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    int    ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
        	ret_val = Init(inUnion);
            break;
        case VDK::kIO_Activate:  
        	ret_val = Activate(inUnion);
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


int IMDMADevice::Init (const VDK::DispatchUnion &inUnion)
{
	m_flags  = 0;
	m_devFlag = VDK::CreateDeviceFlag();
	m_chNum = *((long *)inUnion.Init_t.pInitInfo);
	SIC_IMASK1 = 0x06000000;
	//SIC_IMASK0 = 0x00000008; // IDMA error interrupt
	D0WasDisabled  = 0;
    S0EnabledCount = 0;
	return 0;
}


int IMDMADevice::Open (const VDK::DispatchUnion &inUnion)
{
	unsigned mode = 0;
	
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
			case ' ': case '\t': case ',':
				break;		// ignore punctuation and whitespace
		}
		++pStr;
	}

	if (0 != m_flags) // (mode & openMode & (READ_MODE | WRITE_MODE)) - no split-open
		return -1; 			// already open in this mode

	// Not open yet
	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(mode);
	m_flags = mode;

	return 0;
}

int IMDMADevice::Close (const VDK::DispatchUnion &inUnion)
{
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.OpenClose_t.dataH);

	if (mode != (mode & m_flags))
		return -1;

	m_flags = 0;
	*inUnion.OpenClose_t.dataH = 0;
	
	return 0;
}

int IMDMADevice::SyncRead(const VDK::DispatchUnion &inUnion)
{
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);
	
	if (0 == (mode & READ_MODE))  // must be open for reading
		return -1;

	m_dataSize = inUnion.ReadWrite_t.dataSize;
    m_pData  =
    	reinterpret_cast<unsigned long *>(inUnion.ReadWrite_t.data);

    VDK::LogHistoryEvent((VDK::HistoryEnum)(VDK::kUserEvent+1), m_dataSize);

	VDK::PushCriticalRegion(); // Prevent Activate() from changing stuff

  	// Set up the DMA for the transfer
  	//
	claim_atomic_access();
    DST_START_ADDR(m_chNum) = m_pData;
	DST_X_COUNT(m_chNum)    = m_dataSize >> 2;
	DST_X_MODIFY(m_chNum)   = 4;
	DST_CONFIG(m_chNum)     = DI_EN | WDSIZE_32 | WNR | DMAEN;
	checkD0EngineState();
	release_atomic_access();

	++IMDMA_DstCount[0];
	
    // Wait for the DMA to complete, which will be serviced by Activate()
    VDK::PendDeviceFlag (m_devFlag, inUnion.ReadWrite_t.timeout);

	VDK::PushCriticalRegion();
    
	claim_atomic_access();
	DST_CONFIG(m_chNum) = DI_EN | WDSIZE_32 | WNR;
    if (m_chNum == 0)
      S0EnabledCount -= 1;
    checkD0EngineState();
	release_atomic_access();
	++IMDMA_DstCount[1];
    
    VDK::PopCriticalRegion();
    	
	return inUnion.ReadWrite_t.dataSize;
}

int IMDMADevice::SyncWrite(const VDK::DispatchUnion &inUnion)
{
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);

	if (0 == (mode & WRITE_MODE))  // must be open for reading
		return -1;

    m_dataSize = inUnion.ReadWrite_t.dataSize;
    m_pData =
    	reinterpret_cast<unsigned long *>(inUnion.ReadWrite_t.data);

    VDK::LogHistoryEvent(VDK::kUserEvent, m_dataSize);
    
	VDK::PushCriticalRegion(); // Prevent Activate() from changing stuff

  	// Set up the DMA for the transfer
  	//
	claim_atomic_access();
    SRC_START_ADDR(m_chNum) = m_pData;
	SRC_X_COUNT(m_chNum)    = m_dataSize >> 2;
	SRC_X_MODIFY(m_chNum)   = 4;
	SRC_CONFIG(m_chNum)     = DI_EN | WDSIZE_32 | DMAEN;
    if (m_chNum == 0)
      S0EnabledCount += 1;
	checkD0EngineState();
	release_atomic_access();
    ++IMDMA_SrcCount[0];
    
    // Wait for the DMA to complete, which will be serviced by Activate()
    VDK::PendDeviceFlag (m_devFlag, inUnion.ReadWrite_t.timeout);
    
	VDK::PushCriticalRegion();
    
	claim_atomic_access();
	SRC_CONFIG(m_chNum) = DI_EN | WDSIZE_32;
	checkD0EngineState();
	release_atomic_access();
	++IMDMA_SrcCount[1];
    
    VDK::PopCriticalRegion();
    
	return inUnion.ReadWrite_t.dataSize;
}

int IMDMADevice::IOCtl    (const VDK::DispatchUnion &inUnion)
{
	return 0;
}


int IMDMADevice::Activate (const VDK::DispatchUnion &inUnion)
{
	VDK::PostDeviceFlag(m_devFlag);

	return 0;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
