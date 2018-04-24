/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public Fifo API functions.
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "TMK.h"

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "VDK_Fifo.h"
#include "VDK_History.h"
#pragma file_attr("OS_Component=Fifos")
#pragma file_attr("Fifos")

#ifndef __ADSPTS__
// TS does not like suppressing this warning
#pragma diag(suppress:1994:" multicharacter character literal potential portability problem  ")
#endif

// Used to test if the fifo pointer is not NULL and points to a fifo.
#define FIFO_TEST(pFifo) ((pFifo != NULL) && (pFifo->ID() == FIFO_SIGNATURE))

// Fifo IDs are really just Fifo pointers that have been cast into 
// an integer/enum form.  At least one of these is used in every 
// Fifo API.
#define FIFO_TO_FIFOID(pFifo) ( static_cast<FifoID>(reinterpret_cast<unsigned int>(pFifo)) )
#define FIFOID_TO_FIFO(fifoID) ( reinterpret_cast<Fifo *>(static_cast<unsigned int>(fifoID)) )

// Used for changing the thread status, changing from kReady to 
// either blocked, or blocked with timeout.
#define FIFO_POST_BLOCKED(timeout) ( (0 == (timeout))? kFifoPostBlocked : kFifoPostBlockedWithTimeout )
#define FIFO_PEND_BLOCKED(timeout) ( (0 == (timeout))? kFifoPendBlocked : kFifoPendBlockedWithTimeout )


namespace VDK
{

    extern int  g_remainingTicks;
    extern int  g_SchedulerRegionCount;

    // s_lock fifo class member is used throughout the fifo code to
    // disable interrupts for ISR-sensitive fifo operations.
    //
    // The fifo functions may log history while holding the fifo lock,
    // so the fifo lock must be of lower precedence than the history buffer
    // lock (which currently has precedence TMK_MAX_LOCK_PRECEDENCE == 255).
    //
    TMK_Lock Fifo::s_lock = TMK_INIT_GLOBAL_INTERRUPT_LOCK(TMK_MAX_LOCK_PRECEDENCE - 1);

    // If the value of the heap is set to system_heap in the kernel tab
    // VdkGen will not create the variable and we will use this weak (which is
    // set to 0 (system_heap). This is an optimisation because we don't know
    // if Fifos are used or not and we don't want to have the variable in
    // VDK.cpp all the time

    #pragma weak_entry
    HeapID g_FifoObjectHeap;

    HeapID g_FifoBufferHeap = g_FifoObjectHeap;

/****************************************************************
 * Fifo::Fifo
 *
 * Internal Fifo constructor.
 * 
 * m_BlockingDirection does not need to be set here as it 
 * set prior to adding a thread to the blocked queue.
 ****************************************************************/
    Fifo::Fifo( const FifoID    inEnumValue,  
                unsigned int    inSize, 
                void            *pInBufferLocation,
                void            (*pfInBufferCleanup)(void *)) 
                        : m_BufferSize(inSize),
                          m_pfBufferCleanup(pfInBufferCleanup)
    {

        TMK_InitDpcElement(this, Invoke);
        TMK_InitThreadList(&m_BlockedThreads);
        TMK_InitThreadList(&m_ISRUnblockedThreads);

        // Fifo data pointers are added to the buffer in increasing array elements.
        // The first available slot for a created fifo will be 0.
        m_NumMsgsInBuffer = 0;
        m_HeadIndex = 0;  
        m_FreeIndex = 0;  
        m_pBuffer = static_cast<void**>(pInBufferLocation);
        
        ID() = FIFO_SIGNATURE;
    }


/****************************************************************
 * Fifo::Invoke
 *
 * We come in here expecting that there are threads to unblock
 * and place in the ReadyQ.  The data will already be in 
 * the thread structure, so this part deals only with the 
 * thread unblock.  Note that we only touch the 
 * m_ISRUnblockedThreads member here, we do nothing with the
 * rest of the fifo structure 
 ****************************************************************/
    void 
    Fifo::Invoke(TMK_DpcElement* inDpcQE)
    {
#pragma suppress_null_check
        Fifo *pFifo = static_cast<Fifo *>(inDpcQE);

        // We need the interrupt lock for periods where
        // we access the unblocked ThreadList as this can
        // be modified at ISR level.
        TMK_AcquireMasterKernelLockFromKernelLevel();
        TMK_LockState state = TMK_AcquireGlobalInterruptLock(&s_lock);

        // If there are no threads here then we fall straight through.
        // This could happen if an ISR posts data midway through 
        // the loop below.
        while(!TMK_IsThreadListEmpty(&pFifo->m_ISRUnblockedThreads))
        {
            // There is a thread waiting to be unblocked - remove from list     
#pragma suppress_null_check
            Thread 
            *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pFifo->m_ISRUnblockedThreads));

