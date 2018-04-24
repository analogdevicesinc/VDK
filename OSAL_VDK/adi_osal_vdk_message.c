/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL message queue APIs.
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

/* Required to check for UINT_MAX */
#include <limits.h>

/* Required for the definition of NULL */
#include <stddef.h>

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Fifos")
#pragma file_attr("Fifos")

/* File misra suppressions */
#pragma diag(push) 
#pragma diag(suppress:misra_rule_11_3 : "Allow integer to pointer casting for ADI_OSAL_INVALID_MESSAGE token")
#pragma diag(suppress:misra_rule_8_7 : "kNoTimeoutError and kDoNotWait are currently variables, and will be changed to #defines in an updated VDK")
#pragma diag(suppress:misra_rule_16_7 : "Message Queue cleanup function does not modify the addressed object of a pointer, but needs to follow the required function prototype required by VDK")
/* FIXME: Raise this issue with the OSAL team */
#pragma diag(suppress:misra_rule_17_5 : "adi_osal_MsgQueuePend requires three levels of indirection")


/* Prototype for static Message Queue cleanup function*/
static void osalMsgQDummyFree(void *pMsgQArray);

/*****************************************************************************
osalMsgQDummyFree

Description
    VDK requires a cleanup function if an array is supplied for the Message 
    Queue.  OSAL does not take this information, so this function is used to 
    satisfy VDK's requirements. Function does nothing, and requires the user 
    to manage any allocated memory.

Arguments
    pMsgQArray [In] Area of memory - Untouched by this function.
    
Return Values
    None.    
*****************************************************************************/

static void osalMsgQDummyFree(void *pMsgQArray)
{
    /** 
     * This does not free the supplied array memory, that will be the 
     * responsibility of the user.
     */
}


/*****************************************************************************
adi_osal_MsgQueueCreate

Description
    The adi_osal_MsgQueueCreate function creates a message queue that is 
    typically used for inter-thread communications.  Messages passed between 
    threads are maintained in a message queue.
    The message maintained in the queue is actually a reference to the actual 
    message to be passed (i.e. the reference to the message is actually sent 
    between threads, not the message contents), thus the size of a message is 
    always 4 bytes (a pointer).  

Conditions
    In the event of failure, the value returned in *phMsgQ will be 
    ADI_OSAL_INVALID_QUEUE.

Arguments
   phMsgQ     [Out]    
        Pointer to the location to write the returned message queue handle.
    aMsgQ     [In]    
        The buffer to be used to store the messages (i.e. the queue).
    nMaxMsgs     [In]    
        The maximum number of messages the queue indicated by aMsgQ can hold.

Return Values
ADI_OSAL_SUCCESS         
        The message queue was created successfully.  
ADI_OSAL_FAILED        
        An error occurred while creating the message queue
ADI_OSAL_MEM_ALLOC_FAILED         
        There was insufficient memory to allocate the space for this queue.
ADI_OSAL_BAD_COUNT    [D]    
        The maximum number of messages indicated (in nMaxMsgs) exceeds the capabilities of the OS.
ADI_OSAL_BAD_HANDLE    [D]     
        If invalid message queue pointer 
ADI_OSAL_CALLER_ERROR     [D]    
        If this function is invoked from an invalid location (i.e. an ISR). 
*****************************************************************************/


ADI_OSAL_STATUS 
adi_osal_MsgQueueCreate(ADI_OSAL_QUEUE_HANDLE    *phMsgQ, 
                         void    *aMsgQ[], 
                         uint32_t         nMaxMsgs)
{
    ADI_OSAL_STATUS result;
    VDK_FifoID vdkFifoID;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if(InISR())
    {
        /* There are no errors for passing a NULL pointer for this API */
        *phMsgQ = ADI_OSAL_INVALID_QUEUE;
        return ADI_OSAL_CALLER_ERROR;
    }

    /* The only check we can make for MsgQ validity is for the pointer to be
     * NULL. */ 
    if((int)NULL == (int)aMsgQ)
    {
        *phMsgQ = ADI_OSAL_INVALID_QUEUE;
        return ADI_OSAL_BAD_HANDLE;
    }   

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/
  
#endif /*OSAL_DEBUG*/  

    
    vdkFifoID = VDK_CreateFifo(nMaxMsgs, aMsgQ, osalMsgQDummyFree);

    if((UINT_MAX) != (unsigned int)vdkFifoID)
    {
        /* Successful creation of the VDK Fifo */
        *phMsgQ = (ADI_OSAL_QUEUE_HANDLE)vdkFifoID;
        result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* VDK was unable to create the Fifo - we cannot be sure why */
        *phMsgQ = ADI_OSAL_INVALID_QUEUE;
        result = ADI_OSAL_FAILED;
    }

    return result;
}



