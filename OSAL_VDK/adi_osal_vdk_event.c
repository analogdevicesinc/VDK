/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL event APIs.
 *
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The OSAL message queue is built on the VDK Fifo buffer 
 *            functionality, NOT VDK messaging. 
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
#pragma file_attr("OS_Component=Events")
#pragma file_attr("Events")


/*****************************************************************************
adi_osal_EventGroupCreate

Description
    The adi_osal_EventGroupCreate function is used to create an event flags 
    register containing up to 31 event flag bits, as defined in Section 
    4.3.1.11.

Conditions
    All flags in the event group are set to an initial value of zero.
    In the event of failure, the value returned in *phEventGroup will be set 
    to ADI_OSAL_INVALID_EVENT_GROUP.

Arguments
    phEventGroup      [Out] 
        Pointer to the location to write the returned handle of the created 
        event group.

Return Values
ADI_OSAL_SUCCESS    
        The event group was created successfully.
ADI_OSAL_FAILED 
        An error occurred while creating the event group. 
ADI_OSAL_MEM_ALLOC_FAILED  
    There was insufficient memory to allocate the space for this event group.
ADI_OSAL_CALLER_ERROR [D]   
        If this function is invoked from an invalid location (i.e. an ISR). 

*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_EventGroupCreate(ADI_OSAL_EVENT_HANDLE   *phEventGroup)
{
    /* Events are not currently supported on the VDK port.  This is a stub
     * that returns fail in all cases */

    /* Use the passed-in pointer to avoid MISRA rule 16.7 */
    ADI_OSAL_EVENT_HANDLE *pUnused = phEventGroup;

    return ADI_OSAL_FAILED;
}



/*****************************************************************************
adi_osal_EventGroupDestroy

Description
    The adi_osal_EventGroupDestroy function is used to remove the specified 
    event group.

Conditions
    This function must be used with care since multiple threads could attempt 
    to access a possibly deleted event group.  Generally, the user must first 
    delete all threads that can access the event group before deleting 
    it.
    Once the event group is successfully deleted, the caller should set the 
    handle to ADI_OSAL_INVALID_EVENT_GROUP for error checking purposes.  The 
    handle is not modified by this call, and thus could be erroneously used 
    for subsequent calls.
    If an attempt is made to delete an event group when a thread is pending on 
    it, the function will return an ADI_OSAL_THREAD_PENDING result. Although 
    some Operating Systems will delete the event group and unblock all pending 
    threads, this behaviour is not supported in the OSAL to ensure 
    portability.

Arguments
hEventGroup         [In]    
        The handle of the event group to be destroyed.  This is the value 
        returned by adi_osal_EventGroupCreate when the event group was 
        created.

Return Values
ADI_OSAL_SUCCESS        
    The event group was deleted successfully.  
ADI_OSAL_FAILED     
    An error occurred while removing the event group.
ADI_OSAL_THREAD_PENDING     
    A thread is pending on the specified event group.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified event group handle is invalid. 
ADI_OSAL_CALLER_ERROR   [D] 
    If this function is invoked from an invalid location (i.e. an ISR)

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_EventGroupDestroy(ADI_OSAL_EVENT_HANDLE const hEventGroup)
{

    /* Events are not currently supported on the VDK port.  This is a stub
     * that returns fail in all cases */
    ADI_OSAL_EVENT_HANDLE hUnused = hEventGroup;

    return ADI_OSAL_FAILED;

}


