/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for the thread-level mutex synchronization primitive. 
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

/* Required to check for UINT_MAX */
#include <limits.h>

/* Required for the definition of NULL */
#include <stddef.h>

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Mutex")
#pragma file_attr("Mutex")


/* File misra suppressions */
#pragma diag(push)   
#pragma diag(suppress:misra_rule_11_3 : "Allow integer to pointer casting for ADI_OSAL_INVALID_MUTEX token")
#pragma diag(suppress:misra_rule_8_7 : "kNoTimeoutError and kDoNotWait are currently variables, and will be changed to #defines in an updated VDK")


extern "C" {
#pragma linkage_name _InitMutex__3VDKFPQ2_3VDK5MutexUib
 void VDK_InitMutex(void *pMutex, uint32_t size, bool history);

#pragma linkage_name _DeInitMutex__3VDKFPQ2_3VDK5Mutex
 void VDK_DeInitMutex(void *pMutex);
} 

/*****************************************************************************
adi_osal_MutexGetMutexObjSize

Description
    The adi_osal_MutexGetMutexObjSize function allows the user to get the 
    exact value of the size of a Mutex object for the underlying object. It 
    allows allocation of memory to use by the adi_osal_MutexCreateStatic 
    function.

Arguments
    pnMutexObjSizeBytes   [Out] 
        Pointer to a location where the Mutex object size for the current
        OS implementation will be written.  

Return Values
ADI_OSAL_SUCCESS            
    The value was returned successfully.
******************************************************************************/
uint32_t 
adi_osal_MutexGetMutexObjSize()
{
    return VDK_MUTEX_SIZE;
}

/*****************************************************************************
adi_osal_MutexCreate

Description
    The adi_osal_MutexCreate function is used to create a mutex. 

Conditions
    In the event of failure, the value returned in *phMutex will be 
    ADI_OSAL_INVALID_MUTEX.

Arguments
    phMutex         [Out]   
        Pointer to the location to write the returned mutex handle.

Return Values
ADI_OSAL_SUCCESS    
    The mutex was created successfully.  
ADI_OSAL_FAILED 
    An error occurred while creating the mutex
ADI_OSAL_MEM_ALLOC_FAILED      
    No memory is available for the creation of the Mutex.
ADI_OSAL_CALLER_ERROR [D]   
    If this function is invoked from an invalid location (i.e. an ISR). 
*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_MutexCreate (ADI_OSAL_MUTEX_HANDLE *phMutex)
{
    
    ADI_OSAL_STATUS result;
    VDK_MutexID mutexID;
    
#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if(InISR())
    {
        /* There are no errors for passing a NULL pointer for this API */
        *phMutex = ADI_OSAL_INVALID_MUTEX;
        return ADI_OSAL_CALLER_ERROR;
    }
#pragma diag(pop)

#endif /*OSAL_DEBUG*/  
 
    /* Let VDK allocate the memory internally and create the mutex */
    mutexID = VDK_CreateMutex();
       
    if((int)UINT_MAX != mutexID)
    {
        /* We have successfully created a VDK mutex. */

        /* There are no errors for passing-in a NULL pointer for this API, so
         * we just try to populate the memory regardless. */
        *phMutex = (ADI_OSAL_MUTEX_HANDLE) mutexID;
        result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* Mutex creation failure */
        
        /* The only failure possible is when there is no memory available for 
         * the mutex. Internally VDK goes to KernelPanic if this API is
         * called at ISR or Kernel interrupt levels. */
        *phMutex = ADI_OSAL_INVALID_MUTEX;
        result = ADI_OSAL_MEM_ALLOC_FAILED;
    }

    return result;
}   



