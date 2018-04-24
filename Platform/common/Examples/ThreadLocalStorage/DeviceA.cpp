/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for DeviceA
 *
 * -----------------------------------------------------------------------------
 *  Comments: This is a trivial implementation of a VDK Device Driver.
 *
 * ===========================================================================*/

#include "DeviceA.h"
#include <stdio.h>

#ifdef VDK_INCLUDE_IO_

/* globals */

/* statics */

/* externs */

/* preprocessor macros */



/******************************************************************************
 * Dispatch Function for the DeviceA Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
DeviceA::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
                /* Put Device's Initialization code HERE */
            break;
        case VDK::kIO_Activate:  
                /* Put Device's Activate code HERE */
            break;
        case VDK::kIO_Open:      
                /* Put Device's Open code HERE */
            break;
        case VDK::kIO_Close:     
                /* Put Device's Close code HERE */
            break;
        case VDK::kIO_SyncRead:      
				printf("ThreadID %d: Reading from Device A\n", VDK::GetThreadID());
            break;
        case VDK::kIO_SyncWrite:     
				printf("ThreadID %d: Writing to Device A\n", VDK::GetThreadID());
            break;
        case VDK::kIO_IOCtl:
                /* Put Device's IOCtl code HERE */
            break;
        default:
                /* Put invalid DeviceDispatchID code HERE */
            break;
    }
    return ret_val;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