            // We only need to disable interrupts for the ThreadList
            // access, so we can release the lock here.
            // Note:
            // Any ISR posts can come in and free more blocked threads
            // pending on data.  If they do, the unblocked threads
            // are added to m_ISRUnblockedThreads and we'll add them
            // to the readyQ next time round the loop.
            TMK_ReleaseGlobalInterruptLock(&s_lock, state);
                     
            // Remove the thread from the TimeQueue (if necessary) 
            if (pThread->runStatus == kFifoPendBlockedWithTimeout)
            {
                // The master kernel lock is still being held
                TMK_CancelTimeout(pThread); 
            }

            HISTORY_(kThreadStatusChange, pThread->runStatus, pThread->ID());

            // Put the thread in the ready queue 
            TMK_MakeThreadReady(pThread);

            // Disable interrupts before touching the ThreadList.
            TMK_LockState state = TMK_AcquireGlobalInterruptLock(&s_lock);
        }

        TMK_ReleaseGlobalInterruptLock(&s_lock, state);
        TMK_ReleaseMasterKernelLockFromKernelLevel();
    }

/****************************************************************
 * Fifo::BufferCleanup()
 *
 * This is used to cleanup the Fifo buffer if the user has
 * let VDK allocate it.  Is only used from DestroyFifo().
 ***************************************************************/
    void 
    Fifo::BufferCleanup(void *pInBuffer)
    {
        // This is an internal function, so pInBuffer should 
        // never be NULL.  Adding a check just in case. 
        if( NULL != pInBuffer )
        {
            heap_free(g_FifoBufferHeap, pInBuffer);
        }
    }

/****************************************************************
 * CreateFifo
 *
 * Public API to create a new Fifo.
 *
 * This API doesn't need to be ISR safe as we're not 
 * manipulating global ID tables.  Currently just allocates
 * the memory for fifo and (optionally) the buffer from the
 * system heap with malloc calls.
 ****************************************************************/
    FifoID
    CreateFifo(unsigned int inSize, void *pInBuffer, void (*pfInBufferCleanup)(void*))
    {

        FifoID fifoID = static_cast <FifoID> (UINT_MAX);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
        if (IS_ISR_LEVEL ())
        {
            KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());    
        }

        // We do not need to hold any scheduling locks as we do not have
        // to deal with global ID tables. This may change for Andromeda.
        if( 0 == inSize )
        {
            if( (NULL != pInBuffer) || (NULL != pfInBufferCleanup) )
            {
                // Error, both args should be NULL for an unbuffered
                // fifo.
                DispatchThreadError(kIncompatibleArguments, 0);
                return fifoID;
            }
        }
        else
        {
            // We need to allocate a buffer, so make sure the args
            // are consistent.
            if( ((NULL == pInBuffer) && (NULL != pfInBufferCleanup)) ||
                ((NULL != pInBuffer) && (NULL == pfInBufferCleanup)) )
            {
                // Error, both args should either be NULL or both
                // should be populated.
                DispatchThreadError(kIncompatibleArguments, 0);
                return fifoID;
            }
        }
