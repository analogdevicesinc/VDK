/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL thread APIs.
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
#include <stddef.h>

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


/* File misra suppressions */
#pragma diag(push) 
#pragma diag(suppress:misra_rule_11_3 : "Allow integer to pointer casting to convert VDK ThreadIDs to OSAL handles")
#pragma diag(suppress:misra_rule_18_1 : "Allow the use of an undefined struct ( as it is not fully defined in the VDK_Public.h header.")

/**
 * The OSAL Thread Template is defined in adi_osal_vdk_threadtemplate.cpp
 */
extern struct VDK_ThreadTemplate g_OSALThreadTemplate;

/**
 * Internal priority conversion functions.
 *
 * Converts between priority levels, but does no checks of validity.
 * Definitions are below, the prototypes are required to avoid misra warnings.
 **/
#pragma const
#pragma inline
#pragma always_inline
ADI_OSAL_PRIORITY PriorityVdkToOsal(VDK_Priority inPrio);

#pragma const
#pragma inline
#pragma always_inline
VDK_Priority PriorityOsalToVdk(ADI_OSAL_PRIORITY inPrio);

VDK_Priority
PriorityOsalToVdk(ADI_OSAL_PRIORITY inPrio)
{
    /* VDK Priority range goes from:
     *    0x30 for kPriority1/kMaxPriority (Highest)
     *    0x1  for kPriority30             (Lowest)
     * OSAL threads go from:
     *    0x0   (Highest)
     *    0x29  (Lowest valid value, in the VDK port of OSAL)
     */ 
    return (VDK_Priority)((int)VDK_kMaxPriority - (int)(inPrio));
}


ADI_OSAL_PRIORITY
PriorityVdkToOsal(VDK_Priority inPrio)
{
    /* VDK Priority range goes from:
     *    0x30 for kPriority1/kMaxPriority (Highest)
     *    0x1  for kPriority30             (Lowest)
     * OSAL threads go from:
     *    0x0   (Highest)
     *    0x29  (Lowest valid value, in the VDK port of OSAL)
     */ 
    return (ADI_OSAL_PRIORITY)((unsigned int)VDK_kMaxPriority - (unsigned int)(inPrio));
}


/*****************************************************************************
adi_osal_ThreadCreate

Description
    The adi_osal_ThreadCreate function is used to create a new thread to be 
    scheduled for execution. If the scheduler is running the newly created 
    thread will be queued for execution.

Conditions
    The members of the ADI_OSAL_THREAD_ATTR structure pointed to by pThreadAttr
    are assumed to have been pre-initialised by the caller, and determine the 
    characteristics of the thread being created.
    
    If creation of a thread fails, the location *phThread will contain the 
    value ADI_OSAL_INVALID_THREAD.
    
    NOTE: The first 5.0 port of OSAL on VDK differs from the specification 
    in that the stack pointer *MUST* be NULL and threads had to be created
    using the APIs that allocate stacks internally.  The nStackSize field of
    the thread attributes then becomes a request rather than a statement.  
    This was changed in 5.0 update 9 to have the same behaviour as Andromeda
    but we keep the behaviour that if NULL is passed then VDK allocates the
    space (for backwards compatibility reasons)

Arguments
    phThread        [Out]       
        A pointer to the location to write the returned handle of the newly 
        created thread.
    pThreadAttr     [In]        
        A pointer to a pre-initialised structure specifying the attributes of 
        the new thread. 

Return Values

ADI_OSAL_SUCCESS        
    The specified thread was created successfully.  
ADI_OSAL_FAILED         
    Thread creation failed
ADI_OSAL_MEM_ALLOC_FAILED      
    No memory is available for the creation of the thread.
ADI_OSAL_PRIORITY_IN_USE        
    The specified priority is already in use by a thread.  This error will 
    only be returned if the underlying OS does not support multiple threads 
    with the same priority level.
ADI_OSAL_BAD_PRIORITY   [D] 
    The specified thread priority is invalid for the underlying OS.
ADI_OSAL_BAD_THREAD_FUNC    [D] 
    Indicates thread entry point is invalid
ADI_OSAL_BAD_STACK_ADDR [D] 
    Stack memory address is invalid (must be word-aligned).
ADI_OSAL_BAD_STACK_SIZE [D]     
    Invalid stack size specified (some Operating Systems may have minimum 
    stack requirements)
ADI_OSAL_BAD_THREAD_NAME    [D] 
    The length of the thread name exceeds ADI_OSAL_MAX_THREAD_NAME.  *Note 
    that VDK does not support thread names*
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 
*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadCreate( ADI_OSAL_THREAD_HANDLE *phThread, 
                       const ADI_OSAL_THREAD_ATTR  *pThreadAttr)
{
    /* We need to create some structure to pass through to the
       VDK thread creation API. */
    VDK_ThreadCreationBlock tcb;    
    VDK_ThreadID thread;
    int32_t fieldBitsSet;
    
    /* OSALRunData is required to store the thread's Run function and the
     * parameter to be passed to the Run function. */
    OSALRunData threadData;
    
    VDK_Priority nOsalPriority = PriorityOsalToVdk(pThreadAttr->nPriority);

    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

