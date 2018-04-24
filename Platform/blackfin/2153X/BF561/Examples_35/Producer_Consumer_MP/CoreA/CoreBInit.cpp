/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for CoreBInit
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "CoreBInit.h"
#include <ccblkfn.h>

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* globals */

/* statics */

/* externs */

/* preprocessor macros */



/******************************************************************************
 * Dispatch Function for the CoreBInit Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
CoreBInit::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK::kIO_Init:      
            adi_core_b_enable();
            break;
    }
    return ret_val;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
