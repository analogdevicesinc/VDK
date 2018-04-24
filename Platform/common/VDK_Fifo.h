/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Fifo class defines a message queue synchronisation 
 *   primative for VDK.  
 *
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Each message is a single pointer of data that is sent-
 *   to and received from a Fifo object.  The fifo object manages a buffer 
 *   containing the messages. The buffer is allocated seperately from the fifo 
 *   object, and can be allocated either by the user or VDK.  Threads can block 
 *   whilst pending on or posting to a fifo.  The only functionality allowed at
 *   kernel and ISR level is posting without blocking.
 *
 *   The primitive is called a Fifo, rather than MessageQueue to avoid customer 
 *   confusion between the existing VDK Messaging model and this one. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef FIFO_H_
#define FIFO_H_

#include "TMK.h"
#include "VDK_API.h"
#include "IDElement.h"

#include <assert.h>

/* 
 * Temporarily using a signature to identify that a pointer
 * actually points to a Fifo.   
 *
 * May be superceded with the introduction of IDTables for 
 * Andromeda development.
 */
#if __ADSP21000__ || __ADSPTS__
#define FIFO_SIGNATURE (static_cast<FifoID>(0x41464D51))
#define DEAD_FIFO_SIGNATURE (static_cast<FifoID>(0x58464D51))
#else
#define FIFO_SIGNATURE (static_cast<FifoID>('AFMQ'))
#define DEAD_FIFO_SIGNATURE (static_cast<FifoID>('DFMQ'))
#endif


namespace VDK
{

    class Fifo;

    class Fifo :
        public IDElementTemplate<FifoID>, public TMK_DpcElement
    {

// BEGIN_INTERNALS_STRIP
        friend void     DestroyFifo( FifoID inFifoID );
        
        friend FifoID   CreateFifo( unsigned int inSize, 
                                    void *pInBuffer, 
                                    void (*pfInBufferCleanup)(void*) );

        friend bool C_ISR_PostFifo( FifoID     inFifoID, 
                                    void           *pInData );

        friend bool PostFifo( FifoID    inFifoID, 
                              void          *pInData, 
                              Ticks         inTimeout );

        friend bool PendFifo(   FifoID  inFifoID,
                                void            **pOutData,
                                Ticks           inTimeout );

        friend SystemError GetFifoDetails( FifoID   inFifoID,
                                           unsigned int     *pOutBufferSize,
                                           unsigned int     *pOutNumFreeSlots, 
                                           void             **pOutbuffer);

    public:    
    
        Fifo( const FifoID inEnumValue,  unsigned int inSize, void *pInBufferLocation, void (*pfInBufferCleanup)(void*) ); 

    protected:
        
        
        /**
         * Query if the fifo is empty
         **/
        unsigned int isEmpty()
        {
            return (0 == m_NumMsgsInBuffer); 
        }

        /**
         * Query if the fifo is full
         */
         bool isFull()
         {
            return (m_NumMsgsInBuffer == m_BufferSize);
         }

        /**
         * Add a data word to the fifo buffer 
         * Must be called with interrupts disabled.
         **/
        void Add(void *pInData)
        {
            // The interrupt lock must be held
            assert(Fifo::s_lock.lockHeld);

            m_NumMsgsInBuffer++;
            m_pBuffer[m_FreeIndex++] = pInData;

            // Wrap-around the free index if need be.
            if( m_FreeIndex >= m_BufferSize )
            {
                // m_FreeIndex's max should be m_BufferSize, any higher and 
                // something has gone wrong.
                assert(!(m_FreeIndex > m_BufferSize));

                m_FreeIndex = 0;
            }
        }

        /** 
         * Remove a data word from the fifo buffer      
         * Must be called with interrupts disabled.
         **/
        void *Remove()
        {
            // The interrupt lock must be held
            assert(Fifo::s_lock.lockHeld);

            void *pData = m_pBuffer[m_HeadIndex++];
            m_NumMsgsInBuffer--;
        
            if( m_HeadIndex >= m_BufferSize )
            {
                // m_HeadIndex's max should be m_BufferSize, any higher and 
                // something has gone wrong.
                assert(!(m_HeadIndex > m_BufferSize));

                m_HeadIndex = 0;
            }
            return pData;
        }

        /** 
         * Queries the fifo to see if any threads are blocked posting.
         **/
        bool hasPostingThreads()
        {
            return (kBlockedOnFifoPost == m_BlockingDirection) &&
                    !TMK_IsThreadListEmpty(&m_BlockedThreads); 
        }

        /** 
         * Queries the fifo to see if any threads are blocked pending.
         * Note:  Pending threads can be removed from ISR level.
         **/
        bool hasPendingThreads()
        {
           return (kBlockedOnFifoPend == m_BlockingDirection)  && 
                  !TMK_IsThreadListEmpty(&m_BlockedThreads);
        }


        /**
         * This is the Deferred Procedure Call used at Kernel Level when
         * a PostFifoISR() has been called at ISR level and at least one 
         * pending thread has been unblocked.  The invoke function 
         * cancels any Pend timeout and places the thread/threads back
         * into the ReadyQ.
         */
        static void Invoke(TMK_DpcElement*);
        
        /**
         * If a Fifo is created and the buffer is allocated by VDK, this
         * cleanup function is used to deallocate the buffer.
         */
        static void BufferCleanup(void *pInBuffer);

        /** 
         *  The following members are all modified at ISR level, so they need
         *  to be protected whenever accessed.  The s_lock member is used
         *  for the global interrupt lock.
         */
        
// END_INTERNALS_STRIP
        static TMK_Lock    s_lock; /*Used for interrupt locking throughout */ 

        TMK_ThreadList      m_ISRUnblockedThreads;  /* Used to hold any threads that have been unblocked from ISR level. */
        TMK_ThreadList      m_BlockedThreads;    /* The blocked thread chain for pending and posting threads */
        unsigned int        m_NumMsgsInBuffer;   /* How many slots are filled*/
        unsigned int        m_FreeIndex;   /* Next available slot */
        FifoBlockingDirection  m_BlockingDirection; /* The blocking direction of the chain */
        
        /**
         * These members are not ISR-sensitive.
         */
        unsigned int        m_BufferSize;     /* size of the buffer array, measured in data pointers */
        unsigned int        m_HeadIndex;   /* The first dataptr to be removed - only modified at thread level*/
        void                (*m_pfBufferCleanup)(void *); /* This is populated with the buffer cleanup fn. */
        void                **m_pBuffer;      /* A fixed-size array for data pointers */
    };


    typedef struct BootFifoInfo
    {
        unsigned int   inFifoID;
        unsigned int   inBufferSize;
        void           *inBufferPtr;
        void           (*inCleanup)(void*);
    }BootFifoInfo;
    
} // namespace VDK

#endif //FIFO_H_

