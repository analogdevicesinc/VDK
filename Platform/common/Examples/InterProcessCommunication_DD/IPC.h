/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for IPC
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _IPC_H_
#define _IPC_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

#ifdef  VDK_MULTIPLE_HEAPS_
namespace VDK {
extern HeapID g_IOObjectHeap;
}
#endif

class IPC : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
#ifndef  VDK_MULTIPLE_HEAPS_
                return(new (IPC));
#else
                return(new (VDK::g_IOObjectHeap) (IPC));
#endif
	}
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _IPC_H_ */

/* ========================================================================== */
