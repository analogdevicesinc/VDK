/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for AD1885
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _AD1885_H_
#define _AD1885_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class AD1885 : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static IOAbstractBase* Create()
	{
		return(new (_STD nothrow) (AD1885));
	}
	
private:
	// Internal member functions
    int Init     (const VDK::DispatchUnion &inUnion);
    int Activate (const VDK::DispatchUnion &inUnion);
    int Open     (const VDK::DispatchUnion &inUnion);
    int Close    (const VDK::DispatchUnion &inUnion);
    int SyncRead (const VDK::DispatchUnion &inUnion);
    int SyncWrite(const VDK::DispatchUnion &inUnion);
    int IOCtl    (const VDK::DispatchUnion &inUnion);
    
    // Instance data
	DeviceFlagID m_DevFlagRead;
	unsigned m_readFlags;
	int  m_readBuffers;
	short *m_vReadCurrBuffStart;
	short *m_vReadNextBuffStart;
	unsigned m_vReadCurrBuffSize;
	unsigned m_vReadNextBuffSize;
	
	DeviceFlagID m_DevFlagWrite;
	unsigned m_writeFlags;
	int  m_writeBuffers;
	short *m_vWriteCurrBuffStart;
	short *m_vWriteNextBuffStart;
	unsigned m_vWriteCurrBuffSize;
	unsigned m_vWriteNextBuffSize;
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _AD1885_H_ */

/* ========================================================================== */
