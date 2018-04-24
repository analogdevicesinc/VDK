/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for DeviceB
 *
 * -----------------------------------------------------------------------------
 *  Comments: This is a trivial implementation of a VDK Device Driver.
 *
 * ===========================================================================*/

#include "DeviceB.h"
#include <stdio.h>

#ifdef VDK_INCLUDE_IO_

/* globals */

/* statics */

/* externs */

/* preprocessor macros */



/******************************************************************************
 * Dispatch Function for the DeviceB Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
DeviceB::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
                /* Put Device2's Initialization code HERE */
            break;
        case VDK::kIO_Activate:  
                /* Put Device2's Activate code HERE */
            break;
        case VDK::kIO_Open:      
                /* Put Device2's Open code HERE */
            break;
        case VDK::kIO_Close:     
                /* Put Device2's Close code HERE */
            break;
        case VDK::kIO_SyncRead:      
				printf("ThreadID %d: Reading from Device B\n", VDK::GetThreadID());
            break;
        case VDK::kIO_SyncWrite:     
				printf("ThreadID %d: Writing to Device B\n", VDK::GetThreadID());
            break;
        case VDK::kIO_IOCtl:
                /* Put Device2's IOCtl code HERE */
            break;
        default:
                /* Put invalid DeviceDispatchID code HERE */
            break;
    }
    return ret_val;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
