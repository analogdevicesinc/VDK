/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _%Name%_H_
#define _%Name%_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class %Name% : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (%Name%));
#else
		return(new (VDK::g_IOObjectHeap) (%Name%));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _%Name%_H_ */

/* ========================================================================== */
