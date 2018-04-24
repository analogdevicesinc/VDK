/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for CoreBInit
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _CoreBInit_H_
#define _CoreBInit_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class CoreBInit : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
		return(new (CoreBInit));
#else
		return(new (VDK::g_IOObjectHeap) (CoreBInit));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _CoreBInit_H_ */

/* ========================================================================== */