#endif

        // Create the Fifo
        Fifo
        *pMemory = static_cast<Fifo*>(heap_malloc(g_FifoObjectHeap, sizeof(Fifo)));

        // Check that we've been able to allocate the space for the Fifo
        if (NULL != pMemory)
        {
            // Don't do anything until we have made sure that
            // we have a valid buffer, either it's been created
            // or we need to create it.
            if( (0 != inSize) && (NULL == pInBuffer) )
            {
                // We need to create it
                pInBuffer = heap_malloc(g_FifoBufferHeap, inSize*sizeof(void*));
                pfInBufferCleanup = Fifo::BufferCleanup;
            }

            // Check to see if there was a buffer allocation failure.
            if( (NULL == pInBuffer) && (0 != inSize) )
            {
                // We cannot allocate the buffer, so we need to 
                // deallocate the Fifo object and return an error
                heap_free(g_FifoObjectHeap, pMemory);
            }
            else
            {
                // Successful allocation of the Fifo object and the buffer
                // (if the buffer was required).

                // Use "placement new" to initialise the raw memory
                Fifo *pFifo = new (pMemory) Fifo(fifoID,  inSize, pInBuffer, pfInBufferCleanup);
               
                // Convert the fifo pointer to an integral ID 
                fifoID = FIFO_TO_FIFOID(pFifo);

                HISTORY_(kFifoCreated, fifoID, GetThreadID());
            }
        }

        // If we managed to create the Fifo correctly, return the 
        // FifoID, otherwise dispatch an error.

#if (VDK_INSTRUMENTATION_LEVEL_>0)
        if( UINT_MAX == fifoID )
        {
            DispatchThreadError(kFifoCreationFailure, fifoID);
        }
#endif

        return fifoID; // if there is an error this will be UINT_MAX

    }


/****************************************************************
 * DestroyFifo
 *
 * Destroy a fifo, and free the buffer it if has been 
 * allocated by VDK.
 *
 * An error will be dispatched if there are any threads blocked 
 * on the Fifo (pend or post).  Data remaining in the buffer 
 * will not cause an error.
 ****************************************************************/
void DestroyFifo( FifoID inFifoID )
{

    Fifo *pFifo = FIFOID_TO_FIFO(inFifoID);
    SystemError errorCode =kNoError;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (IS_ISR_LEVEL ())
    {
        KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
    }
#endif

    TMK_LockState state = TMK_AcquireGlobalInterruptLock(&Fifo::s_lock);

    if ERRCHK( FIFO_TEST(pFifo) ) 
    {
        if (TMK_IsThreadListEmpty(&pFifo->m_BlockedThreads)) 
        {
            // We're assigning a value here before the deallocation as we
            // need to remove the FIFO_SIGNATURE signature from memory.  
            // Without this a user could still successfully use the old
            // ID without any error. Also, now any access from Post or
            // pend fifo will now fail (for fully instrumented/ error
            // checking builds).
            pFifo->ID() = DEAD_FIFO_SIGNATURE;
            TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state);
            
            // Check if we need to cleanup the buffer
            if( 0 < pFifo->m_BufferSize )
            {
                // Dereference the buffer cleanup function.  Note that
                // the cleanup function cannot be NULL as we've done all 
                // the checking during Fifo creation.  
                (*pFifo->m_pfBufferCleanup)(pFifo->m_pBuffer);
            }

            // Call the fifo destructor...
            pFifo->~Fifo();

            //.... and then free up the memory.
            heap_free (g_FifoObjectHeap, pFifo);

            HISTORY_(kFifoDestroyed, inFifoID, GetThreadID());
        }
       else 
       {
            // There are threads waiting on this Fifo - error.
            TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state);
            errorCode = kFifoDestructionFailure;
       }
    }
    else 
    {
        // Not a valid FifoID.
        TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state);
        errorCode = kInvalidFifoID;
    }

