/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <stdlib.h>  // for heap_ functions
#include "VDK_API.h"
#include "VDK_Abs_Device.h"
#include "VDK_DCB.h"
#include "IODeviceTable.h"
#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")
#pragma file_attr("PoolMarshaller")

namespace VDK {

int PoolMarshaller (VDK::MarshallingCode code,
					VDK::MsgWireFormat *pMsgPacket,
					VDK::DeviceInfoBlock *pDev,
					unsigned int area,
					VDK::Ticks timeout)
{
	VDK::PoolID pool = (VDK::PoolID)area;
	int result = 0;
	
	switch (code)
	{
	case VDK::ALLOCATE:
	case VDK::ALLOCATE_AND_RECEIVE:
		pMsgPacket->payload.addr = VDK::MallocBlock(pool);
		
		if (VDK::ALLOCATE == code) break;
		
#if 1
		result = VDK::SyncRead(pDev->dd, (char*)pMsgPacket->payload.addr, pMsgPacket->payload.size, timeout);
#else
		DispatchUnion t;
		t.ReadWrite_t.dataH = pDev->pDcb->DD_DataPointer();
		t.ReadWrite_t.timeout = 0;
		t.ReadWrite_t.dataSize = pMsgPacket->payload.size;
		t.ReadWrite_t.data = (char*)pMsgPacket->payload.size;
		result = (int)(pDev->pfDispatchFunction)(pDev->pDevObj, kIO_SyncRead, &t);
#endif
		break;
	case VDK::TRANSMIT_AND_RELEASE:
#if 1		
		VDK::SyncWrite(pDev->dd, (char*)pMsgPacket, sizeof (VDK::MsgWireFormat), 0);
		result = VDK::SyncWrite(pDev->dd, (char*)pMsgPacket->payload.addr, pMsgPacket->payload.size, timeout);
#else
		t.ReadWrite_t.dataH = pDev->pDcb->DD_DataPointer();
		t.ReadWrite_t.timeout = 0;
		t.ReadWrite_t.dataSize = sizeof (VDK::MsgWireFormat);
		t.ReadWrite_t.data = (char*)pMsgPacket;
		(pDev->pfDispatchFunction)(pDev->pDevObj, kIO_SyncWrite, &t);
		t.ReadWrite_t.dataSize = pMsgPacket->payload.size;
		t.ReadWrite_t.data = (char*)pMsgPacket->payload.size;
		result = (int)(pDev->pfDispatchFunction)(pDev->pDevObj, kIO_SyncWrite, &t);
#endif
	case VDK::RELEASE:
		VDK::FreeBlock(pool, pMsgPacket->payload.addr);
		break;
	}

	return result;
}

int EmptyPoolMarshaller (VDK::MarshallingCode code,
					VDK::MsgWireFormat *pMsgPacket,
					VDK::DeviceInfoBlock *pDev,
					unsigned int area,
					VDK::Ticks timeout)
{
	VDK::PoolID pool = (VDK::PoolID)area;
	int result = 0;
	
	switch (code)
	{
	case VDK::ALLOCATE:
	case VDK::ALLOCATE_AND_RECEIVE:
		pMsgPacket->payload.addr = VDK::MallocBlock(pool);
		break;
	case VDK::TRANSMIT_AND_RELEASE:
		result = VDK::SyncWrite(pDev->dd, (char*)pMsgPacket->payload.addr, pMsgPacket->payload.size, timeout);
	case VDK::RELEASE:
		VDK::FreeBlock(pool, pMsgPacket->payload.addr);
		break;
	}

	return result;
}

}; // namespace VDK
