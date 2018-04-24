/* =============================================================================
 *
 *  Description: This is a C implementation of a VDK Device Driver for %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "%HeaderFile%"

#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

#ifdef VDK_INCLUDE_IO_

/* globals */

/* statics */

/* externs */

/* preprocessor macros */


/******************************************************************************
 *
 * Dispatch Function for the %Name% Device Driver
 *
 */
 
void*
%Name%_DispatchFunction(VDK_DispatchID inCode, VDK_DispatchUnion inUnion)
{
    void    *ret_val = 0;
    switch(inCode)
    {
        case VDK_kIO_Init:      
                /* Put %Name%'s Initialization code HERE */
            break;
        case VDK_kIO_Activate:  
                /* Put %Name%'s Activate code HERE */
            break;
        case VDK_kIO_Open:      
                /* Put %Name%'s Open code HERE */
            break;
        case VDK_kIO_Close:     
                /* Put %Name%'s Close code HERE */
            break;
        case VDK_kIO_SyncRead:      
                /* Put %Name%'s Read code HERE */
            break;
        case VDK_kIO_SyncWrite:     
                /* Put %Name%'s Write code HERE */
            break;
        case VDK_kIO_IOCtl:
                /* Put %Name%'s IOCtl code HERE */
            break;
        default:
                /* Put invalid DeviceDispatchID code HERE */
            break;
    }
    return ret_val;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
