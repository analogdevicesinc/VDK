/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL timing APIs.
 *
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 *  Copyright (c) 2009 Analog Devices, Inc.,  All rights reserved
 *
 *  This software is the property of Analog Devices, Inc.
 *
 *  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
 *  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
 *  PERFORMANCE, OR USE OF THIS SOFTWARE.
 * ===========================================================================*/

#include "osal_vdk.h"

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("System")



/*****************************************************************************
adi_osal_TickPeriodInMicroSec

Description
    The adi_osal_TickPeriodInMicroSec function is used to indicate the current 
    duration of a system tick in microseconds.  This is useful for conversion 
    of time units into the system tick counts needed by timing APIs (such as 
    the pend APIs of message queues, semaphores, mutexes, events).

Conditions
    Depending on system clock frequency and rounding issues, this value will 
    be the tick period rounded to the nearest microsecond.

Arguments
    pnTickPeriod        [Out]   A pointer to the location to write the 
    returned tick period (a value in microseconds).

Return Values
    ADI_OSAL_SUCCESS    The tick period was returned successfully.  
    ADI_OSAL_FAILED Failed to determine the tick period (should not happen).

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_TickPeriodInMicroSec(uint32_t *pnTickPeriod)
{

    /* GetTickPeriod() returns the period in milliseconds
     */ 
    *pnTickPeriod = (unsigned int)VDK_GetTickPeriod()*1000u;

    return ADI_OSAL_SUCCESS;
}


/*****************************************************************************
adi_osal_TimeTick

Description
    The adi_osal_TimeTick function is used to indicate to the Operating 
    System that a system clock tick period has completed.  This is used by 
    the OS to process timing events, such as timeouts, sleeps, delays, etc.

Conditions 
    This function should be called from a timer ISR to perform the system 
    clock functionality.
    For some Operating Systems, this function does nothing.

Arguments
    None

Return Values
    None

*****************************************************************************/

void adi_osal_TimeTick(void)
{
    /* in VDK this does nothing */
}

/*****************************************************************************
adi_osal_GetCurrentTick

Description
    The adi_osal_GetCurrentTick function returns the current value of the 
    continuously incrementing system timer tick counter.  This counter is a 
    32-bit value that increments once for every system timer interrupt, 
    continuously wrapping.  It is the responsibility of the user of the tick 
    value to take into account any wrap that may have occurred.

Arguments
    pnTicks     [Out]       A pointer to the location to write the returned 
                            tick count.
Return Values
ADI_OSAL_SUCCESS    The current tick count was returned successfully.  
ADI_OSAL_FAILED     Failed to retrieve the current tick count (should not 
                    happen).
*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_GetCurrentTick(uint32_t    *pnTicks)
{
    *pnTicks = VDK_GetUptime();

    return ADI_OSAL_SUCCESS;
}


