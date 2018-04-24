/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for IMDMADevice
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _IMDMADevice_H_
#define _IMDMADevice_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class IMDMADevice : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (IMDMADevice));
#else
		return(new (VDK::g_IOObjectHeap) (IMDMADevice));
#endif
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

	DeviceFlagID m_devFlag;
	unsigned m_flags;
	unsigned int m_dataSize;
	unsigned long *m_pData;
	
	int m_chNum;
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _IMDMADevice_H_ */

/* ========================================================================== */
