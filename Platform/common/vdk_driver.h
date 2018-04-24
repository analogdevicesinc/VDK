/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _VDK_DRIVERS_
#define _VDK_DRIVERS_

#include "TMK.h"

#include "VDK_ABS_DEVICE.h"
#include <new>

namespace VDK
{
	// Device driver class
	// 
class DeviceDriver : public IOAbstractBase, public TMK_DpcElement
{
public:
        DeviceDriver();

	void * operator new(size_t allocate_size,  HeapID heap);
	void * operator new(size_t allocate_size);
	void * operator new(size_t allocate_size, const _STD nothrow_t) {return ::new char[allocate_size];}
	inline void operator delete(void * ptr) { ::delete ptr;}
	inline void operator delete(void * ptr,HeapID heap) { ::delete ptr;}
	inline void operator delete(void * ptr,const _STD nothrow_t) { ::delete ptr;}

 protected:
        /**
         * Plugs into the pfInvoke_m ptr in DpcQueueElement
         */
        static void Invoke(TMK_DpcElement *pSelf);

	// User-derived class implements the dispatcher function
}; 
}  	   /* namespace VDK */
#endif /* _VDK_DRIVERS_ */
