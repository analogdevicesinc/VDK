/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is a C++ VDK Device Driver for TheDeviceDriver
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
 *
 * ===========================================================================*/


#include "VDK.h"
#include "TheDeviceDriver.h"


/* globals */

/* statics */

/* externs */

/* preprocessor macros */



/******************************************************************************
 * Dispatch Function for the TheDeviceDriver Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
TheDeviceDriver::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
                /* Put TheDeviceDriver's Initialization code HERE */
            break;
        case VDK::kIO_Activate:  
                /* Put TheDeviceDriver's Activate code HERE */
            break;
        case VDK::kIO_Open:      
                /* Put TheDeviceDriver's Activate code HERE */
            break;
        case VDK::kIO_Close:     
                /* Put TheDeviceDriver's Close code HERE */
            break;
        case VDK::kIO_SyncRead:      
                /* Put TheDeviceDriver's Read code HERE */
            //break;
        case VDK::kIO_SyncWrite:     
                /* Put TheDeviceDriver's Write code HERE */
            //break;
        case VDK::kIO_IOCtl:
                /* Put TheDeviceDriver's IOCtl code HERE */
            break;
        default:
                /* Put invalid DeviceDispatchID code HERE */
            break;
    }
    return ret_val;
}