/*****************************************************************************
adi_osal_MutexCreateStatic

Description
    The adi_osal_MutexCreateStatic function is similar to the 
    adi_osal_MutexCreate function but allow the caller to pass memory to use 
    for its internal structure instead of using dynamic memory allocation. It 
    is necessary when dynamic memory allocation is not available for example 
    when the heap is not yet setup. This function can be called during 
    Startup.

Conditions
    In the event of failure, the value returned in *phMutex will be 
    ADI_OSAL_INVALID_MUTEX.

Arguments
    pMutexObject        [In]    
        Pointer to memory to be used by the mutex object. It needs to be 4-
        bytes aligned.
    nMutexObjSize       [In]    
        size of the pMutexObject in bytes. The pre-processor constant   
        ADI_OSAL_MAX_MUTEX_SIZE_BYTES ( See 4.5.1.6 can be used to  define   
        that value if the value for the specific OS is not known. This     
        value can also be determine at runtime by calling   
        adi_osal_MutexGetMutexObjSize
    phMutex         [Out]   
        Pointer to the location to write the returned mutex handle.
    
Return Values
ADI_OSAL_SUCCESS    
    The mutex was created successfully.
ADI_OSAL_FAILED 
    An error occurred while creating the mutex
ADI_OSAL_MEM_ALLOC_FAILED      
    The provided memory is not sufficient to create the mutex
ADI_OSAL_BAD_MEMORY   [D]   
    The provided memory is invalid (must be word-aligned).
ADI_OSAL_CALLER_ERROR [D]   
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_MutexCreateStatic(void* const pMutexObject,
                           uint32_t nMutexObjSize,                
                           ADI_OSAL_MUTEX_HANDLE *phMutex)
{
    ADI_OSAL_STATUS result;
    VDK_MutexID     mutexID;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* This API can be called at startup, so only a check for ISR level is
       required */
    if(InISR())
    {
        *phMutex = ADI_OSAL_INVALID_MUTEX;
        return ADI_OSAL_CALLER_ERROR;
    }

    /* Check word alignment, and make sure it is not a NULL ptr */
    if( (0u != (3u & (unsigned int)pMutexObject)) || ((int)NULL == (int)pMutexObject))
    {
        *phMutex = ADI_OSAL_INVALID_MUTEX;
        return ADI_OSAL_BAD_MEMORY;
    }

#pragma diag(pop)

#endif /*OSAL_DEBUG*/

    if(nMutexObjSize >= VDK_MUTEX_SIZE)
    {
            /* This call cannot fail in 5.0.
             * We do not want to log history for OSAL mutex use. */ 
            VDK_InitMutex(pMutexObject, nMutexObjSize, false);

            /* We have a valid MutexID */
            *phMutex = (ADI_OSAL_MUTEX_HANDLE)pMutexObject;
            result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* The supplied memory size was not large enough */
        *phMutex = ADI_OSAL_INVALID_MUTEX;
        result = ADI_OSAL_MEM_ALLOC_FAILED;
    }

    return result;
}

/*****************************************************************************
adi_osal_MutexDestroyStatic

Description
    This function is used to delete a mutex that has been allocated with
    adi_osal_MutexCreateStatic().

    
Arguments
    hMutexID      - Handle of the mutex to be deleted

Returns

ADI_OSAL_SUCCESS      
    If mutex is deleted successfully
ADI_OSAL_FAILED       
    If failed to delete mutex
ADI_OSAL_BAD_HANDLE   
    If the specified mutex handle is invalid
ADI_OSAL_CALLER_ERROR 
    If function is invoked from an invalid location
ADI_OSAL_THREAD_PENDING  
    one or more thread is pending on the specified mutex.

Notes:
      Only owner is authorised to release the acquired mutex. But it
      can be "destroyed" by any task.

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_MutexDestroyStatic(ADI_OSAL_MUTEX_HANDLE const hMutex)
{

    VDK_MutexID     mutexID;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* This API can be called at startup, so only a check for ISR level is
       required */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }

    if((NULL == hMutex) || (ADI_OSAL_INVALID_MUTEX == hMutex))
    {
        return ADI_OSAL_BAD_HANDLE; 
    }

#pragma diag(pop)

#endif /*OSAL_DEBUG*/

   /* There is no way to check if this Mutex is valid, so we pass it to the
    * DeInitMutex API and see what happens.  If it is invalid then VDK goes to
    * KernelPanic for Full Instrumentation or Error Checking builds.  For
    * Non Error Checking, or a valid mutex, we return here assuming the
    * DeInit was successful.*/
    VDK_DeInitMutex((void *)hMutex); 

    /* We have to assume success here */    
    return ADI_OSAL_SUCCESS; 
}


