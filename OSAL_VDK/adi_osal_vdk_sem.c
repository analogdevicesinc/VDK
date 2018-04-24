/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL on VDK semaphore APIs.
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


/* File attributes */
#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Semaphores")
#pragma file_attr("Semaphores")


/* File misra suppressions */
#pragma diag(push) 
#pragma diag(suppress:misra_rule_11_3 : "Allow integer to pointer casting for ADI_OSAL_INVALID_SEM token")

/*****************************************************************************
adi_osal_SemCreate

Description
    The adi_osal_SemCreate function is used to create a counting semaphore (a 
    binary semaphore is simply a semaphore whose count only goes between 0 and 
    1). 

Conditions
    In the event of failure, the value returned in *phSem will be 
    ADI_OSAL_INVALID_SEM.
    The OSAL does not support periodic semaphores. It is expected that if a 
    periodic semaphore is required, that the equivalent behaviour is created 
    using timer services.
    OSAL semaphores also do not support specification of a maximum value for 
    the count.  The max is always the maximum supported by the size of the 
    count implemented by the underlying OS.  Note that the maximum range of 
    the count varies from one Operating System to another; therefore please 
    refer to Section 5.1 for compatibility information.

Arguments
    phSem       [Out]   
        Pointer to the location to write the returned semaphore handle.
    nInitCount  [In]    
        Specifies the initial count with which the semaphore needs to be 
        created. A non-zero value is typically used for resource management. A 
        value of zero for nInitCount can be used to create a semaphore with 
        unavailable state, or where the semaphore is used as a synchronization 
        tool.  This initial count must be less than the maximum count 
        supported by the underlying operating system  refer to Section 5.1 for 
        details.
    
Return Values
ADI_OSAL_SUCCESS        
    The semaphore was created successfully.  
ADI_OSAL_FAILED     
    An error occurred while creating the semaphore 
ADI_OSAL_MEM_ALLOC_FAILED          
    No memory is available for the creation of the Semaphore.
ADI_OSAL_BAD_COUNT  [D] 
    The value specified in nInitCount exceeds the capabilities of the OS.
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_SemCreate(ADI_OSAL_SEM_HANDLE *phSem, uint32_t nInitCount)
{

    ADI_OSAL_STATUS result;
    VDK_SemaphoreID vdkSemID;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if(InISR())
    {
        /* There are no errors for passing a NULL pointer for this API */
        *phSem = ADI_OSAL_INVALID_SEM;
        return ADI_OSAL_CALLER_ERROR;
    }

    /* The maximum count and maximum initial value for a VDK Semaphore is
     * UINT_MAX - the maximum value achievable by 32bit unsigned int. 
     * However, as VDK_GetSemaphoreValue() returns a count of UINT_MAX for an 
     * invalid SemaphoreID we are going to use UINT_MAX - 1 as the maximum. */
    if(nInitCount > VDK_SEMAPHORE_MAX_COUNT)
    {
       *phSem = ADI_OSAL_INVALID_SEM;
        return ADI_OSAL_BAD_COUNT;
    }   

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/
  
#endif /*OSAL_DEBUG*/  

    /* The errors from this VDK API call with the following arguments are 
     * limited to a kAPIUsedfromISR kernel panic (which will not return), or an
     * error if it cannot create the semaphore because there is not any space.
     * If the API returns then it was either successful or there was not enough
     * memory. */
    vdkSemID = VDK_CreateSemaphore(nInitCount, VDK_SEMAPHORE_MAX_COUNT, 0u, 0u);

    if(((int)UINT_MAX) != vdkSemID)
    {
        /* Successful creation of the VDK Semaphore */
        *phSem = (ADI_OSAL_SEM_HANDLE)vdkSemID;
        result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* VDK was unable to create the Semaphore due to memory constraints
         * or there are too many semaphores already in the system (5.0)
         * We can't differentiate between the two, so returning 'failed'. */
        *phSem = ADI_OSAL_INVALID_SEM;
        result = ADI_OSAL_FAILED;
    }

    return result;
}


