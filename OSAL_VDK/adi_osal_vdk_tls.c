/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: 
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
#if defined (__ADSPBLACKFIN__)
#include <ccblkfn.h>
#endif

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=ThreadLocalStorage")
#pragma file_attr("ThreadLocalStorage")

 
/*****************************************************************************
adi_osal_ThreadSlotAcquire

Description
    The adi_osal_ThreadSlotAcquire function is used to allocate a slot (a 
    memory location of word - 4 byte - size) from the thread local storage 
    buffer for this thread, and returns the slot number.  This is a non-
    blocking call.

Conditions
    The slot numbers are allocated across all threads, thus slots cannot 
    individually be allocated for each thread - once a slot is allocated it is 
    done so for all threads (the slot becomes reserved in each thread's table).
    If a free slot cannot be found (i.e. any error occurs during acquisition), 
    the value in *pnThreadSlotNum will be set to ADI_OSAL_INVALID_THREAD_SLOT.
    The maximum number of thread local storage slots supported is determined 
    by the nNumTLSSlots configuration value supplied as part of the 
    ADI_OSAL_CONFIG structure passed into adi_osal_Init (refer to Section 
    4.3.2 .1)
    The callback indicated by pCallbackFunc is called when any thread 
    terminates, and is called within the context of the thread destroy 
    function being used to terminate the thread.  This callback is typically 
    used to free up resources that may be stored in the indicated slot for the 
    terminating thread.  The return value of the callback indicates its 
    success or failure (see Section 4.3.1.7).  This callback will be passed 
    the contents of the slot of the thread being terminated.

Arguments
    pnThreadSlotNum    [Out]    
        Pointer to the location to write the returned slot number.
    pCallbackFunc        [In]      
        Pointer to a callback to be invoked when any thread terminates.  Set 
        this value to NULL if no callback is desired.

Return Values
ADI_OSAL_SUCCESS             
    The requested slot was acquired successfully.  
ADI_OSAL_FAILED            
    An error occurred while acquiring the specified slot
ADI_OSAL_CALLER_ERROR     [D]        
    If this function is invoked from an invalid location (i.e. an ISR). 


*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ThreadSlotAcquire(ADI_OSAL_TLS_SLOT_NUM *pnThreadSlotNum,
                           ADI_OSAL_TLS_CALLBACK_PTR pTerminateCallbackFunc)
{
    ADI_OSAL_STATUS result;
    int32_t nSlot = VDK_kTLSUnallocated;
    bool vdkSuccess;
    
#if defined(OSAL_DEBUG)
#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if(InISR())
    {
        /* Cannot allocate a thread slot from an ISR */
        *pnThreadSlotNum = ADI_OSAL_INVALID_THREAD_SLOT;
        return ADI_OSAL_CALLER_ERROR;
    }
    
#pragma diag(pop)
#endif     

#pragma diag(push)
#pragma diag(suppress:misra_rule_11_1:"Allowing an ugly cast of function pointer until such times as the adi_osal_ThreadSlotAcquire API prototype changes.")

    /* Try to allocate the slot */
    if(VDK_AllocateThreadSlotEx(&nSlot, (void(*)(void*))pTerminateCallbackFunc))
    {
        /* Successful slot allocation */
         *pnThreadSlotNum = (ADI_OSAL_TLS_SLOT_NUM)nSlot;
         result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /*Allocation failed */
        *pnThreadSlotNum = ADI_OSAL_INVALID_THREAD_SLOT;
        result = ADI_OSAL_FAILED;
    }
#pragma diag(pop)

    return result;
}

/*****************************************************************************
adi_osal_ThreadSlotRelease

Description
    The adi_osal_ThreadSlotRelease function frees the specified slot in the 
    thread local storage table (i.e. makes this slot available for use) and 
    clears out any registered callback for the slot.
Conditions
    This function does not alter the contents of the thread local storage 
    buffer for any given thread.  It only affects the availability of the 
    global slot. It is the applications responsibility to ensure that no thread
    -local data is in use at the time the slot is freed; the thread 
    termination callback put in place by adi_osal_ThreadSlotAcquire is not 
    called when a slot is released.
    This will return ADI_OSAL_FAILED if an attempt is made to release a slot 
    that is not currently allocated.
    Once the slot has been successfully released, the caller should set the 
    local slot number to ADI_OSAL_INVALID_THREAD_SLOT for error checking 
    purposes (to prevent erroneous further use by subsequent calls).

Arguments
    nThreadSlotNum    [In]    
        The slot number to be freed. This is a slot number previously returned 
        by adi_osal_ThreadSlotAcquire.

Return Values
ADI_OSAL_SUCCESS             
    The specified slot was released successfully.  
ADI_OSAL_FAILED            
    An error occurred while releasing the specified slot
ADI_OSAL_BAD_SLOT_NUM    [D]     
    Specified slot number is invalid.


*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ThreadSlotRelease(ADI_OSAL_TLS_SLOT_NUM nThreadSlotNum)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
#if defined(OSAL_DEBUG)
#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* Cannot free a thread slot from an ISR */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    
#pragma diag(pop)    
#endif

    /* Free the thread slot */
    if(!VDK_FreeThreadSlot((int)nThreadSlotNum))
    {
        /* The only failure that can occur if the thread slot number is 
         * invalid. However, we handle the failure diffently if we are using 
         * OSAL_DEBUG libs */
#if defined(OSAL_DEBUG)
        result = ADI_OSAL_BAD_SLOT_NUM;
#else
        result = ADI_OSAL_FAILED;
#endif    
    }
    
    return result;
}



