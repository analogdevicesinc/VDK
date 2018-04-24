/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <Heap.h> // for heap_ functions
#include "Heap.h" // VDK Header
#include <stdlib.h>
#include "VDK_API.h"
#include "VDK_Abs_Device.h"
#include "VDK_DCB.h"
#include "IODeviceTable.h"
#pragma file_attr("OS_Component=MP_Messages")
#pragma file_attr("MP_Messages")
#pragma file_attr("HeapMarshaller")

namespace VDK {

int HeapMarshaller (VDK::MarshallingCode code,
					VDK::MsgWireFormat *pMsgPacket,
					VDK::DeviceInfoBlock *pDev,
					unsigned int heapID,
					VDK::Ticks timeout)
{
	int result = 0;
	unsigned int heapIndex = g_Heaps[heapID].m_Index;

	switch (code)
	{
	case VDK::ALLOCATE:
	case VDK::ALLOCATE_AND_RECEIVE:
		pMsgPacket->payload.addr = heap_malloc(heapIndex, pMsgPacket->payload.size);
		
		if (NULL == pMsgPacket->payload.addr) return false;
		
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
		heap_free(heapIndex, pMsgPacket->payload.addr);
		break;
	}
	
	return result;
}


int EmptyHeapMarshaller (VDK::MarshallingCode code,
					VDK::MsgWireFormat *pMsgPacket,
					VDK::DeviceInfoBlock *pDev,
					unsigned int heapID,
					VDK::Ticks timeout)
{
	int result = 0;
	unsigned int heapIndex = g_Heaps[heapID].m_Index;

	switch (code)
	{
	case VDK::ALLOCATE:
	case VDK::ALLOCATE_AND_RECEIVE:
		pMsgPacket->payload.addr = heap_malloc(heapIndex, pMsgPacket->payload.size);
		
		if (NULL == pMsgPacket->payload.addr) return false;
		break;
	case VDK::TRANSMIT_AND_RELEASE:
		result = VDK::SyncWrite(pDev->dd, (char*)pMsgPacket->payload.addr, pMsgPacket->payload.size, timeout);
	case VDK::RELEASE:
		heap_free(heapIndex, pMsgPacket->payload.addr);
		break;
	}
	
	return result;
}

}; // namespace VDK