#ifdef OSAL_DEBUG

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if(InISR())
    {
        *phThread = ADI_OSAL_INVALID_THREAD;
        return ADI_OSAL_CALLER_ERROR;
    }

    /* On blackfin the supplied stack size is added to the size of the
     * context record (the addition is made internally by VDK, and the 
     * context record size is not visible to users), so the allocated stack will
     * be bigger than that requested here.  The only check we
     * need to make is for a valid size.  We don't document a minimum stack
     * size, so we are going to follow the example of the kernel tab and 
     * reject a stack size of 0 (>1 is considered valid). 
     */
    if(0u == pThreadAttr->nStackSize)
    {
        *phThread = ADI_OSAL_INVALID_THREAD;
        return ADI_OSAL_BAD_STACK_SIZE;
    }

    if( (nOsalPriority > VDK_kMaxPriority) ||
        (nOsalPriority < VDK_kMinPriority) )
    {
        /* Priority is not in the valid VDK priority range */
        *phThread = ADI_OSAL_INVALID_THREAD;
        return ADI_OSAL_BAD_PRIORITY;
    }

    /* Checking for 16-bit alignment and NULL is the only real check we can 
     * make. */
    if( (0u != (1u & (unsigned int)pThreadAttr->pThreadFunc)) || 
        ((int)NULL == (int)pThreadAttr->pThreadFunc) )
    {
        *phThread = ADI_OSAL_INVALID_THREAD;
        return ADI_OSAL_BAD_THREAD_FUNC;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/
        
#endif /*OSAL_DEBUG*/
   
    threadData.pfRun   = pThreadAttr->pThreadFunc;
    threadData.pParam  = pThreadAttr->pParam;
   
    /* The supplied thread stack size is in bytes, but VDK takes a stack size
     * in words. As VDK will be allocating the memory we must ensure that the
     * allocated memory can hold the requested size - i.e. It must be the same
     * as that requested or bigger if the requested size does not divide 
     * exactly by sizeof word. */ 
    tcb.thread_stack_size   = (pThreadAttr->nStackSize + (sizeof(int) - 1u)) / sizeof(int);

    tcb.template_id         = kDynamicThreadType;
    tcb.thread_priority     = nOsalPriority;
    tcb.pTemplate           = &g_OSALThreadTemplate;
    tcb.stack_pointer       = (unsigned int *)pThreadAttr->pStackBase;
    tcb.user_data_ptr       = (void *)&threadData;


    /* We need to convert the fields required to an unsigned int, 
     * as the TCBBitField enum is treated as an int and will 
     * violate MISRA rule 10.3. */                          

    /* The first 5.0 release of OSAL always took 0 as a stack pointer which
     * meant that VDK would allocate the stack. We do not want to break those
     * applications so we only set the field if what we were passed is non-zero
     */

    if (NULL == pThreadAttr->pStackBase) 
    {
        fieldBitsSet = VDK_kSetThreadStackSize | 
                       VDK_kSetThreadPriority  |
                       VDK_kSetUserDataPointer | 
                       VDK_kSetThreadTemplatePointer;
    }
    else
    {
        fieldBitsSet = VDK_kSetThreadStackSize | 
                       VDK_kSetThreadPriority  |
                       VDK_kSetUserDataPointer | 
                       VDK_kSetThreadTemplatePointer |
                       VDK_kSetThreadStackPointer;
    }

    thread = VDK_CreateThreadEx2(&tcb, fieldBitsSet); 
                                                      
    /* Check for thread creation success. */
    if((int)UINT_MAX != thread)
    {
       *phThread = (ADI_OSAL_THREAD_HANDLE)thread;
    }   
    else
    {
        /* This failure can be for many reasons, and may include those that 
         * have their own failure code.  VDK behaves differently depending on
         * configuration so we can't consistently check everything.  
         * Returning fail. */
        *phThread = ADI_OSAL_INVALID_THREAD;
        result = ADI_OSAL_FAILED;
    } 

    return result;
}


