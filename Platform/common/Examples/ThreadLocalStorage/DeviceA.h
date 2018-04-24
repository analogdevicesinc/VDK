/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for DeviceA
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _DeviceA_H_
#define _DeviceA_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class DeviceA : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (DeviceA));
#else
		return(new (VDK::g_IOObjectHeap) (DeviceA));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _DeviceA_H_ */

/* ========================================================================== */