#if (VDK_INSTRUMENTATION_LEVEL_>0)
   if (errorCode != kNoError)
   {
        DispatchThreadError(errorCode, inFifoID);
   }
#endif

}


/****************************************************************
 * C_ISR_PostFifo
 *
 * Post a data word to a Fifo from ISR level.
 *
 * This should only be called from ISR level.  
 *
 * A PostFifoISR call can do two things 
 *      - add data to the fifo buffer.
 *      - unblock a pending thread.
 *
 * Note:
 *   There is no error for the case where the buffer is
 * full, just a failure return.
 *
 *  Any threads that are pending on the Fifo will be
 * populated with the posted data without the buffer being 
 * involved.  E.g. If 10 threads are pending on a fifo that can
 * hold 2 words of data (say), and an ISR posts 10 times, then all
 * posts will pass straight to the intended threads and all 
 * posts will be successful.
 *
 * No errors are dispatched if this API is called from a non-ISR
 * level (thread or kernel).
 ****************************************************************/
    bool C_ISR_PostFifo(FifoID inFifoID,
                        void   *pInData )

    {
        bool result = true; // The return value from this call.

        Fifo *pFifo = FIFOID_TO_FIFO(inFifoID); 

        // Fifos can only be destroyed at thread level, the
        // following instruction is safe without an interrupt
        // lock.
        if ERRCHK( FIFO_TEST(pFifo) )  
        {
            HISTORY_(kFifoPosted, inFifoID, static_cast<ThreadID>(VDK_INTERRUPT_LEVEL_));

            TMK_LockState 
            state = TMK_AcquireGlobalInterruptLock(&Fifo::s_lock);

            // We need to check for the case where the buffer
            // is empty and there are threads waiting to receive.
            // We should also check that these threads aren't waiting 
            // to send to an unbuffered fifo.
            if( pFifo->hasPendingThreads() )
            {
                // Extract the thread from the blocked list
#pragma suppress_null_check
                Thread
                *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pFifo->m_BlockedThreads));
                
                // Insert the data
                pThread->m_APIData = pInData;

                // We need to add to the unblocked queue for this
                // fifo.  Ordering is not important.
                TMK_AppendThreadToList(&pFifo->m_ISRUnblockedThreads, pThread);

                // Now add the fifo to the dpc queue for unblocking
                TMK_InsertQueueDpc(pFifo);
                TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 
                
            } 
            else if( !pFifo->isFull() )
            {
                // There's a spot in the buffer to inject the 
                // data.
                pFifo->Add(pInData);

                TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state);
            }
            else
            {
                TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state);
                // The Fifo is full, 
                result = false;
            }
        } 
        else 
        {
            // invalid FifoID 
            result = false;   
        }

        return result;
    }