/*****************************************************************************
adi_osal_ThreadSlotSetValue

Description
    The adi_osal_ThreadSlotSetValue function is used to store data in the 
    specified slot for the currently executing thread.

Conditions
    Data is allocated on a per-thread basis, even though the slot number used 
    applies to all threads.
    If an attempt is made to write data to a thread slot that has not been 
    acquired using adi_osal_ThreadSlotAquire (see Section 4.2.18), the error 
    ADI_OSAL_FAILED will occur.

Arguments
    nThreadSlotNum    [In]    
        The slot number to write the data to. This is a slot number previously 
        returned by adi_osal_ThreadSlotAcquire.
    SlotValue        [In]    
        This contains the data to write to the slot. This is a word sized (4-
        byte) value, and can be a pointer.

Return Values
ADI_OSAL_SUCCESS         
    The data was successfully stored in the specified slot
ADI_OSAL_FAILED    
    An error occurred while storing the data. This error can occur if the slot 
    being written has not been previously acquired.
ADI_OSAL_BAD_SLOT_NUM    [D]     
    Specified slot number is invalid.
*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_ThreadSlotSetValue(ADI_OSAL_TLS_SLOT_NUM nThreadSlotNum,
                            ADI_OSAL_SLOT_VALUE SlotValue)
{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;
    
#if defined(OSAL_DEBUG)
#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")
    
    /* Cannot set a thread slot value from ISR */
    if((!VDK_IsRunning()) || (InISR()))
    {
        return ADI_OSAL_CALLER_ERROR;
    }
    
#pragma diag(pop)
#endif

    /* Free the thread slot */
    if(!VDK_SetThreadSlotValue((int)nThreadSlotNum, SlotValue))
    {
        /* The only failure that can occur if the thread slot number is 
         * invalid. However, we handle the failure diffently if we are using 
         * OSAL_DEBUG libs */
#if defined(OSAL_DEBUG)
        result = ADI_OSAL_BAD_SLOT_NUM;
#else
        result = ADI_OSAL_FAILED;
#endif    
    }
    
    return result;
}



/*****************************************************************************
adi_osal_ThreadSlotGetValue

Description
    The adi_osal_ThreadSlotGetValue function is used to retrieve the value 
    stored in the specified slot for the currently executing thread.

Conditions
    Data is allocated on a per-thread basis, even though the slot number used 
    applies to all threads.
    If an attempt is made to read data from a thread slot that has not been 
    acquired using adi_osal_ThreadSlotAquire (see Section 4.2.18), the error 
    ADI_OSAL_FAILED will occur.
    Upon error, the location indicated by pSlotValue will contain the value 
    zero.

Arguments
    nThreadSlotNum    [In]    
        The slot number to obtain the data from. This is a slot number 
        previously returned by adi_osal_ThreadSlotAcquire.
pSlotValue        [Out]    
        A pointer to the location to write the returned thread slot data. The 
        returned data is a word sized (4-byte) value, and could represent a 
        pointer.

Return Values
ADI_OSAL_SUCCESS         
    The data was successfully read from the specified slot
ADI_OSAL_FAILED    
    An error occurred while reading the data. This error can occur if the slot 
    being read from has not been previously acquired.
ADI_OSAL_BAD_SLOT_NUM    [D]     
    Specified slot number is invalid.
*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ThreadSlotGetValue(ADI_OSAL_TLS_SLOT_NUM nThreadSlotNum,
                            ADI_OSAL_SLOT_VALUE    *pSlotValue)
{
    void *pData;
    
#if defined(OSAL_DEBUG)
#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")
    
    /* Cannot set a thread slot value from ISR */
    if((!VDK_IsRunning()) || (InISR()))
    {
        /* There is no ADI_OSAL_CALLER_ERROR return for this API */
        return ADI_OSAL_FAILED;
    }
    
#pragma diag(pop)
#endif

    /* The API returns NULL if the call was unsuccessful, but NULL can also be
     * the stored value - so we cannot garauntee that this call has worked.
     * Always successful.*/
     
    /* Get the stored value */
    *pSlotValue = VDK_GetThreadSlotValue((int)nThreadSlotNum);
    
    return ADI_OSAL_SUCCESS;
} 