/*****************************************************************************
adi_osal_MsgQueueDestroy

Description
    The adi_osal_MsgQueueDestroy function deletes the specified message queue.  

Conditions
    This function must be used with care since multiple threads could attempt 
    to access a possibly deleted queue.  Generally, the user must first delete 
    all threads that can access the queue before deleting the queue.
    Once the message queue is successfully deleted, the caller should set the 
    handle to ADI_OSAL_INVALID_QUEUE for error checking purposes.  The handle 
    is not modified by this call, and thus could be erroneously used for 
    subsequent calls.
    If an attempt is made to delete a message queue when a thread is pending 
    on it, the function will return an ADI_OSAL_THREAD_PENDING result. 
    Although some Operating Systems will delete the message queue and unblock 
    all pending threads, this behaviour is not supported in the OSAL to ensure 
    portability.

Arguments
    hMsgQ            [In]    
        The handle of the message queue to be destroyed.  This is the value 
        returned by adi_osal_MsgQueueCreate when the queue is created.

Return Values
ADI_OSAL_SUCCESS         
        The message queue was deleted successfully.  
ADI_OSAL_FAILED        
        An error occurred while removing the message queue
ADI_OSAL_THREAD_PENDING        
        A thread is pending on the specified message queue.
ADI_OSAL_BAD_HANDLE    [D]    
        If the specified message queue handle is invalid. 
ADI_OSAL_CALLER_ERROR     [D]    
        If this function is invoked from an invalid location (i.e. an ISR or 
        during startup). 

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_MsgQueueDestroy(const ADI_OSAL_QUEUE_HANDLE hMsgQ)
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

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/

    /* Calls to DestroyFifo() requires the use of GetLastThreadError() to
       establish if any failures occurred.  Any previous thread errors have to
       be cleared before we make the DestroyMutex() call. */
       
    /* For threads created using OSAL, or for threads that ignore this error 
     * in their Error() functions, the error will be logged internally by VDK
     * and execution will return here.
     * Note that for Non Error Checking builds of VDK the error is not logged
     * and this call appears successful.
     */
    VDK_ClearThreadError();

    VDK_DestroyFifo((VDK_FifoID)hMsgQ);

    /* Check for any problems during destruction. */
    switch(VDK_GetLastThreadError())
    {
        case VDK_kNoError:
            /* Everything is ok */
            break;
    
        case VDK_kFifoDestructionFailure:
            /* This failure indicates that the FifoID is valid, but there is
            * at least one thread pending on the fifo. */  
            result = ADI_OSAL_THREAD_PENDING;
            break;
        
#if defined(OSAL_DEBUG)
        case VDK_kInvalidFifoID:
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
adi_osal_MsgQueuePend

Description
    The adi_osal_MsgQueuePend function allows a thread to wait for a message 
    to be queued in the specified queue.  This function will then retrieve the 
    message reference, removing it from the queue.

Conditions
    If an error or timeout occurs the location indicated by *ppMsg will 
    contain NULL.
    If a timeout value of ADI_OSAL_TIMEOUT_NONE is used, this function will 
    immediately return.  If the message queue was empty at the time, this will 
    return ADI_OSAL_QUEUE_EMPTY, and *ppMsg will contain NULL, otherwise the 
    first message in the queue will be returned.
    If a timeout value of ADI_OSAL_TIMEOUT_FOREVER is used, the calling thread 
    will suspend until a message becomes available.

Arguments
    hMsgQ            [In]    
        The handle of the message queue to monitor for a message.
    ppMsg            [Out]        
        A pointer to the location to write the retrieved message reference.
    nTimeoutInTicks    [In]    
        The timeout, in system ticks, for retrieving the message reference. 
        This can take any of the values specified in Section 4.3.1.10). 

Return Values
ADI_OSAL_SUCCESS         
    A message reference was retrieved successfully.  
ADI_OSAL_FAILED        
    An error occurred while retrieving the message reference
ADI_OSAL_TIMEOUT        
    Message request timed out.
ADI_OSAL_QUEUE_EMPTY        
    The specified message queue was empty when the request was made (only 
    occurs if timeout is ADI_OSAL_TIMEOUT_NONE).
ADI_OSAL_BAD_HANDLE    [D]    
    If the specified message queue handle is invalid. 
ADI_OSAL_BAD_TIME    [D]    
    The value specified for the timeout (nTimeoutInTicks) is invalid for the 
    underlying OS.  Please refer to Section 4.3.1.10 for details of timeout 
    ranges for maximum compatibility.
ADI_OSAL_CALLER_ERROR    [D]    
    If this function is invoked from an invalid location (i.e. an ISR or 
    during startup).
*****************************************************************************/