/*****************************************************************************
adi_osal_TaskDestroy()

Description:
To delete the task (hence can no longer be scheduled)
Notes: * Depending on OS, This function may not be returned when you try to "destroy" a task by itself. 


Arguments:
    hTaskID     
        Handle of the task to be deleted

Return Values:
ADI_OSAL_SUCCESS         
    If successfully removed the task from the system.
ADI_OSAL_FAILED         
    If failed to delete the task
ADI_OSAL_INVALID_ID     
    If the specified task ID is invalid
ADI_OSAL_CALLER_ERROR     
    If tried to delete the task from ISR

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadDestroy(ADI_OSAL_THREAD_HANDLE const hThread)
{
    VDK_ThreadID thread = (VDK_ThreadID)hThread;
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
#ifdef OSAL_DEBUG

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if( (!VDK_IsRunning()) || (InISR()) )
    {
        return ADI_OSAL_CALLER_ERROR;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/


    /* OSAL Threads cannot destroy themselves */
    if(VDK_GetThreadID() != thread)
    { 
        VDK_ClearThreadError();

        VDK_DestroyThread(thread, true);

        if(VDK_kNoError != VDK_GetLastThreadError())
        {
            /* The only error can be if the thread is unknown.  OSAL does 
             * not have any knowledge of the idle thread, so it's simply 
             * passing an unknown token */
#ifdef OSAL_DEBUG
            result = ADI_OSAL_BAD_HANDLE;
#else
            result = ADI_OSAL_FAILED;
#endif /* OSAL_DEBUG */
        }
    }
    else
    {   
        /* Trying to destroy itself */
        result = ADI_OSAL_DESTROY_SELF; 
    }

    return result;       
}

/*****************************************************************************
adi_osal_ThreadGetName 

Description
    The adi_osal_ThreadGetName function is used to retrieve the name of the 
    currently executing thread.  The thread name, up to a maximum of the value 
    specified by nNumBytesToCopy, will be copied to the location indicated by 
    pThreadName.  

Conditions
    The number of bytes indicated by nNumBytesToCopy will be copied to the 
    specified location.  If the thread name is shorter than this value, then 
    the whole name will be copied.  If it is desired to ensure that the whole 
    name is retrieved, then nNumBytesToCopy should be set to 
    ADI_OSAL_MAX_THREAD_NAME.
    The contents of the location indicated by pThreadName will not be altered 
    if the function fails to retrieve the thread name.

Arguments
    hThread     [In]    
        The handle of the thread of which to retrieve the name.
    pThreadName     [In]        
        A pointer to the location to write the thread name.
    nNumBytesToCopy     [In]    
        Specifies the maximum number of bytes of the name to copy to the 
        destination (including the terminating NULL).

Return Values
ADI_OSAL_SUCCESS            
    The thread name was returned successfully.  
ADI_OSAL_FAILED     
    The thread name could not be determined.
ADI_OSAL_BAD_HANDLE    [D]  
    The specified thread handle is invalid.

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ThreadGetName( ADI_OSAL_THREAD_HANDLE const hThread, 
                        char_t  *pTaskName,
                        uint32_t    nNumBytesToCopy)
{
    /* Use the passed-in pointer - unused pointers violate MISRA rule 16.7 */
    char_t *pUnused = pTaskName;
    ADI_OSAL_THREAD_HANDLE hUnused =hThread;

    /* VDK does not support the adi_osal_ThreadGetName API */
    return ADI_OSAL_FAILED;
}