/*****************************************************************************
adi_osal_SemDestroy

Description
    The adi_osal_SemDestroy function is used to delete a semaphore previously 
    created using the adi_osal_SemCreate function.

Conditions
    If a thread is pending on the semaphore to be destroyed, this function 
    will return an ADI_OSAL_THREAD_PENDING result. Although some Operating 
    Systems will delete the semaphore and unblock all pending threads, this 
    behaviour is not supported in the OSAL to ensure portability.
    Once the semaphore is successfully deleted, the caller should set the 
    handle to ADI_OSAL_INVALID_SEM for error checking purposes.  The handle is 
    not modified by this call, and thus could be erroneously used for 
    subsequent calls. It is the applications responsibility to ensure that 
    threads do not use deleted semaphores.
    This call should be used with care since other threads may be relying on 
    the presence of this semaphore, and multiple threads could attempt to 
    access a possibly deleted semaphore.  Generally, the user must first 
    delete all threads that can access the semaphore before deleting it.
    A semaphore cannot be deleted from an ISR.

Arguments
    hSem            [In]    
        Handle of the semaphore to destroy.

Return Values
ADI_OSAL_SUCCESS        
    The semaphore was deleted successfully.  
ADI_OSAL_FAILED     
    An error occurred while deleting the semaphore.  
ADI_OSAL_THREAD_PENDING     
    A thread is pending on the specified semaphore.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified semaphore handle is invalid. 
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_SemDestroy(const ADI_OSAL_SEM_HANDLE hSem)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* This API can be called at startup, so only a check for ISR level is
     * required */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/

    /* Calls to DestroySemaphore() require the use of GetLastThreadError() to
     * establish if any failures occurred.  Any previous thread errors have to
     * be cleared before we make the DestroySemaphore() call. */
       
    /* For threads created using OSAL, or for threads that ignore this error 
     * in their Error() functions, the error will be logged internally by VDK
     * and execution will return here.
     * Note that for Non Error Checking builds of VDK the error is not logged
     * and this call appears successful.
     */
    VDK_ClearThreadError();

    VDK_DestroySemaphore((VDK_SemaphoreID)hSem);

    /* Check for any problems during destruction. */
    switch(VDK_GetLastThreadError())
    {
        case VDK_kNoError:
            /* Everything went well */
            break;

        case VDK_kSemaphoreDestructionFailure:
            /* This failure indicates that the SemaphoreID is valid, but there i
             * is at least one thread pending on the semaphore. */  
            result = ADI_OSAL_THREAD_PENDING;
            break;

#if defined(OSAL_DEBUG)
        case VDK_kUnknownSemaphore:
            /* The error is the same - unknown semaphore, but the returns are
             * different depending on the OSAL configuration. */
            result = ADI_OSAL_BAD_HANDLE;
            break;
#endif /*OSAL_DEBUG*/
        
        default:
            result = ADI_OSAL_FAILED;
            break;
    }

    return result;    
}


/*****************************************************************************
adi_osal_SemPend

Description
    The adi_osal_SemPend function is used to wait for access to a resource, 
    synchronise with other threads or ISRs or to wait for an event.  If the 
    semaphore count is non-zero a pend call will decrement the count and 
    return to the calling thread.  If the semaphore count is zero, the thread 
    is suspended until the count becomes non-zero.

Conditions
    If a timeout value of ADI_OSAL_TIMEOUT_NONE is used, this function will 
    immediately return regardless of the semaphore count.  In this case, the 
    return code will be ADI_OSAL_FAILED if the semaphore's count is zero.
    If a timeout value of ADI_OSAL_TIMEOUT_FOREVER is used, the calling thread 
    will suspend until the semaphore is acquired.
    A semaphore can not be acquired from an ISR.
    In a no-OS system, use of a timeout other than ADI_OSAL_TIMEOUT_NONE or 
    ADI_OSAL_TIMEOUT_FOREVER will result in an ADI_OSAL_BAD_TIME error if the 
    sempahore is not available.

Arguments
    hSem            [In]    
        Handle of the semaphore to obtain.
    nTimeoutInTicks [In]    
        The timeout, in system ticks, for acquiring the semaphore. This can 
        take any of the values specified in Section 4.3.1.10).

Return Values
ADI_OSAL_SUCCESS        
    The semaphore was acquired successfully.  
ADI_OSAL_FAILED     
    An error occurred while acquiring the semaphore.
ADI_OSAL_TIMEOUT    
    If the call failed to acquire the semaphore within the specified timeout 
    period.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified semaphore handle is invalid. 
ADI_OSAL_BAD_TIME   [D] 
    The value specified for the timeout (nTimeoutInTicks) is invalid for the 
    underlying OS.  Please refer to Section 4.3.1.10 for details of timeout 
    ranges for maximum compatibility.
ADI_OSAL_CALLER_ERROR [D]   
    If this function is invoked from an invalid location (i.e. an ISR).

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_SemPend( const ADI_OSAL_SEM_HANDLE hSem, ADI_OSAL_TICKS nTimeoutInTicks)
{
    ADI_OSAL_STATUS result;
    bool bSemAcquired = false;
    
    /* VDK_Ticks, using uint32_t for misra compliance */
    uint32_t nVdkTimeout;            

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if((!VDK_IsRunning()) || (InISR()))
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    
    /* Holding a lock when pending with a timeout other than 
     * ADI_OSAL_TIMEOUT_NONE is a dangerous thing to do. If a lock is
     * held we return an error.  */
    if( (VDK_IsInRegion()) && (ADI_OSAL_TIMEOUT_NONE != nTimeoutInTicks) )
    {
       return ADI_OSAL_CALLER_ERROR; 
    }

    /* Is the timeout within VDK limits */
    if( (VDK_MAX_TIMEOUT < nTimeoutInTicks) &&
        (ADI_OSAL_TIMEOUT_NONE != nTimeoutInTicks) &&
        (ADI_OSAL_TIMEOUT_FOREVER != nTimeoutInTicks) )
    {
        return ADI_OSAL_BAD_TIME;
    }


