/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for the OSAL initialisation and start APIs
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

/* Required for the definition of NULL */
#include "stddef.h"

/* Contains auto-generated build number macro, generated from the OSAL VDK 
 * makefile */
#include "adi_osal_vdk_version.h"

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Startup")
#pragma file_attr("Startup")


/*****************************************************************************
adi_osal_Init

Description
    The adi_osal_Init function initialises the OSAL and any internal 
    structures required for OSAL operation.  This function does not initialise 
    the underlying OS, nor does it create anything required from the OS (this 
    is done in adi_osal_OSStart, as described in Section 4.2.3).

Conditions
    The initialisation function for the OSAL assumes that the OSAL takes 
    ownership of all the interrupt vectors.  Therefore it will reset all 
    interrupt vectors for IVG 2, and 5-15 such that all these vectors will use 
    the UnregisteredInterrupt ISR as a method of trapping interrupts that 
    occur without a registered handler (calls to adi_osal_ISRInstall can be 
    used to override the default handlers - see Section 4.2.43).  This default 
    ISR is simply an ISR that can be used to cause an emulator exception to 
    allow debugging (if an emulator is active). It is expected that, 
    subsequently, a service such as an interrupt manager then uses the 
    adi_osal_ISRInstall interface to take over and manage the interrupt 
    vectors.
    The structure pointed to by pConfig must be pre-initialised to contain the 
    required configuration values for the application.  If it is desired that 
    default behaviour be used for all the configuration inputs, then simply 
    set this pointer to NULL, and the defaults specified in Section 4.5.1.4 
    will be used. 
    This function does not initialise the system timer to be used for OS timer 
    ticks.  It is expected that the desired timer is initialised independently 
    prior to calling adi_osal_Init or adi_osal_OSStart (see Section 3.1.4).

Arguments    
    pConfig        [In]    A pointer to configuration parameters required for 
                           initialisation of the OSAL.  If all the defaults (
                           as specified in Section 4.5.1.4) are required for 
                           all parameters, set this pointer to NULL.

Return Values
    ADI_OSAL_SUCCESS         
        The OSAL was initialised successfully.  
    ADI_OSAL_FAILED         
        The OSAL failed to initialise properly.
    ADI_OSAL_BAD_TIMER_PERIOD    
        The value specified in the nSysTimerPerioduSec field of the 
        configuration structure is invalid.  This typically occurs if the 
        value is specified as zero, but the underlying OS does not supply 
        timing information.  In this case, supply a valid value for the 
        period.
    ADI_OSAL_BAD_PRIO_INHERIT    
        The priority inheritance operation specified is not supported by the 
        underlying OS. 
    ADI_OSAL_BAD_HEAP    
        The value specified in the pHeap field of the configuration structure 
        is invalid.  This occurs if the address of the specified heap is not 
        aligned to a 4-byte boundary
    ADI_OSAL_BAD_SLOT_NUM    
        The value specified in the nNumTLSSlots field of the configuration 
        structure exceeds the maximum number of supported slots (32 slots 
        maximum).
    ADI_OSAL_MEM_ALLOC_FAILED    
        There is insufficient memory in the heap to allocate storage required 
        by the OSAL.
    ADI_OSAL_OS_ERROR    
        The version of the underlying OS is incompatible or some OS 
        initialization function returned an error.
******************************************************************************/
ADI_OSAL_STATUS 
adi_osal_Init(const ADI_OSAL_CONFIG *pConfig)
{

    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
    /* If the pConfig is NULL then the user does not want to change the default
     * VDK configuration */
    if(NULL != pConfig)
    {
        if(pConfig->eEnablePrioInherit == ADI_OSAL_PRIO_INHERIT_ENABLED)
        {
            /* The user has specified a preference for the priority
             * inheritance configuration.  VDK 5.0 has no support
             * for priority inheritance, so this call fails. */
            result = ADI_OSAL_BAD_PRIO_INHERIT;
        }
        else if(pConfig->nNumTLSSlots > 8u)
        {
            /* VDK only supports 8 TLS slots */
            result = ADI_OSAL_BAD_SLOT_NUM;
        }
        else if(pConfig->pHeap != NULL)
        {
            /* The VDK Kernel Tab dictates which heap allocation should come 
             * from. This field should be NULL for VDK. */
            result = ADI_OSAL_BAD_HEAP;
        }
        else
        {
            /* Configure the timer if required */
            if(pConfig->nSysTimerPeriodInUsec != 0u)
            {
                /* Convert from microseconds to milliseconds */
                VDK_SetTickPeriod(((float)pConfig->nSysTimerPeriodInUsec)/1000.0);
            }
        }
    }
    
    return result;
}