/****************************************************************
 * PostFifo
 *
 * Post a data word to a Fifo.
 *
 * This can be called from Kernel and thread levels.  There 
 * is also a block-on-send option for this API.
 *
 *
 *   A post can do three things - add data to the fifo buffer.
 *                              - unblock a pending thread.
 *                              - block the current thread.
 *
 * Note:
 *   There is no error for the case where the buffer is
 *   full or for a timeout, just a failure return.
 *   Kernel-level posts will require the timeout to be
 *   set to kDoNotWait, otherwise an error will be 
 *   dispatched.
 *
 * Any threads that are pending on the Fifo will be
 * populated with the posted data without the buffer being 
 * involved.  E.g. If 10 threads are pending on a fifo that can
 * hold 2 words of data (say), and an ISR posts 10 times, then all
 * posts will pass straight to the intended threads and all 
 * posts will be successful.
 ****************************************************************/
    bool PostFifo(  FifoID inFifoID,
                    void   *pInData,
                    Ticks  inTimeout )
    {
       
        bool result = true; // The return value from this call.

        Fifo *pFifo = FIFOID_TO_FIFO(inFifoID); 
   

#if (VDK_INSTRUMENTATION_LEVEL_>0)

        if( IS_USER_ISR_LEVEL() )
        {
            // This API should only be called from Kernel or
            // Thread level.
           KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
        }

        // Check that the timeout is within range
        if (INT_MAX == inTimeout || (Ticks) INT_MIN == inTimeout)
        {
            // We had an out-of-range timeout passed in
            DispatchThreadError(kInvalidTimeout, inTimeout);
            return false;
        }

        // If there is a scheduling lock and a blockable
        // timeout then we dispatch an error.  If we are at 
        // kernel level then the timeout should be kDoNotWait.
        if( ( TMK_AtKernelLevel() || TMK_IsSchedulingSuspended())
            && (kDoNotWait != inTimeout) )
        {
            // We are going to perform a Post, and we may block.
            // This is a possible error state, so throw an error to the current thread
            DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
        }
#endif

        TMK_AcquireMasterKernelLock(); 

        // Fifos can only be destroyed at thread level, the
        // following instruction is safe without an interrupt
        // lock.
        if ERRCHK( FIFO_TEST(pFifo) )  
        {
            HISTORY_(kFifoPosted, inFifoID, GetThreadID());

            TMK_LockState 
            state = TMK_AcquireGlobalInterruptLock(&Fifo::s_lock);
            
            // We need to check for the case where the buffer
            // is empty and there are threads waiting to receive.
            // This test is valid for buffered and unbuffered fifos.
            if( pFifo->hasPendingThreads() )
            {
                // Extract the thread from the blocked list
#pragma suppress_null_check
                Thread
                *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pFifo->m_BlockedThreads));

                // MasterKernelLock still held
                TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 

                // Insert the data
                pThread->m_APIData = pInData;

                // Remove from the time queue
                if (pThread->runStatus == kFifoPendBlockedWithTimeout)
                {
                    TMK_CancelTimeout(pThread);  // safe wrt. RescheduleISR
                }

                // Log the status change for the unblocked thread.
                HISTORY_(kThreadStatusChange, pThread->runStatus, pThread->ID());

                // Put the thread in the ready queue
                TMK_MakeThreadReady(pThread);
                TMK_Reschedule();

                // We have just had a successful post.  The result is already 
                // 'true' so we just release the MasterKernelLock and return
                TMK_ReleaseMasterKernelLock();
            } 
            else if( !pFifo->isFull() )
            {
                // There's a spot in the buffer so we can pass-in the 
                // data.
                pFifo->Add(pInData);
                TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&Fifo::s_lock, state); 
            }
            else
            {
                // The Fifo is full, either block at thread level or
                // return a fail.  Do we need to block?
                if( kDoNotWait != inTimeout )
                {
                    // We're going to block.

                    // Threads can only be added to a queue from thread
                    // level (pend or post), so we can release the 
                    // interrupt lock here.  We're still safe from ISR posts
                    // as we're setting the direction before we add the thread
                    // to the list - ISR posts don't do anything to the queue
                    // if the queue is empty, or if the queue isn't empty 
                    // and the direction is POST.  By setting the direction
                    // to POST before inserting, both conditions will be covered 
                    // regardless of when the PostFifoISR comes in during the 
                    // insertion process.
                    //
                    // The master kernel lock is still held.  
                    TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 

#if (VDK_INSTRUMENTATION_LEVEL_>0)
                    // If we are blocking we need to make sure a schedule will 
                    // occur afterwards. If scheduling is off then bad things
                    // will happen.
                    if (TMK_IsSchedulingSuspended())
                    {
                        TMK_ReleaseMasterKernelLock();
                        DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
                        return false;
                    }
#endif

                    // While we wait to post, we copy the fifo data into the
                    // thread's structure.  This will be queried after the buffer
                    // has a space available.
#pragma suppress_null_check 
                    Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());
                    pThread->m_APIData = pInData;

                    // Log previous status
                    HISTORY_(kThreadStatusChange, pThread->runStatus, pThread->ID());

                    TMK_MakeThreadNotReady(pThread, FIFO_POST_BLOCKED(inTimeout));

                    // Insert thread into the blocked queue. There is no priority
                    // ordering for threads waiting to send, as the FIFO
                    // behaviour must be maintained.
                    // These instructions MUST be in this order. 
                    // update m_BlockingDirection AND THEN add to
                    // list. 
                    pFifo->m_BlockingDirection = kBlockedOnFifoPost;
                    TMK_AppendThreadToList(&pFifo->m_BlockedThreads, pThread);

                    // The global interrupt lock has been released, but
                    // the master kernel lock is still held. 
                    pThread->BlockedOnIndex() = inFifoID;

                    // Register for timeout, if required. Use TMK_ResetTimeout() instead of
                    // TMK_SetTimeout() in case the current thread is round-robin.
                    //
                    if (inTimeout != 0) /* if not Wait Forever*/
                    {
                        g_remainingTicks = TMK_ResetTimeout(pThread, inTimeout & INT_MAX);   
                    }

                    TMK_Block();
                    
                    // We've been unblocked, either by a fifo post or by timeout
                    TMK_ReleaseMasterKernelLock();

                    // Check the error status.  This is set in Thread::Timeout()
                    if (pThread->TimeoutOccurred())
                    {
                        if ((int)inTimeout > 0)   // test sign bit
                        {
                            DispatchThreadError(kPostFifoTimeout, inTimeout);
                        }
                        pThread->TimeoutOccurred() = false;

                        result = false;
                    }
                }
                else  /* Do not wait */
                {
                    TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&Fifo::s_lock, state); 
                    result = false;
                }
            }
        } 
        else 
        {
            // invalid FifoID 
#if (VDK_INSTRUMENTATION_LEVEL_>0)

            TMK_ReleaseMasterKernelLock();
            DispatchThreadError(kInvalidFifoID, inFifoID);
#endif
            result = false;   
        }

        return result; 
    }