#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/

    /* Clear any thread error, as we need to check them after the pend. */
    VDK_ClearThreadError();
  
    /* Convert the OSAL timeout into a VDK timeout */ 
    switch (nTimeoutInTicks)
    { 
        case ADI_OSAL_TIMEOUT_NONE:
            nVdkTimeout = VDK_kDoNotWait; 
            break;
        
        case ADI_OSAL_TIMEOUT_FOREVER:
            nVdkTimeout = 0u;  
            break;

        default:
            /* We have been passed a timeout */
            /* For a given number of Ticks, we do not want to go to the
             * thread error function if there was a timeout. */
            nVdkTimeout = (nTimeoutInTicks | VDK_kNoTimeoutError);
            break;
    }
        
    /* Try to pend on the semaphore */
    bSemAcquired = VDK_PendSemaphore((VDK_SemaphoreID)hSem, nVdkTimeout);

    if(bSemAcquired)
    {
        /* Successful pend */
        result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* A failure occurred */
        
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
            case VDK_kUnknownSemaphore:
#if defined(OSAL_DEBUG)
                result = ADI_OSAL_BAD_HANDLE;
                break;
#endif
                /* Fall through for non OSAL_DEBUG */
            case VDK_kDbgPossibleBlockInRegion:   /* Fall through */
            case VDK_kBlockInInvalidRegion:       /* Fall through */
            case VDK_kInvalidTimeout:             /* Fall through */
                /* These errors should be checked for OSAL_DEBUG above,
                 * for the release case they are just failures. */
                result = ADI_OSAL_FAILED;
                break;
            default:
                /* If there are no other thread errors, and the Pend failed
                 * then a timeout must have occurred or the semaphore was
                 * not available for a kDoNotWait. */
                if(VDK_kDoNotWait == nVdkTimeout)
                {
                    result = ADI_OSAL_FAILED;
                }
                else
                {
                    result = ADI_OSAL_TIMEOUT;
                }
                break;
        }  
                        
    }

    return result;
}

/*****************************************************************************
adi_osal_SemPost

Description
    The adi_osal_SemPost function allows a thread to release a semaphore that 
    it had previously acquired using adi_osal_SemPend. The semaphore count is 
    incremented by this call, and any threads waiting for the semaphore are 
    scheduled.

Conditions
    The behaviour upon semaphore count overflow is OS dependent; if the OS 
    supports an error check, then the error ADI_OSAL_COUNT_OVERFLOW will be 
    returned.  If not, then the behaviour is dependent on the implementation 
    in the OSAL and on the underlying OS behaviour, and may either clip or 
    wrap the count.  Therefore it is up to the user to ensure that this 
    situation does not occur.

Arguments
    hSem            [In]    
        Handle of the semaphore to release.

Return Values
ADI_OSAL_SUCCESS        
    The semaphore was released successfully.  
ADI_OSAL_FAILED     
    An error occurred while releasing the semaphore
ADI_OSAL_BAD_HANDLE [D] 
    If the specified semaphore handle is invalid. 
ADI_OSAL_COUNT_OVERFLOW     [D] 
    The semaphore count overflowed

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_SemPost(const ADI_OSAL_SEM_HANDLE hSem)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

    /* We have to check kernel level first, as the kernel interrupt is
     * flagged as an ISR with the InISR() call. */
    if(VDK_KERNEL_LEVEL_ == (unsigned int)VDK_GetThreadID())
    {
       /* At Kernel level we have no way to check this call was successful. */
       VDK_PostSemaphore((VDK_SemaphoreID)hSem);
    }
    else if(InISR())
    {
       /* At ISR level we have no way to check this call was successful. */
       VDK_C_ISR_PostSemaphore((VDK_SemaphoreID)hSem);
    }
    else
    {
        /* We will be looking at any errors when (if) this call returns.*/
        VDK_ClearThreadError();
    
        /* PostSemaphore will handle the invalid ID case */
        VDK_PostSemaphore((VDK_SemaphoreID)hSem);
       
        switch(VDK_GetLastThreadError())
        {
            case VDK_kNoError:
                /* Everything went well */
                break;
 
#if defined(OSAL_DEBUG)
            case VDK_kUnknownSemaphore:
                result = ADI_OSAL_BAD_HANDLE;
                break;
#endif
            default:
                result = ADI_OSAL_FAILED;
                break;
        }
    }

    return result;
}


#pragma diag(pop)  /* File Misra suppressions */
