/* =============================================================================
 *
 *  Description: This is a C++ implementation of a custom marshalled message 
 *  type for %TypeName%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include <VDK.h>

#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")

extern "C" int %Name% (VDK::MarshallingCode, VDK::MsgWireFormat*, VDK::DeviceInfoBlock*, unsigned int, VDK::Ticks);

int %Name% (VDK::MarshallingCode code, VDK::MsgWireFormat *pMsgPacket, VDK::DeviceInfoBlock *pDev, unsigned int area, VDK::Ticks timeout) {

	// TODO - Put the marshaller's "main" body HERE
	return 0;

}

