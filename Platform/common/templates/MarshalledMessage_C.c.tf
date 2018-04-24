/* =============================================================================
 *
 *  Description: This is a C implementation of a custom marshalled message 
 *  type for %TypeName%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/
#include <VDK.h>

#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")

extern int %Name% (VDK_MarshallingCode, VDK_MsgWireFormat*, VDK_DeviceInfoBlock*, unsigned int, VDK_Ticks);

int %Name% (VDK_MarshallingCode code, VDK_MsgWireFormat *pMsgPacket, VDK_DeviceInfoBlock *pDev, unsigned int area, VDK_Ticks timeout) {

	// TODO - Put the marshaller's "main" body HERE
	return 0;
}

