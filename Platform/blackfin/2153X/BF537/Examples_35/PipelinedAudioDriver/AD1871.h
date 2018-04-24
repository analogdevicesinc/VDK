/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for AD1871
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _AD1871_H_
#define _AD1871_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class AD1871 : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (AD1871));
#else
		return(new (VDK::g_IOObjectHeap) (AD1871));
#endif
	}
	
private:
	// Internal member functions
    void Init     (const VDK::DispatchUnion &inUnion);
    void Activate (const VDK::DispatchUnion &inUnion);
    int  Open     (const VDK::DispatchUnion &inUnion);
    int  Close    (const VDK::DispatchUnion &inUnion);
    int  SyncRead (const VDK::DispatchUnion &inUnion);
    int  IOCtl    (const VDK::DispatchUnion &inUnion);
    
    // Instance variables
    int m_devNo;
    int m_sampleBytes;
	unsigned short m_wdSize;
	unsigned short m_slen;
    
	DeviceFlagID m_DevFlagRead;
	unsigned m_readFlags;
	int  m_readBuffers;
	short   *m_vReadCurrBuffStart;
	short   *m_vReadNextBuffStart;
	unsigned m_vReadCurrBuffCount;
	unsigned m_vReadNextBuffCount;
	
	// Class variables
	static volatile unsigned short c_vDmaIrqStatus[];
	static volatile short *c_nextAddr;
	static volatile short  c_nextCount;
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _AD1871_H_ */

/* ========================================================================== */
