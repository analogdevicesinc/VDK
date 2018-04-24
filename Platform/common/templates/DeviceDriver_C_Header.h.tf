/* =============================================================================
 *
 *  Description: This is a C++ to C Header file for Device Driver %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _%Name%_H_
#define _%Name%_H_

#include "VDK.h"

#ifdef VDK_INCLUDE_IO_


#ifdef __cplusplus
#include <new>

extern "C" void* %Name%_DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion inUnion);
#ifdef VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class %Name%_DriverWrapper : public VDK::DeviceDriver
{
public:
    void* DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
    { return (%Name%_DispatchFunction(inCode,inUnion)); }

	static VDK::IOAbstractBase* Create()
	{
#ifndef VDK_MULTIPLE_HEAPS_
		return(new (%Name%_DriverWrapper));
#else
		return(new (VDK::g_IOObjectHeap) (%Name%_DriverWrapper));
#endif
	}
};

#else /* C access */ 
extern "C" void* %Name%_DispatchFunction(VDK_DispatchID inCode, VDK_DispatchUnion inUnion);
#endif  /* __cplusplus */

#endif /* VDK_INCLUDE_IO_ */
#endif /* _%Name%_H_ */

/* ========================================================================== */
