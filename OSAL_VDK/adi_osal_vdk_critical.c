/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for the OSAL region APIs, specifically for scheduler
 *                and critical regions. 
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

/* Used for the sti/cli builtins */
#include <ccblkfn.h>

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Regions")
#pragma file_attr("Regions")


/* Module does not need to disable any MISRA warnings */

static struct 
{
    uint32_t mask;  /* Used to store the interrupt mask for critical regions*/ 
    uint32_t count; /* Used to keep track of the nested critical regions */
}  s_State;


/*****************************************************************************
adi_osal_IsSchedulerActive

Description
    The adi_osal_IsSchedulerActive function is used to determine if the 
    scheduler has been started and is actively scheduling tasks. This is 
    useful to if tasks need to be performed differently during startup and 
    normal operation. 

Conditions
    This function does not indicate if the scheduler has been temporarily 
    halted due to a scheduler lock - it only indicates started or 
    not.
    For the No-OS variant, this will always return false (i.e. no scheduler 
    active).

Arguments
    None

Return Values
    TRUE if the scheduler is running, otherwise FALSE.         
*****************************************************************************/
bool 
adi_osal_IsSchedulerActive()
{
    /* Return whether or not the scheduler has started */
    /* ISR safe VDK call */
    return VDK_IsRunning();
}

/*****************************************************************************
adi_osal_SchedulerLock

Description
    The adi_osal_SchedulerLock function prevents thread scheduling until its 
    counterpart, adi_osal_SchedulerUnlock, is called. This is used to prevent 
    pre-emption of a critical thread section by another 
    thread.
    The thread that calls this service keeps control of the CPU even though 
    other higher priority threads are ready to run. However, interrupts are 
    still recognized and serviced (assuming interrupts are enabled). 

Conditions
    The functions adi_osal_SchedulerLock and adi_osal_SchedulerUnlock must be 
    used in pairs. Scheduling is enabled when an equal number of 
    adi_osal_SchedulerUnlock calls have been made.
    Be aware that in the unlikely event that the counter used to track nesting 
    overflows, there is no error checking, and the potential for erroneous 
    behaviour exists.

    NOTE: After calling adi_osal_SchedulerLock, the application must not make 
    calls to the functions that suspend execution of the current thread (such 
    as any of the Pend functions, and adi_osal_ThreadSleep()). This is because 
    the scheduler is locked out, so no other thread is allowed to run, and the 
    system will lock up.
    Scheduler locking cannot be performed within an ISR. 

Arguments
    None
Return Values
    None    this function always succeeds.


*****************************************************************************/

void 
adi_osal_SchedulerLock(void)
{
    /* We do not do anything if we are in an ISR or at startup */
    if((VDK_IsRunning()) && (!InISR()))
    {
        VDK_PushUnscheduledRegion();
    }
}


/*****************************************************************************
adi_osal_SchedulerUnlock

Description
    The adi_osal_SchedulerUnlock function re-enables scheduling after a 
    corresponding call to adi_osal_SchedulerLock has been 
    issued.

Conditions
    The functions adi_osal_SchedulerLock and adi_osal_SchedulerUnlock must be 
    used in pairs. Scheduling is enabled when an equal number of 
    adi_osal_SchedulerUnlock calls have been 
    made.

Arguments
   None

Return Values
ADI_OSAL_SUCCESS            
    The scheduler was successfully unlocked
ADI_OSAL_FAILED   [D]     
    Scheduler unlock was performed before performing a corresponding lock.  In 
    non-debug mode, the function always succeeds.

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_SchedulerUnlock(void)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
    /* We only touch the scheduler if we are at thread level */
    if((VDK_IsRunning()) && (!InISR()))
    {
#if defined(OSAL_DEBUG)
        /* In the OSAL DEBUG case we attempt to handle the 
         * ThreadError if it returns */ 
        VDK_ClearThreadError();
#endif

        /* Enable scheduling - This may not return if there is an
         * underflow. */
        VDK_PopUnscheduledRegion();
        
#if defined(OSAL_DEBUG)
        /* For Non Error Checking builds of VDK this check will not
         * detect if the error condition occurred */
        if(VDK_GetLastThreadError() != VDK_kNoError)
        {
            result = ADI_OSAL_FAILED;
        }
#endif
    }
    
    return result;
}


/*****************************************************************************
adi_osal_EnterCriticalRegion

Description
    The function adi_osal_EnterCriticalRegion disables interrupts to allow the 
    execution of critical section code.

Conditions
    Note that critical sections can be nested, and that there must be an equal 
    number of adi_osal_ExitCriticalRegion calls before interrupt functionality 
    will be successfully restored; interrupts will remain disabled until the 
    balance is restored.
    NOTE: Be very careful about use of these critical regions around code that 
    may use an OS function that relies on a timeout (suspends a thread or 
    performs a thread sleep), since disabling interrupts will disable the timer
    time-tick and any suspended threads will therefore never complete.  
    Similarly, it is not wise to call any OS function inside a critical region,
    care should be exercised if this is done.

Arguments
    None    
Return Values
    None    this function always succeeds.


*****************************************************************************/

void adi_osal_EnterCriticalRegion(void)
{
    uint32_t currMask;
    
    /* We only use the VDK Push/Pop critical region from thread level */
    if((!VDK_IsRunning()) || (InISR()))
    {
        /* ISR or Startup - use cli()/sti() directly. */
        
        /* Disable interrupts before anything else */
        currMask = cli();
        
        /* Increment the critical region count */
        s_State.count++;
        
        /* Now check the count, if it is now 1 (was 0) then we take this
         * as the new mask */
        if(1u == s_State.count)
        {
            s_State.mask = currMask;
        }
    }
    else
    {
        /* We're at thread level, so we can just call PushCriticalRegion() */
        VDK_PushCriticalRegion();
    }
    
    /* This API returns no errors and assumes success */
}



/*****************************************************************************
adi_osal_ExitCriticalRegion


Description
    The function adi_osal_ExitCriticalRegion enables interrupts that were 
    disabled using the corresponding adi_osal_EnterCriticalRegion function 
    call.

Conditions
    The calls to Enter/ExitCriticalRegion must be matched in pairs if the 
    calls are nested.

Arguments
    None        
Return Values
    None    this function always succeeds. 

*****************************************************************************/

void adi_osal_ExitCriticalRegion(void)
{
    if((!VDK_IsRunning()) || (InISR()))
    {
        /* ISR or Startup - use cli()/sti() directly.*/
        
        /* Decrement the critical region count */
        s_State.count--;
        
        /* Check the count, if it is now 0 then
         * we need to restore the interrupt state */
        if(0u == s_State.count)
        {
            
            /*  Only enable interrupts after we have accessed the the
             *     current state count */
            sti(s_State.mask);
        }
    }
    else
    {
        /* We're at thread level, so we can just call PushCriticalRegion() */
        VDK_PopCriticalRegion();
    }
    
    /* This API returns no errors, and assumes that interrupts are now 
     * enabled */
}