/****************************************************************
 * PendFifo
 *
 * Pend on a data word from a Fifo.
 *
 * This can only be called from Thread level. This API can also
 * do three things  - Remove data from the buffer.
 *                  - Block, pending on the buffer.
 *                  - Unblock a posting thread.
 *
 * Threads that are blocked on a Fifo are not stored in priority
 * order.  The threadlist behaviour is Fifo for both pends and
 * posts.
 ****************************************************************/
    bool PendFifo(    FifoID     inFifoID,
                    void    **outData,
                    Ticks      inTimeout)
    {

        bool result = true;
        Fifo *pFifo = FIFOID_TO_FIFO(inFifoID); 
        
#if (VDK_INSTRUMENTATION_LEVEL_>0)

        // This API cannot be called from ISR or Kernel level.
        if (IS_ISR_LEVEL ()) 
        {
            KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
        }

        // Check that the timeout is within range
        if (INT_MAX == inTimeout || (Ticks) INT_MIN == inTimeout)
        {
            // We had an out-of-range timeout passed in
            DispatchThreadError(kInvalidTimeout, inTimeout);
            return false;
        }
        if ( NULL == outData )
        {
            // We can't use a NULL pointer for returning data
            DispatchThreadError(kInvalidPointer, inFifoID);
            return false; 
        }

        if (TMK_IsSchedulingSuspended() && kDoNotWait != inTimeout)
        {
            // We are going to perform a Pend, and we may block.
            // This is a possible error state, so throw an error to the current thread.
            DispatchThreadError( kDbgPossibleBlockInRegion, g_SchedulerRegionCount );
        }
#endif

        TMK_AcquireMasterKernelLock(); 

        if ERRCHK( FIFO_TEST(pFifo) ) 
        {
            HISTORY_(kFifoPended, inFifoID, GetThreadID());

            TMK_LockState 
            state = TMK_AcquireGlobalInterruptLock(&Fifo::s_lock);
          
            // There are two things we need to check:
            //  Is there any data in the buffer?
            //  If not, are there any threads waiting to
            //  send data? 
            if( pFifo->isEmpty() )
            {
                // There could be a thread waiting to post data even if
                // the number of messages in the buffer is zero. 
                // i.e. an unbuffered fifo.
                if ( pFifo->hasPostingThreads() )
                {
                    // There is a thread waiting to send.
#pragma suppress_null_check
                    Thread
                    *pPostThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pFifo->m_BlockedThreads));
                    
                    // Only the threadlist needs to be interrupt-safe.
                    // MasterKernelLock is still held.
                    TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 

                    // Extract the data from the posting thread structure.
                    *outData = pPostThread->m_APIData;

                    if( kFifoPostBlockedWithTimeout == pPostThread->runStatus )
                    {
                        // safe wrt. RescheduleISR
                        TMK_CancelTimeout(pPostThread);  
                    }

                    HISTORY_(kThreadStatusChange, pPostThread->runStatus, pPostThread->ID());

                    // Put the thread in the ready queue
                    TMK_MakeThreadReady(pPostThread);
                    TMK_Reschedule();

                    // We have just had a successful pend.  The result is already 'true'
                    // so we just release the MasterKernelLock and return
                    TMK_ReleaseMasterKernelLock();
                }
                else
                {
                    // There is no data available, and no threads waiting to send.
                    // Block?
                    if( kDoNotWait == inTimeout )
                    {
                        // No data, and we've not to wait. Failed call.
                        // We have the fifo lock so release it before returning.
                        TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&Fifo::s_lock, state);
                        result = false;
                    }
                    else
                    {
                        // Wait for data

#if (VDK_INSTRUMENTATION_LEVEL_>0)
                        // If we are blocking we need to make sure a schedule 
                        // will occur afterwards. If scheduling is off then 
                        // bad things will happen.
                        if (TMK_IsSchedulingSuspended())
                        {
                            // We have the fifo lock so release it before i
                            // returning.
                            TMK_ReleaseGlobalInterruptAndMasterKernelLocks(&Fifo::s_lock, state);
                            DispatchThreadError( kBlockInInvalidRegion, g_SchedulerRegionCount );
                            return false;
                        }
#endif

#pragma suppress_null_check
                        Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

                        TMK_MakeThreadNotReady(pThread, FIFO_PEND_BLOCKED(inTimeout));

                        
                        // The threadlist is interrupt-sensitive, as ISRs can potentially
                        // remove all the threads at any time.
                        pFifo->m_BlockingDirection = kBlockedOnFifoPend; 
                        TMK_AppendThreadToList(&pFifo->m_BlockedThreads, pThread);

                        // MasterKernelLock is still masked
                        TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 

                        // We'd normally post the history event before making the thread blocked,
                        // unfortunately we have to keep the reschedule interrupt lock on
                        // for the previous instructions.  Moving this call outside
                        // of the interrupt lock area to reduce interrupt latency. 
                        HISTORY_(kThreadStatusChange, kReady, pThread->ID());

                        // Register for timeout, if required. Use TMK_ResetTimeout() instead of
                        // TMK_SetTimeout() in case the current thread is round-robin.
                        //
                        if (inTimeout != 0) /* if not Wait Forever*/
                        {
                            g_remainingTicks = TMK_ResetTimeout(pThread, inTimeout & INT_MAX);   // mask out the sign bit
                        }

                        // Block
                        pThread->BlockedOnIndex() = inFifoID;

                        TMK_Block();

                        // We've been unblocked, either by a fifo post or by timeout
                        TMK_ReleaseMasterKernelLock();

                        // Check the error status.  This is set in Thread::Timeout()
                        if (pThread->TimeoutOccurred())
                        {
                            if ((int)inTimeout > 0)   // test sign bit
                            {
                                DispatchThreadError(kPendFifoTimeout, inTimeout);
                            }
                            pThread->TimeoutOccurred() = false;

                            result = false;
                        }
                        else
                        {
                            // A successful pend. Any posted data will be written
                            // to this thread's internal structure. No need for 
                            // locking protection.
                            *outData = pThread->m_APIData;
                        }
                    }
                }
            }
            else
            {
                // There's data available - remove it.
                *outData = pFifo->Remove();

                // There may be a thread waiting to post, so now that there's a space
                // available we can check to see if we can populate the freed space
                // and unblock the waiting-to-post thread.
                // If there WAS data avilable, that means that there are no threads
                // waiting to receive - any threads that are waiting are waiting to
                // send.
                if (!TMK_IsThreadListEmpty(&pFifo->m_BlockedThreads))
                {
                    // Extract the thread from the blocked list
#pragma suppress_null_check
                    Thread
                    *pThread = static_cast<Thread*>(TMK_GetNextThreadFromList(&pFifo->m_BlockedThreads));
                    
                    // Insert the data into the buffer
                    pFifo->Add(pThread->m_APIData);

                    // Masterlock is still held.
                    TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 

                    // Remove from the time queue if required
                    if (pThread->runStatus == kFifoPendBlockedWithTimeout)
                    {
                        TMK_CancelTimeout(pThread);  // safe wrt. RescheduleISR
                    }

                    // Log the status change for the unblocked thread.
                    HISTORY_(kThreadStatusChange, pThread->runStatus, pThread->ID());

                    // Put the thread in the ready queue
                    TMK_MakeThreadReady(pThread);
                    TMK_Reschedule();
                }
                else
                {
                    // No threads waiting to post - just free the space.
                    TMK_ReleaseGlobalInterruptLock(&Fifo::s_lock, state); 
                }
                TMK_ReleaseMasterKernelLock();
            }
        }
        else
        {
            // Not a valid fifo - fail.
            TMK_ReleaseMasterKernelLock();
            result = false;
#if (VDK_INSTRUMENTATION_LEVEL_>0)
            DispatchThreadError(kInvalidFifoID, inFifoID);
#endif
        }

        return result;
    }