/*****************************************************************************
adi_osal_EventPend

Description
    The adi_osal_EventPend function allows a thread to wait for event 
    notifications.  Event flags are obtained from the specified event group.  
    The currently executing thread is suspended until the requested event(s) 
    occur (unless timeout occurs).  An event occurs when the requested flag is 
    set.

Conditions
    If a timeout value of ADI_OSAL_TIMEOUT_NONE is used, this function will 
    immediately return the flags available at the time of the request, 
    regardless of success.  
    If a timeout value of ADI_OSAL_TIMEOUT_FOREVER is used, the calling thread 
    will suspend until the requested flags are set.
    The flags returned in *pnReceivedEvents will only contain flags that were 
    requested in nRequestedEvents.
    The state of the event flags in the specified group is not altered by this 
    request; it does not consume the flags (i.e. it does not clear them).

Arguments
    hEventGroup         [In]    
        The handle of the event group to retrieve flags from.
    nRequestedEvents    [In]    
        Indicates which event flags are required.  This is a bit-mapped value. 
        A bit set indicates that the corresponding event bit is being requested
        . This can be considered to be a mask for event selection (refer 
        to Section 4.3.1.11).
    eGetOption  [In]    
        Specifies the options for checking event bits.  This is further 
        explained in Section 4.3.3.2. 
    nTimeoutInTicks [In]    
        Timeout for the event flag (in system ticks).  This can take any of 
        the values specified in Section 4.3.1.10 
    pnReceivedEvents    [Out]   
        Pointer to the location where the retrieved event flags are placed 
        when the thread resumes. In the case of an error or timeout, this 
        location will contain all zeros. Refer to Section 4.3.1.11 for more 
        details.

Return Values
ADI_OSAL_SUCCESS        
    The event flags were retrieved successfully.  Note that this does not 
    indicate the condition of the event flags  the user must separately test 
    the retrieved event flags, especially in cases where more than one event 
    bit was requested, depending on the settings of eGetOption.
ADI_OSAL_FAILED     
    An error occurred while retrieving event flags. The value returned in *
    pnReceivedEvents is invalid.
ADI_OSAL_TIMEOUT        
    Failed to retrieve the specified event flag(s) due to timeout.
ADI_OSAL_CALLER_ERROR   [D]     
    This occurs if an attempt is made to call this function from an invalid 
    location (i.e. from within an ISR or during startup)
ADI_OSAL_BAD_HANDLE [D] 
    The handle specified by hEventGroup is invalid.
ADI_OSAL_BAD_EVENT  [D] 
    Indicates that an invalid event flag has been specified in nRequestedEvents
    .  Refer to Section 4.3.1.11 for more details.
ADI_OSAL_BAD_TIME   [D] 
    The value specified for the timeout (nTimeoutInTicks) is invalid for the 
    underlying OS.  Please refer to Section 4.3.1.10 for details of timeout 
    ranges for maximum compatibility.
ADI_OSAL_BAD_OPTION [D] 
    The event checking option specified by eGetOption is invalid.

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_EventPend(ADI_OSAL_EVENT_HANDLE const hEventGroup,
                   ADI_OSAL_EVENT_FLAGS        nRequestedEvents, 
                   ADI_OSAL_EVENT_FLAG_OPTION  eGetOption, 
                   ADI_OSAL_TICKS              nTimeoutInTicks, 
                   ADI_OSAL_EVENT_FLAGS        *pnReceivedEvents)
{
    /* Events are not currently supported on the VDK port.  This is a stub
     * that returns fail in all cases */

    /* Use the passed-in pointer to avoid MISRA rule 16.7 */
    ADI_OSAL_EVENT_FLAGS *pUnusedEvents = pnReceivedEvents;
    ADI_OSAL_EVENT_HANDLE hUnused = hEventGroup;
    
    return ADI_OSAL_FAILED;
}

/*****************************************************************************
adi_osal_EventSet

Description
    The adi_osal_EventSet function can be used to set event flags within the 
    specified event group.  The specified event flags are ORed with the events 
    in the group.
    ISR SAFE

Arguments
    hEventGroup         [In]    
        The handle of the event group to set flags in.
    nEventFlags        [In] 
        Indicates which event flags are to be set.  This is a bit-mapped value
        . A bit set indicates that the corresponding event bit is to be set (
        refer to Section 4.3.1.11). 

Return Values
ADI_OSAL_SUCCESS        
    The event flags were set successfully.  
ADI_OSAL_FAILED     
    An error occurred while setting event flags. 
ADI_OSAL_BAD_EVENT  [D] 
    Indicates that an invalid event flag has been specified in nEventFlags.  Refer to Section 4.3.1.11 for more details.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified event group handle is invalid. 

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_EventSet(ADI_OSAL_EVENT_HANDLE const hEventGroup,
                   ADI_OSAL_EVENT_FLAGS    nEventFlags)
{
    /* Events are not currently supported on the VDK port.  This is a stub
     * that returns fail in all cases */
    ADI_OSAL_EVENT_HANDLE hUnused = hEventGroup;

    return ADI_OSAL_FAILED;
}


/*****************************************************************************
adi_osal_EventClear

Description
    The adi_osal_EventClear function can be used to clear event flags from the 
    specified event group.  This is the equivalent of adi_osal_EventSet, 
    except that it clears the specified flags instead of setting them.
    ISR SAFE

Arguments
    hEventGroup  [In]   
        The handle of the event group to clear flags from.
    nEventFlags     [In]    
        Indicates which event flags are to be cleared.  This is a bit-mapped 
        value. A bit set indicates that the corresponding event bit is to be 
        cleared (refer to Section 4.3.1.11). 

Return Values
ADI_OSAL_SUCCESS        
    The event flags were cleared successfully.  
ADI_OSAL_FAILED     
    An error occurred while clearing event flags. 
ADI_OSAL_BAD_EVENT  [D] 
    Indicates that an invalid event flag has been specified in nEventFlags.  
    Refer to Section 4.3.1.11 for more details.
ADI_OSAL_BAD_HANDLE [D] 
    If the specified event group handle is invalid. 

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_EventClear(ADI_OSAL_EVENT_HANDLE const hEventGroup,
                    ADI_OSAL_EVENT_FLAGS    nEventFlags)
{
    /* Events are not currently supported on the VDK port.  This is a stub
     * that returns fail in all cases */
    ADI_OSAL_EVENT_HANDLE hUnused = hEventGroup;

    return ADI_OSAL_FAILED;
}

