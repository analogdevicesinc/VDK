/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for AD1854
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _AD1854_H_
#define _AD1854_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class AD1854 : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (AD1854));
#else
		return(new (VDK::g_IOObjectHeap) (AD1854));
#endif
	}

private:
	// Internal member functions
    void Init     (const VDK::DispatchUnion &inUnion);
    void Activate (const VDK::DispatchUnion &inUnion);
    int  Open     (const VDK::DispatchUnion &inUnion);
    int  Close    (const VDK::DispatchUnion &inUnion);
    int  SyncWrite(const VDK::DispatchUnion &inUnion);
    int  IOCtl    (const VDK::DispatchUnion &inUnion);
    
    // Instance variables
    int m_devNo;
    int m_sampleBytes;
	unsigned short m_wdSize;
	unsigned short m_slen;

	DeviceFlagID m_DevFlagWrite;
	unsigned m_writeFlags;
	int  m_writeBuffers;
	short   *m_vWriteCurrBuffStart;
	short   *m_vWriteNextBuffStart;
	unsigned m_vWriteCurrBuffCount;
	unsigned m_vWriteNextBuffCount;

	// Class variables
	static volatile unsigned short c_vDmaIrqStatus[];
	static volatile short *c_nextAddr;
	static volatile short  c_nextCount;
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _AD1854_H_ */

/* ========================================================================== */
