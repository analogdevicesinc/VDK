/* =============================================================================
 *
 *  Description: This is a VDK Device Driver Header for LinkPort
 *
 * -----------------------------------------------------------------------------
 *  Comments: This driver is intended to support multiprocessor messaging,
 *  and provided only the factlities required for that purpose. In particular,
 *  there are optimisations for single-quadword transfers, and the device is
 *  never expected to be closed.
 *  It is a prototype and is not intended to be a fully-functional, general-
 *  -purpose linkport driver.
 *
 * ===========================================================================*/

#ifndef _LinkPort_H_
#define _LinkPort_H_

/* Include the C runtime API */
#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include <new>

class LinkPort : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static VDK::IOAbstractBase* Create()
	{
		return(new (_STD nothrow) (LinkPort));
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
    
    // Instance data
	int            m_linkNo;

	DeviceFlagID   m_devFlag; // controls SyncRead()/SyncWrite() blocking
	unsigned       m_flags;
	unsigned       m_lctl;
	unsigned int   m_dataSize;
	unsigned long *m_pData;
};

#endif /* VDK_INCLUDE_IO_ */
#endif /* _LinkPort_H_ */

/* ========================================================================== */
