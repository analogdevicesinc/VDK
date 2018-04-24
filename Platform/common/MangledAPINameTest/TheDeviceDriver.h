/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is a VDK Device Driver Header for TheDeviceDriver
 *
 *   Created on:  Monday, May 14, 2001 - 02:54:43 PM
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef _TheDeviceDriver_H_
  #define _TheDeviceDriver_H_

  /* Include the C runtime API */
  #include "VDK.h"

#if defined(__cplusplus)
/* C++ declaration */

class TheDeviceDriver : public VDK::DeviceDriver
{
public:
	void*
	DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion);
	static IOAbstractBase* Create()
	{
		return(new(TheDeviceDriver));
	}
};

#endif /* ifdef __ECC__ */
#endif /* _TheDeviceDriver_H_ */



