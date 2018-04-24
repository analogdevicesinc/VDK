/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for DeviceB
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _DeviceB_H_
#define _DeviceB_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class DeviceB : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (DeviceB));
#else
		return(new (VDK::g_IOObjectHeap) (DeviceB));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _DeviceB_H_ */

/* ========================================================================== */