/*****************************************************************************
adi_osal_ThreadGetHandle()

Description:
    To get the task ID (handle) of the current task.

    Note: * Call to this function must not fail. However, phTaskID set to "
    ADI_OSAL_INVALID_THREAD_ID" in case if it failed. 

Arguments:
    phThread     
        pointer to a location to write the current task's handle (return value)

Return Values:

* None 


*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadGetHandle(ADI_OSAL_THREAD_HANDLE *phThread)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

    /* VDK can return an ISR-level code and a Kernel Level code if this API
     * is called at either of these levels.  OSAL knows nothing of them, so 
     * we're going to check the level beforehand and return a FAIL if it's not 
     * at thread level */ 
    if((!VDK_IsRunning()) || (InISR()))
    {
        /* We're either at startup or at an ISR level */
        result = ADI_OSAL_FAILED;
    }
    else
    {
        *phThread = (ADI_OSAL_THREAD_HANDLE)VDK_GetThreadID();
    }

    return result; 
}


/*****************************************************************************
Function: adi_osal_GetTaskPrio()
Description:
To get the priority of the current task.

Arguments:
    pnTaskPrio     
        pointer to a location to write the current task priority (return value)

Return Values:
ADI_OSAL_SUCCESS            
    The thread priority was returned successfully.  
ADI_OSAL_FAILED             
    The thread priority could not be obtained.
ADI_OSAL_BAD_HANDLE    [D]  
    The specified thread handle is invalid.
ADI_OSAL_CALLER_ERROR  [D]  
    If this function is invoked from an invalid location (i.e. an ISR). 

Notes:

    * pnTaskPrio is set to "ADI_OSAL_ERROR_PRIORITY" if this function failed 
      to get the priority of the current task. 
    
    * The priority returned is not the VDK priority caste into an OSAL
      priority form.  The VDK priority is transformed into the OSAL range
      where 0 is the highest priority (kPriority1) and 30 the lowest 
      (kPriority30).

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ThreadGetPrio(ADI_OSAL_THREAD_HANDLE  const hThread, 
                       ADI_OSAL_PRIORITY       *pnThreadPrio)
{
    VDK_Priority priority; 
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

#ifdef OSAL_DEBUG

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if( (!VDK_IsRunning()) || (InISR()) )
    {
        return ADI_OSAL_CALLER_ERROR;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/
    
    priority = VDK_GetPriority((VDK_ThreadID)hThread);

    if((int)UINT_MAX == priority)
    {
#ifdef OSAL_DEBUG
        result = ADI_OSAL_BAD_HANDLE;
#else
        /* Depending on the configuration, this could mean that the thread
         * does not exist or that the API is being called incorrectly */
        result = ADI_OSAL_FAILED;
#endif /* OSAL_DEBUG */
    }
    else
    {
        *pnThreadPrio = PriorityVdkToOsal(priority);
    }

    return result;
}


