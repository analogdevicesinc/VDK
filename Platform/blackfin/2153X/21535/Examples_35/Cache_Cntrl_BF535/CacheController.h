/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for CacheController
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _CacheController_H_
#define _CacheController_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class CacheController : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (CacheController));
#else
		return(new (VDK::g_IOObjectHeap) (CacheController));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _CacheController_H_ */

/* ========================================================================== */
