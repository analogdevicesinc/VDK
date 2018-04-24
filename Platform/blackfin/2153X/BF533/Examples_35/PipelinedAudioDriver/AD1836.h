/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for AD1836
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _AD1836_H_
#define _AD1836_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class AD1836 : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static IOAbstractBase* Create()
	{
		return(new (_STD nothrow) (AD1836));
	}
	
private:
	// Internal member functions
    void Init     (const VDK::DispatchUnion &inUnion);
    void Activate (const VDK::DispatchUnion &inUnion);
    int  Open     (const VDK::DispatchUnion &inUnion);
    int  Close    (const VDK::DispatchUnion &inUnion);
    int  SyncRead (const VDK::DispatchUnion &inUnion);
    int  SyncWrite(const VDK::DispatchUnion &inUnion);
    int  IOCtl    (const VDK::DispatchUnion &inUnion);
    
    // Instance variables
    int m_devNo;
    int m_sampleBytes;
    
	DeviceFlagID m_DevFlagRead;
	unsigned m_readFlags;
	int  m_readBuffers;
	short   *m_vReadCurrBuffStart;
	short   *m_vReadNextBuffStart;
	unsigned m_vReadCurrBuffCount;
	unsigned m_vReadNextBuffCount;
	
	DeviceFlagID m_DevFlagWrite;
	unsigned m_writeFlags;
	int  m_writeBuffers;
	short   *m_vWriteCurrBuffStart;
	short   *m_vWriteNextBuffStart;
	unsigned m_vWriteCurrBuffCount;
	unsigned m_vWriteNextBuffCount;
	
	// Class variables
	static volatile unsigned short c_vDma1IrqStatus[];
	static volatile unsigned short c_vDma2IrqStatus[];
 	static volatile short *c_nextRxAddr;
 	static volatile short  c_nextRxCount;
 	static volatile short *c_nextTxAddr;
 	static volatile short  c_nextTxCount;

};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _AD1836_H_ */

/* ========================================================================== */
