/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"
#include "VDK_Abs_Device.h"
#include "VDK_DCB.h"
#include "IODeviceTable.h"

#if defined(__2106x__)
#include <Cdef21060.h>

#define BASE  0x00080000   // start of MP space
#define LIMIT 0x00080000   // size (end) of local space
#define SHIFT 19           // bit shift for procid
#elif defined(__2116x__)
#include <Cdef21160.h>
#define BASE  0x00100000   // start of MP space
#define LIMIT 0x00080000   // size (end) of local space
#define SHIFT 20           // bit shift for procid
#endif

#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")

namespace VDK {

#if defined(__2106x__) || defined(__2116x__)

int ClusterBusMarshaller (VDK::MarshallingCode code,
						  VDK::MsgWireFormat *pMsgPacket,
						  VDK::DeviceInfoBlock *pDev,
						  unsigned int area,
						  VDK::Ticks timeout)
{
	// get the hardwired processor ID from the SYSTAT register (range 1-6)
    const unsigned int procid   = (*pSYSTAT >> 8) & 0x7;
	const unsigned int addr     = (unsigned int)pMsgPacket->payload.addr;
	const unsigned int baseAddr = (BASE | ((procid - 1) << SHIFT));
	const unsigned int endAddr  = (BASE | ( procid      << SHIFT)) - 1;
	int result = 0;
	
	switch (code)
	{
	case VDK::ALLOCATE:
	case VDK::ALLOCATE_AND_RECEIVE:
		// Translate cluster address to local
		if (addr >= baseAddr && addr <= endAddr) // address is in this proc's cluster space
		{
			// Map it to the local space
			pMsgPacket->payload.addr = (void *)(addr - baseAddr);
		}
		break;

	case VDK::TRANSMIT_AND_RELEASE:
		// Translate local address to cluster
		if (addr < LIMIT) // is in local address space
		{
			// Map to cluster space for this procid
			pMsgPacket->payload.addr = (void *)(addr + baseAddr);
		}

		// Send the message packet containing the remapped address
		VDK::SyncWrite(pDev->dd, (char*)pMsgPacket, sizeof (VDK::MsgWireFormat), 0);

	case VDK::RELEASE:
		// No automatic freeing of clusterbus payloads, since all we're really
		// transferring is the (possibly adjusted) address.
		break;
	}

	return result;
}

#endif

}; // namespace VDK