/******************************************************************************
adi_osal_OSStart

Description
The adi_osal_OSStart function is used to:
    "    initialise the underlying OS
    "    allocate operating system resources required by the OSAL.
    "    create the start thread (using the attributes indicated by 
         pStartThread)
    "    invoke the startup callback (as specified by pStartCB) to perform 
         initialization tasks that are dependent on the operating system
    "    start the scheduler.  

Conditions
    It is expected that the OSAL initializes and starts the OS.  If the user 
    makes direct calls to the OS and uses the OSAL, then the user MUST NOT 
    independently initialise or start the OS.
    This function does not return if the underlying OS was started successfully
    . Upon failure the scheduler is not started.
    For the no-OS variant, an attempt to call this function will result in a 
    linker error - this is intentional since system design must be explicitly 
    targeted at operation without an OS, therefore usage of this call 
    indicates possible usage of no-OS in a system intended for operation with 
    an Operating System.

Arguments
pOSInitData        [In]    
    Specifies information that is supplied to the underlying operating system's 
    initialisation function.  This information is OS-specific and its format and
    contents are operating system dependent.  If no information is required by 
    the operating system, this value can be set to NULL.
pStartCB        [In]    
    Specifies Startup Callback information.  Set this to NULL if no callback is 
    required.  See Section 4.3.2.2 for further information on the contents of 
    this structure.
pStartThread        [In]    
    Specifies the attributes of the initial thread to be created and run by the 
    scheduler when it is started.  Refer to Section 4.3.2.3 for details of the 
    contents of this structure and its initialisation. 

Return Values
ADI_OSAL_FAILED        
    The OS could not be initialised or started.
ADI_OSAL_MEM_ALLOC_FAILED        
    There is no available memory to allocate the start thread.
ADI_OSAL_CALLBACK_FAILED        
    The callback function specified by pStartCB indicated that a failure 
    occurred.
ADI_OSAL_BAD_PRIORITY     [D]    
    The specified start thread priority is invalid for the underlying OS.
ADI_OSAL_BAD_THREAD_FUNC    [D]    
    Indicates the start thread entry point is invalid
ADI_OSAL_BAD_STACK_ADDR    [D]    
    Stack memory address for the start thread is invalid (must be word-aligned).
ADI_OSAL_BAD_STACK_SIZE    [D]    
    Invalid stack size for the start thread specified (some Operating Systems 
    may have minimum stack requirements)
ADI_OSAL_BAD_THREAD_NAME    [D]    
    The length of the thread name exceeds ADI_OSAL_MAX_THREAD_NAME as defined 
    in Section 4.5.1.5.
******************************************************************************/

ADI_OSAL_STATUS 
adi_osal_OSStart(const void *pOSInitData,
                 const ADI_OSAL_CALLBACK_ATTR     *pStartCB,
                 const ADI_OSAL_THREAD_ATTR    *pStartThread)
{
     ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
     ADI_OSAL_THREAD_HANDLE startThread;
     
     /* Initialise and start VDK.  
      * This API should be called within main(), and should not be called in
      * conjunction with VDK_Init() and VDK_Run() as that's what we do here. */
     
     VDK_Initialize();
      
     if(NULL != pStartCB)
     {
         /* Call the user-supplied callback. */
         if(ADI_OSAL_SUCCESS != (*pStartCB->pCallbackFunc)(pStartCB->pParam))
         {
            result = ADI_OSAL_CALLBACK_FAILED;
         }
     }
     
    /* VDK does not need any initialisation data, and there is no relevant 
     * error for passing data that is not required, so we do not check
     * the pOSInitData param */
     
    /* Create the supplied user thread if the pointer is not NULL (fix for
     * TAR-44377) Note that this call only adds the thread to the ready queue
     * and does not guarantee that it will be the first thread to run.  If 
     * this thread is the highest priority thread in the ready queue then 
     * it will be the first to run. */

    if( (NULL != pStartThread) && (ADI_OSAL_SUCCESS == result) )
    {
        result = adi_osal_ThreadCreate(&startThread, pStartThread);
    }
    
    /* We only start the scheduler if the thread created correctly and
     * any callbacks returned correctly */
    if(ADI_OSAL_SUCCESS == result)
    {
        /* Start VDK */
        VDK_Run();        
        
        /* We should never get here - it is a failure if we do */
        result = ADI_OSAL_CALLBACK_FAILED;
    }
    
    return result;
}



/******************************************************************************
adi_osal_GetVersion

Description
    The adi_osal_GetVersion function is used to access the version of the OSAL 
    code for the variant in use.  

Conditions
    The code version can vary from one OSAL variant to another, although in all 
    cases, the major and minor version fields are always the same as the API 
    version (please refer to Section 4.3.2.4).
    ISR SAFE

Arguments
pVersion        [Out]    
    A pointer to the location to write the returned version information.  
    Please refer to Section 4.3.2.4 for details on the contents of the 
    ADI_OSAL_VERSION structure.

Return Values
ADI_OSAL_SUCCESS     
    The version information was returned successfully.  
******************************************************************************/
ADI_OSAL_STATUS 
adi_osal_GetVersion(ADI_OSAL_VERSION *pVersion)
{

    pVersion->nMajor = ADI_OSAL_MAJOR_VER;
    pVersion->nMinor = ADI_OSAL_MINOR_VER;
    pVersion->nPatch = ADI_OSAL_PATCH_VER;
    pVersion->nBuild = ADI_OSAL_BUILD_VER;

    return(ADI_OSAL_SUCCESS);
}