/*****************************************************************************
adi_osal_MutexDestroy

Description
    The adi_osal_MutexDestory function is used to delete a mutex previously 
    created using the adi_osal_MutexCreate function.

Conditions
    If an attempt is made to delete a mutex when a thread is pending on it, 
    the function will return an ADI_OSAL_THREAD_PENDING result. Although some 
    Operating Systems will delete the mutex and unblock all pending threads, 
    this behaviour is not supported in the OSAL to ensure 
    portability.
    Once the mutex is successfully deleted, the caller should set the handle 
    to ADI_OSAL_INVALID_MUTEX for error checking purposes.  The handle is not 
    modified by this call, and thus could be erroneously used for subsequent 
    calls.
    This call should be used with care since other threads may be relying on 
    the presence of this mutex, and multiple threads could attempt to access a 
    possibly deleted mutex.  Generally, the user must first delete all threads 
    that can access the mutex before deleting it.
    A mutex cannot be deleted from an ISR.

Arguments
    hMutex          [In]    
        Handle of the mutex to destroy.

Return Values
ADI_OSAL_SUCCESS        
    The mutex was deleted successfully.  
ADI_OSAL_FAILED     
    An error occurred while deleting the mutex.  This can occur if there are 
    other threads pending on this mutex.
ADI_OSAL_THREAD_PENDING     
    A thread is pending on the specified mutex.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified mutex handle is invalid. 
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_MutexDestroy (ADI_OSAL_MUTEX_HANDLE const hMutex)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* This API can be called at startup, so only a check for ISR level is
       required */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }
#pragma diag(pop)
    
#endif /*OSAL_DEBUG*/ 

    /* Calls to DestroyMutex() requires the use of GetLastThreadError() to
       establish if any failures occurred.  Any previous thread errors have to 
       be cleared before we make the DestroyMutex() call. */ 
       
    /* For threads created using OSAL, or for threads that ignore this error 
     * in their Error() functions, the error will be logged and execution will 
     * return here.
     * Note that for Non Error Checking builds of VDK the error is not logged
     * and this call will appear successful.
     */
    VDK_ClearThreadError();

    VDK_DestroyMutex((VDK_MutexID)hMutex);

    /* Check for any problems during destruction. */
    switch(VDK_GetLastThreadError())
    {
        case VDK_kNoError:
            /* Everything went well */
            break;
 
#if defined(OSAL_DEBUG)
        case VDK_kInvalidMutexID:
            result = ADI_OSAL_BAD_HANDLE; 
            break;
#endif /*OSAL_DEBUG*/

        case VDK_kMutexDestructionFailure:  /* Fall Through */        
        default:
            /* Catch-all for failures */
            /* A mutex cannot be destroyed if it is owned, as there may be 
             * threads pending on the mutex. VDK only checks if the mutex is 
             * owned, so we cannot return ADI_OSAL_THREAD_PENDING as this may 
             * not be true */  
            result = ADI_OSAL_FAILED;
            break;
    }

    return result;
}




/*****************************************************************************
adi_osal_MutexPend

Description
    The adi_osal_MutexPend function is used to wait for and acquire a mutex 
    lock. This supports recursive acquisition (repeated attempts to lock the 
    mutex by the same owner), and internally tracks the number of acquisitions 
    and releases. 

Conditions
    If a timeout value of ADI_OSAL_TIMEOUT_NONE is used, this function will 
    immediately return regardless of whether the mutex was successfully 
    acquired.  In this case, the return code will be ADI_OSAL_FAILED if the 
    mutex was not acquired.
    If a timeout value of ADI_OSAL_TIMEOUT_FOREVER is used, the calling thread 
    will suspend until the mutex is acquired.
    The behaviour upon overflow of the ownership count is OS dependent; if the 
    OS supports an error check, then the error ADI_OSAL_COUNT_OVERFLOW will be 
    returned.  If not, then the behaviour is dependent on the implementation 
    in the OSAL and on the underlying OS behaviour, and may return the 
    overflow error, clip or wrap the count.  It is unlikely that this 
    situation would occur, but the user should be aware of the potential for 
    unusual behaviour.
    A mutex cannot be acquired from an ISR.

Arguments
    hMutex          [In]    
        Handle of the mutex to destroy.
    nTimeoutInTicks [In]    
        The timeout, in system ticks, for acquiring the mutex. This can take 
        any of the values specified in Section 4.3.1.10 

Return Values
ADI_OSAL_SUCCESS    
    The mutex was acquired successfully.  
ADI_OSAL_FAILED 
    An error occurred while acquiring the mutex
ADI_OSAL_TIMEOUT    
    If the call failed to acquire the mutex within the specified timeout period.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified mutex handle is invalid. 
ADI_OSAL_BAD_TIME   [D] 
    The value specified for the timeout (nTimeoutInTicks) is invalid for the 
    underlying OS.  Please refer to Section 4.3.1.10 for details of timeout 
    ranges for maximum compatibility.
ADI_OSAL_COUNT_OVERFLOW [D] 
    The ownership count overflowed.
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_MutexPend(ADI_OSAL_MUTEX_HANDLE const     hMutex, 
                   ADI_OSAL_TICKS               nTimeoutInTicks)
{
    ADI_OSAL_STATUS result;
    uint32_t nVdkTimeout;  /* Using uint32_t instead of Ticks for misra complience */
    bool bMutAcquired = false;
    
