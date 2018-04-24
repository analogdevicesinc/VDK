/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for AD1885
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "AD1885.h"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

#define READ_MODE               (1 << 0)
#define WRITE_MODE              (1 << 1)

/* globals */

// All communication between the driver and its ISR is via these globals.
//
extern volatile int g_TxCount_AD1885;
extern volatile short *g_TxPtr_AD1885;
extern volatile int g_RxCount_AD1885;
extern volatile short *g_RxPtr_AD1885;

/* statics */

/* externs */
extern void InitAC97();
extern void InitCodec(void);

/* preprocessor macros */



/******************************************************************************
 * Dispatch Function for the AD1885 Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
AD1885::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
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


int AD1885::Init (const VDK::DispatchUnion &inUnion)
{
	m_readFlags  = 0;
	m_writeFlags = 0;
	m_DevFlagRead  = VDK::CreateDeviceFlag();
	m_DevFlagWrite = VDK::CreateDeviceFlag();
	m_vReadCurrBuffSize  = 0;
	m_vReadNextBuffSize  = 0;
	m_vWriteCurrBuffSize = 0;
	m_vWriteNextBuffSize = 0;
	
	return 0;
}


int AD1885::Open (const VDK::DispatchUnion &inUnion)
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


	unsigned openMode = m_readFlags | m_writeFlags;

	if (mode & openMode & (READ_MODE | WRITE_MODE))
		return -1; 			// already open in this mode

	if (0 == openMode)  	// Not open yet
	{
		InitAC97();
		InitCodec();
	}

	*inUnion.OpenClose_t.dataH = reinterpret_cast<void*>(mode);

	if (mode & READ_MODE)
	{
		m_readBuffers = 0;
		m_readFlags = mode;
	}

	if (mode & WRITE_MODE)
	{
		m_writeBuffers = 0;
		m_writeFlags = mode;
	}

	return 0;
}

int AD1885::Close (const VDK::DispatchUnion &inUnion)
{
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
		SyncRead(outUnion);
		m_readFlags = 0;
	}

	if (mode & WRITE_MODE)
	{
		SyncWrite(outUnion);
		m_writeFlags = 0;
	}

	*inUnion.OpenClose_t.dataH = 0;
	
	return 0;
}

int AD1885::SyncRead(const VDK::DispatchUnion &inUnion)
{
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);

	if (0 == (mode & READ_MODE))  // must be open for reading
		return -1;

	// Prevent Activate() from changing m_readBuffers
	VDK::PushCriticalRegion();

	int prevPendingState = m_readBuffers;

	m_vReadNextBuffStart = (short *)inUnion.ReadWrite_t.data;
	m_vReadNextBuffSize  = inUnion.ReadWrite_t.dataSize >> 2;

	// If the buffer size is zero then we don't count it as
	// a pending buffer
	if (0 != inUnion.ReadWrite_t.dataSize)
	{
		++m_readBuffers;
	}

	// If m_readBuffers was non-zero on entry then input is
	// already underway into a previous buffer...
	if (0 != prevPendingState)
	{
		// ...in which case we have to block until it completes.
		VDK::PendDeviceFlag(m_DevFlagRead, inUnion.ReadWrite_t.timeout);
	}
	else
	{
		// ...otherwise we need to start the transfer by setting up
		// the globals for the ISR.
		g_RxPtr_AD1885       = m_vReadNextBuffStart;
		g_RxCount_AD1885     = m_vReadNextBuffSize;
		VDK::PopCriticalRegion();
	}

	// Now reap the result of the previous operation
	int result = m_vReadCurrBuffSize << 2;
	
	// and shunt the slots up by one
	m_vReadCurrBuffSize  = m_vReadNextBuffSize;
	m_vReadCurrBuffStart = m_vReadNextBuffStart;
	m_vReadNextBuffSize  = 0;
	m_vReadNextBuffStart = NULL;

	return result;
}

int AD1885::SyncWrite(const VDK::DispatchUnion &inUnion)
{
	unsigned mode = reinterpret_cast<unsigned>(*inUnion.ReadWrite_t.dataH);

	if (0 == (mode & WRITE_MODE))  // must be open for reading
		return -1;

	// Prevent Activate() from changing m_writeBuffers
	VDK::PushCriticalRegion();

	int prevPendingState = m_writeBuffers;

	m_vWriteNextBuffStart = (short *)inUnion.ReadWrite_t.data;
	m_vWriteNextBuffSize  = inUnion.ReadWrite_t.dataSize >> 2;

	// If the buffer size is zero then we don't count it as
	// a pending buffer
	if (0 != inUnion.ReadWrite_t.dataSize)
	{
		++m_writeBuffers;
	}

	// If m_writeBuffers was non-zero on entry then input is
	// already underway into a previous buffer...
	if (0 != prevPendingState)
	{
		// ...in which case we have to block until it completes.
		VDK::PendDeviceFlag(m_DevFlagWrite, inUnion.ReadWrite_t.timeout);
	}
	else
	{
		// ...otherwise we need to start the transfer by setting up
		// the globals for the ISR.
		g_TxPtr_AD1885       = m_vWriteNextBuffStart;
		g_TxCount_AD1885     = m_vWriteNextBuffSize;
		VDK::PopCriticalRegion();
	}

	// Now reap the result of the previous operation
	int result = m_vWriteCurrBuffSize << 2;
	
	// and shunt the slots up by one
	m_vWriteCurrBuffSize  = m_vWriteNextBuffSize;
	m_vWriteCurrBuffStart = m_vWriteNextBuffStart;
	m_vWriteNextBuffSize  = 0;
	m_vWriteNextBuffStart = NULL;

	return result;
}

int AD1885::IOCtl    (const VDK::DispatchUnion &inUnion)
{
	return 0;
}


int AD1885::Activate (const VDK::DispatchUnion &inUnion)
{
	// We've been activated because the ISR has reached the end of one of
	// its buffers (Tx or Rx).
	//
	// Activate()'s job is to:
	// - update the driver's instance variables
	// - update the volatile globals to swap buffers
	// - unblock waiting threads
	//
	if (0 == g_TxCount_AD1885) // output buffer has emptied
	{
		if (2 == m_writeBuffers) // there is another buffer waiting
		{
		    // install the new buffer
			g_TxPtr_AD1885   = m_vWriteNextBuffStart;
			g_TxCount_AD1885 = m_vWriteNextBuffSize;
		}

		// Post Tx device flag
		if (m_writeBuffers > 0)
		{
			--m_writeBuffers;
			VDK::PostDeviceFlag(m_DevFlagWrite);
		}
	}

	if (0 == g_RxCount_AD1885) // input buffer is full
	{
		if (2 == m_readBuffers) // there is another buffer waiting
		{
		    // install the new buffer
			g_RxPtr_AD1885   = m_vReadNextBuffStart;
			g_RxCount_AD1885 = m_vReadNextBuffSize;
		}

		// Post Rx device flag
		if (m_readBuffers > 0)
		{
			--m_readBuffers;
			VDK::PostDeviceFlag(m_DevFlagRead);
		}
	}

	return 0;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
