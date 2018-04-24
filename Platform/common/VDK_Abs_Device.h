/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _VDK_ABS_DEVICE_
#define _VDK_ABS_DEVICE_

#include "VDK_API.h"
#include "IDTable.h"

namespace VDK
{
	//
	// Abstract I/O device class, any future devices or pseudo devices have to derive from
	// this class.
	//
	class IOAbstractBase  : public IDElementTemplate<IOID>
	{
		public:
		//
		// Pure virtual function implemented in the derived classes.
		// inDispatchID could be one of the following
		//
		virtual void* DispatchFunction(DispatchID inDispatchID,DispatchUnion &inDispatchUnion) = 0;
	};

	// A template that is filled with the Create function call 
	// which is a static function in the user derived class.
	// TODO: Move this struct to vdk_api.h.
	//
	typedef struct IODeviceTemplate		
	{
		IOAbstractBase* (*CreateFuncP)(void);
	} IODeviceTemplate;
}/* namespace VDK */

#endif /* _VDK_ABS_DEVICE_ */