/*****************************************************************************
adi_osal_SetTaskPrio()

Description:
    adi_osal_ThreadSetPrio function is used to set the priority level of the 
    specified thread.  This function is helpful for use with priority inversion
    .  If priority inheritance is not supported by the underlying operating 
    system, this API can be used to manually shift the priority of the thread 
    to overcome deadlock caused by priority inversion.

Conditions
    This function must not be called from an ISR
    The range of allowable priority levels varies depending on the underlying 
    OS, as defined in Section 4.3.1.9.  Please refer to Section 3.2.1 in the 
    Quick Start for more information on maintaining portability.
    If an attempt is made to set more than one thread to the same priority 
    level, some Operating Systems may not allow this, and an 
    ADI_OSAL_PRIORITY_IN_USE error may occur.  The application should plan 
    accordingly if maximum portability is required.

Arguments
    hThread     [In]        
        The handle of the thread to change priority level.
    nNewPriority        [In]    
        The new priority level for the thread.

Return Values
ADI_OSAL_SUCCESS        
    The thread priority was changed successfully.  
ADI_OSAL_FAILED     
    The thread priority could not be changed.
ADI_OSAL_PRIORITY_IN_USE        
    The specified priority is already in use by a thread.  This error will 
    only be returned if the underlying OS does not support multiple threads 
    with the same priority level.
ADI_OSAL_BAD_HANDLE [D] 
    The specified thread handle is invalid.
ADI_OSAL_BAD_PRIORITY   [D] 
    The specified thread priority is invalid for the underlying OS.
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadSetPrio(ADI_OSAL_THREAD_HANDLE const hThread,
                       ADI_OSAL_PRIORITY nNewPriority)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    VDK_Priority newPriority = PriorityOsalToVdk(nNewPriority);

#ifdef OSAL_DEBUG

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if( (!VDK_IsRunning()) || (InISR()) )
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    
    if( (newPriority > VDK_kMaxPriority) ||
           (newPriority < VDK_kMinPriority) )
    {
        return ADI_OSAL_BAD_PRIORITY;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/

    VDK_ClearThreadError();

    VDK_SetPriority((VDK_ThreadID)hThread, newPriority);

    if(VDK_kNoError != VDK_GetLastThreadError())
    {
        /* A bad threadID and bad priority are only returned with OSAL_DEBUG
         * Here we know that a bad priority will already be caught, so any
         * failure will be a bad threadID. For non DEBUG OSAL, it's just a
         * failure */
#ifdef OSAL_DEBUG
        result = ADI_OSAL_BAD_HANDLE;
#else
        result = ADI_OSAL_FAILED;
#endif /* OSAL_DEBUG */
    }
    
    return result;
}


/*****************************************************************************
adi_osal_ThreadSleep

Description
    The adi_osal_ThreadSleep function is used to suspend execution of the 
    current thread for a specified period of time.

Conditions
    A value of zero for nTimeInTicks means that the thread is not suspended or 
    delayed, and this call essentially does nothing (it returns immediately).

Arguments
    nTimeInTicks    [In]    
        Specifies how long to put the thread to sleep, in system ticks.

Return Values
ADI_OSAL_SUCCESS        
    The sleep completed successfully.  
ADI_OSAL_FAILED     
    An error occurred while putting the thread to sleep, or the sleep period 
    did not complete (for example, if it was aborted by some other method).
ADI_OSAL_BAD_TIME   [D] 
    The value specified for nTimeInTicks is invalid for the underlying OS (See 
    Section 4.3.1.10). Please refer to Section 5.1 for details on choosing 
    values for maximum OS compatibility.
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadSleep(ADI_OSAL_TICKS nTimeInTicks)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

#ifdef OSAL_DEBUG

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if( (!VDK_IsRunning()) || (InISR()) )
    {
        return ADI_OSAL_CALLER_ERROR;
    }

    /* VDK's timeouts are limited to 31 bits (INT_MAX) */
    if(nTimeInTicks >= (unsigned int)INT_MAX)
    {
        return ADI_OSAL_BAD_TIME;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/
   
    /* A sleep period of 0 produces an error in VDK, but is permissable in 
     * OSAL. In OSAL, a zero sleep time simply returns. */ 
    if(0u != nTimeInTicks)
    {
        VDK_ClearThreadError();
    
        VDK_Sleep(nTimeInTicks);
    
        if(VDK_kNoError != VDK_GetLastThreadError())
        {
            /* The only return we get is for a bad time, but that should be
             * caught above in non-DEBUG OSAL.  Here we just return fail. */
            result = ADI_OSAL_FAILED;
        }
    }

    return result;
}

#pragma diag(pop)