#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if((!VDK_IsRunning()) || (InISR()))
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    

    /* In 5.0 VDK does not have a PendMutex API, so we can only acquire */
    if( ADI_OSAL_TIMEOUT_FOREVER != nTimeoutInTicks )
    {
        return ADI_OSAL_BAD_TIME;
    }

#pragma diag(pop)

#endif /*OSAL_DEBUG*/

    /* We rely on the GetLastThreadError() API to see if there's 
     * anything wrong. To do that we must ensure any previous errors are 
     * removed. */
    VDK_ClearThreadError(); 

    /* Try to acquire the mutex */
    VDK_AcquireMutex((VDK_MutexID)hMutex);

    /* A failure occured */

    /* We need to figure out if there was a timeout. OSAL shouldn't 
     * go to the error function unless it can be avoided, but 
     * unfortunately for the TIMEOUT error this is the only way that 
     * a Thread Error is logged.  Here we check for all the errors, 
     * and if no error is logged for a failed Pend then it can only 
     * be a timeout. 
     * 
     * In Non Error Checking builds of VDK, the only error return is
     * timeout - the call will either be successful, timeout or 
     * the behaviour will be undefined. */
    
    switch(VDK_GetLastThreadError())
    {
        case VDK_kNoError:
            result = ADI_OSAL_SUCCESS;
            break;
        case VDK_kInvalidMutexID:
#if defined(OSAL_DEBUG)
            result = ADI_OSAL_BAD_HANDLE;
            break;
#endif
            /* Fall through for non OSAL_DEBUG */
        case VDK_kDbgPossibleBlockInRegion:   
        case VDK_kBlockInInvalidRegion:
            /* These errors should be checked for OSAL_DEBUG above,
            * for the release case they are just failures. */
        default:
            result = ADI_OSAL_FAILED;
            break;
    }  
                        
    return result;  
}



/*****************************************************************************
adi_osal_MutexPost

Description
    The adi_osal_MutexPost function allows a thread to unlock (release) a 
    mutex that it had previously acquired using adi_osal_MutexPend. When a 
    mutex is unlocked, the highest priority thread waiting on the mutex will 
    resume execution.

Arguments
    hMutex          [In]    
        Handle of the mutex to unlock.

Return Values
ADI_OSAL_SUCCESS        
    The mutex was released successfully.  
ADI_OSAL_FAILED     
    An error occurred while releasing the mutex
ADI_OSAL_NOT_MUTEX_OWNER        
    The specified mutex is not owned by the caller
ADI_OSAL_BAD_HANDLE [D] 
    If the specified mutex handle is invalid. 
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_MutexPost (ADI_OSAL_MUTEX_HANDLE const hMutex)
{
    VDK_SystemError vdkError;
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if((!VDK_IsRunning()) || (InISR()))
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    
#pragma diag(pop)
        
#endif /*OSAL_DEBUG*/
 
    /* There's nothing we can do in the OSAL to check that the mutex is owned,
     * or not owned by this thread, so that check will be handled by VDK.
     * In the case of an unknown mutex VDK dispatches a Thread Error. For 
     * threads created using OSAL, or for threads that ignore this error in
     * their Error() functions, the error will be logged and execution will 
     * return here.
     * Note that for Non Error Checking builds of VDK the error is not logged
     * and this call appears successful.*/
    VDK_ClearThreadError();
    
    /* Do the work of releasing the Mutex */
    VDK_ReleaseMutex((VDK_MutexID)hMutex);

    /* ReleaseMutex does not return an error directly, we have to query
     * the last thread error for success. */
    switch(VDK_GetLastThreadError())
    {
        case VDK_kNoError:
            result = ADI_OSAL_SUCCESS;
            break;
        case VDK_kInvalidMutexOwner:
        case VDK_kNotMutexOwner:
        case VDK_kMutexNotOwned:
            result = ADI_OSAL_NOT_MUTEX_OWNER;
            break;        
        case VDK_kInvalidMutexID:
#if defined(OSAL_DEBUG)
            result = ADI_OSAL_BAD_HANDLE;
            break;    
#endif
        default:
            result = ADI_OSAL_FAILED;
            break;
    }

    return result;
}


#pragma diag(pop)