ADI_OSAL_STATUS 
adi_osal_MsgQueuePend(ADI_OSAL_QUEUE_HANDLE const hMsgQ, 
                      void **ppMsg, 
                      ADI_OSAL_TICKS nTimeoutInTicks)
{
    ADI_OSAL_STATUS result;
    bool bFifoAcquired = false;
    uint32_t nVdkTimeout;            /* VDK_Ticks, using uint32_t for misra complience */
    void *pData = NULL;  /* Data Pointer used for the PendFifo call */  

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    if((!VDK_IsRunning()) || (InISR()))
    {
        *ppMsg = NULL;
        return ADI_OSAL_CALLER_ERROR;
    }

    /* Is the timeout within VDK limits */
    if( (VDK_MAX_TIMEOUT < nTimeoutInTicks) &&
        (ADI_OSAL_TIMEOUT_NONE != nTimeoutInTicks) &&
        (ADI_OSAL_TIMEOUT_FOREVER != nTimeoutInTicks) )
    {
        *ppMsg = NULL;
        return ADI_OSAL_BAD_TIME;
    }

    /* Holding a lock when pending with a timeout other than 
       ADI_OSAL_TIMEOUT_NONE is a dangerous thing to do. If a lock is
       held we return an error.  */
    if( (VDK_IsInRegion()) && (ADI_OSAL_TIMEOUT_NONE != nTimeoutInTicks) )
    {
        *ppMsg = NULL;
        return ADI_OSAL_FAILED; 
    }

#pragma diag(pop) /*OSAL_DEBUG misra suppressions*/

#endif /*OSAL_DEBUG*/

    /* Clear any thread error, as we need to check them after the pend. */
    VDK_ClearThreadError();
    
    if(ADI_OSAL_TIMEOUT_NONE == nTimeoutInTicks)
    {
        /* Don't wait */
        nVdkTimeout = VDK_kDoNotWait;
    }
    else
    {
        /* Block if the fifo is unavailable */
        /* Timeout will either be FOREVER or a given number of ticks.
           For a given number of Ticks, we do not want to go to the
           thread error function if there was a timeout. */
        nVdkTimeout = (ADI_OSAL_TIMEOUT_FOREVER == nTimeoutInTicks)? 0u : (nTimeoutInTicks | VDK_kNoTimeoutError);
    }
        
    /* Try to pend on the Fifo */
    bFifoAcquired = VDK_PendFifo((VDK_FifoID)hMsgQ, &pData, nVdkTimeout);

    if(bFifoAcquired)
    {
        /* Successful pend */
        *ppMsg = pData;
        result = ADI_OSAL_SUCCESS;
    }
    else
    {
        /* A failure occured */
   
        if(nVdkTimeout == VDK_kDoNotWait)
        {
            result =  ADI_OSAL_QUEUE_EMPTY;
        }
        else
        {
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
                case VDK_kInvalidFifoID:
#if defined(OSAL_DEBUG)
                    result = ADI_OSAL_BAD_HANDLE;
                    break;
#endif
                    /* Fall through for non OSAL_DEBUG */
                case VDK_kDbgPossibleBlockInRegion:   
                case VDK_kBlockInInvalidRegion:
                case VDK_kInvalidTimeout:
                    /* These errors should be checked for OSAL_DEBUG above,
                     * for the release case they are just failures. */
                    result = ADI_OSAL_FAILED;
                    break;
                default:
                    /* If there are no other thread errors, and the Pend failed
                    then a timeout must have occurred. */
                    result = ADI_OSAL_TIMEOUT;
                    break;
            }  
        }

        /* Null the MsgPtr */
        *ppMsg = NULL;
    }

    return result;
}


/*****************************************************************************
adi_osal_MsgQueuePost

Description
    The adi_osal_MsgQueuePost function allows a thread to send a message to 
    the specified queue.  What the actual message is (and its structure) is 
    application dependent, and the OSAL will actually send the pointer to the 
    message to the recipient (the OSAL will send a reference to the message).

Conditions
ISR SAFE

Arguments
    hMsgQ            [In]    
        The handle of the message queue to send the message to.
    pMsg    [In]    
        A pointer to the message to send.  This is the value actually sent to 
        the queue.
Return Values
ADI_OSAL_SUCCESS         
    A message reference was sent successfully.  
ADI_OSAL_FAILED        
    An error occurred while sending the message reference
ADI_OSAL_QUEUE_FULL        
    The specified message queue is full.
ADI_OSAL_BAD_HANDLE    [D]    
    If the specified message queue handle is invalid. 
*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_MsgQueuePost(ADI_OSAL_QUEUE_HANDLE const hMsgQ, 
                      void *pMsg)

{
    ADI_OSAL_STATUS result = ADI_OSAL_SUCCESS;

    if(InISR())
    {
        /* All we can check for is that the post was successful or not */
        if(!VDK_C_ISR_PostFifo((VDK_FifoID)hMsgQ, pMsg))
        {
            result = ADI_OSAL_FAILED;
        }    
       
    }
    else
    {
        /* We will be looking at any errors when (if) this call returns.*/
        VDK_ClearThreadError();
    
        /* PostFifo will handle the invalid ID case */
        if(!VDK_PostFifo((VDK_FifoID)hMsgQ, pMsg, VDK_kDoNotWait))
        {

            /* This error check should reduce to ADI_OSAL_FAILED for non-debug
             *  OSAL */
            if(VDK_GetLastThreadError() == VDK_kInvalidFifoID)
            { 
#if defined(OSAL_DEBUG)
                result = ADI_OSAL_BAD_HANDLE;
#else
                result = ADI_OSAL_FAILED;
#endif
            }
            else
            {
                result = ADI_OSAL_QUEUE_FULL;
            }
        }
    }

    return result;
}

#pragma diag(pop)  /* File Misra suppressions */