/****************************************************************
 * GetFifoDetails
 *
 * Returns the user-relevant data about a given Fifo.
 *
 * The internal buffer management data is not included here, just
 * the information used to create the Fifo, the current number
 * of data pointers that are in the buffer and the address of
 * the buffer itself.  
 * If any NULL pointers are passed-in then
 * it is assumed that the user isn't interested in that field.
 * There is no check for all the pointers being NULL, as although
 * no information is returned it is not actually wrong.
 ****************************************************************/
    SystemError 
    GetFifoDetails( FifoID     inFifoID,
                    unsigned int     *pOutFifoSize,
                    unsigned int     *pOutNumFreeSlots, 
                    void             **pOutBuffer)
    {
        Fifo *pFifo = FIFOID_TO_FIFO(inFifoID);
        SystemError errorCode =kNoError;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
        if (IS_ISR_LEVEL ())
        {
            KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
        }
#endif

        // We need to protect this from the fifo being
        // destroyed by another thread.  The number of
        // pointers in the buffer could be changed by
        // an ISR, but protection would be futile as
        // the ISR will run when we release the ISR lock
        // before we return.
        TMK_AcquireMasterKernelLock(); 
        
        if( FIFO_TEST(pFifo) )
        {
            // We could fall through here without populating
            // any of the pointers.  
            if( NULL != pOutFifoSize )
            {
                *pOutFifoSize = pFifo->m_BufferSize;
            }

            if( NULL != pOutNumFreeSlots )
            {
                *pOutNumFreeSlots =  pFifo->m_BufferSize - pFifo->m_NumMsgsInBuffer;
            }
           
            if( NULL != pOutBuffer )
            {
                *pOutBuffer = pFifo->m_pBuffer;
            }  
        }
        else
        {
            // Invalid FifoID
            errorCode = kInvalidFifoID;
        }

        TMK_ReleaseMasterKernelLock(); 

        return errorCode;

    }

} /* namespace VDK */


