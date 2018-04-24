/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Abstract base class for a schedulable thread of execution
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Thread is the base class for all threads of execution in VDK.
 *  The thread class contain all of the data structure required for pending on
 *  semaphores or events (with or without timeout), waiting in the time queue,
 *  waiting to run in the ready queue or executing.  The class provides two
 *  entry points: Run() and Error().  Run() is a pure virtual function.  Error()
 *  has a default implentation, but may be overridden by a user implementation.
 *  Threads are always accessed by pointers within the kernel, but always only
 *  by ThreadIDs from the public interface to avoid dangling references.  
 *
 *  Threads have two sets of doubly linked list pointers through multiple 
 *  inheritance of ListElement.  Through PriorityListElement, Thread inherits 
 *  pointers that it uses for insertion into the ready queue or for pending on
 *  a semaphore, event or Fifo.  Through TimeListElement, Thread inherits a 
 *  second set of pointers that it uses for insertion onto the time queue for 
 *  sleep (pure delay) or for timeout in conjunction with pending.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/




#ifndef VDK_THREAD_H_
#define VDK_THREAD_H_

#include "TMK.h"

#include <VDK_API.h>
#include <VDK_History.h>
#include <VDK_DCB.h>
#include <MessageQueue.h>
#include <vdk_Abs_Device.h>
#include "Heap.h"
#include "BitOps.h"

namespace VDK 
{
    class DebugInfo;
	class Thread;
	struct ThreadTemplate;

// BEGIN_INTERNALS_STRIP

     extern void InstrumentStackNum(unsigned int);

// END_INTERNALS_STRIP
    
    /**
     * Thread is the base class for all user and system threads.  To create
     * a Thread, the user inherits the VDK::Thread class, overloads the Run() function
     * (and Error(...) if desired), and has a static Create(VDK::ThreadCreationBlock&)
     * function.  The static Create(...) function just creates a new derived thread.
     */
    class Thread
        : public VDK::IDElementTemplate<ThreadID>, public TMK_Thread, public TMK_TimeElement
        
    {
        
    friend void DestroyThread( const ThreadID inThreadID, bool inDestroyNow );
    friend bool C_ISR_PostFifo(FifoID 	inFifoID, void 	        *pInData );
    friend bool PostFifo(FifoID 	inFifoID, void 	*pInData, Ticks 	inTimeout );
    friend bool PendFifo(FifoID 	inFifoID, void	**pOutData, Ticks  	inTimeout );

    public:
	void * operator new(size_t allocate_size,  ThreadType templateID);
	void * operator new(size_t allocate_size,  ThreadCreationBlock &tcb);
	void * operator new(size_t allocate_size);
	void * operator new(size_t allocate_size,const _STD nothrow_t) {
		return ::new (_STD nothrow) char[allocate_size];
	}
	inline void operator delete(void * ptr) { ::delete ptr;}
	inline void operator delete(void * ptr, ThreadType templateID) { ::delete ptr;}
	inline void operator delete(void * ptr, ThreadCreationBlock &tcb) { ::delete ptr;}
	inline void operator delete(void * ptr, const _STD nothrow_t) { ::delete ptr;}
        /**
         * This typedef is needed because existing user thread classes use 
         * VDK::Thread::ThreadCreationBlock in their create functions, whereas
         * ThreadCreationBlock now resides in the outer VDK namespace.
         */
    	typedef ::VDK::ThreadCreationBlock ThreadCreationBlock;

        /**
         * Datatype that is the pointer to functions that create user threads.
         */
        typedef VDK::Thread* (*CreateFunctionP)(ThreadCreationBlock&);
    
        /**
         * Begin member functions
         */
        Thread(ThreadCreationBlock		&t);
    
        /**
         * Virtual destructor so all destruction takes place on a thread
         */
        virtual ~Thread();
    
    	/**
    	 * Overload the Run() function to accomplish some useful work
    	 */
        virtual void Run() = 0;
    
    	/**
    	 * Default Error handler.  It destroys this thread.
		 */
        virtual int ErrorHandler();
// BEGIN_INTERNALS_STRIP

		/**
		 * Error state handlers
		 */
		inline const VDK::SystemError&	LastErrorType() const
			{ return m_LastErrorType; }

		inline VDK::SystemError&  LastErrorType()
			{ return m_LastErrorType; }

		inline const int&	LastErrorValue() const
			{ return m_LastErrorValue; }

		inline int& LastErrorValue()
			{ return m_LastErrorValue; }


        /**
         * Message Queue
         */
        VDK::MessageQueuePtr &        		MessageQueue()
                                            { return m_MessageQueuePtr; }
        const VDK::MessageQueuePtr & 		MessageQueue() const
                                            { return m_MessageQueuePtr; }

        /**
         * Priority handlers
         */
        inline VDK::Priority DefaultPriority() const
    		{ return m_InitialPriority; }
    
        /**
         * Get the Thread's TemplatePtr.
         */
        inline ThreadTemplate* TemplatePtr() const
    		{ return m_TemplatePtr; }
    
    
        /**
         * Used to tell the index of Event or Semaphore a Thread is blocked on.
         *
         * These two functions are only used in API.cpp in SetPriority
         */
        inline int&                 BlockedOnIndex()
                                            { return m_BlockedOnIndex; }
        inline const int&           BlockedOnIndex() const
                                            { return m_BlockedOnIndex; }

        /**
         * Used in API.cpp to pass when an error has occured on a pend back from
         * VDK::Thread::Timeout().  VDK::Thread::Timeout() sets the m_TimeoutOccured()
         * to true, and PendSemaphore/PendEvent check it.
         */
        inline const bool&  TimeoutOccurred() const
                                { return m_TimeoutOccurred; }
        inline bool&        TimeoutOccurred()
                                { return m_TimeoutOccurred; }
    
	unsigned int GetStackUsed();
	unsigned int GetStackUsed2();
	friend void VDK::InstrumentStackNum(unsigned int);
protected:
        /**
         * When a thread has timed out on the time queue, this function
         * is called via the m_pfTimeout function pointer in TimeListElement.
         */
        static void Timeout(TMK_TimeElement*);


		static void RunFunc(TMK_Thread *pTmkThread);

//--------------------------------------------------------------------------------
/*	Thread Local Storage: */ 
// END_INTERNALS_STRIP
	private:
		enum { kMaxNumDataSlots = 8 }; // This number must be less than machine word size - 1!!!

	public:
// BEGIN_INTERNALS_STRIP
		static bool AllocateTLSSlot(int *ioSlotNum, void (*pfCleanup)(void *));

		bool SetTLSSlotNum(int inSlotNum, void *inValue);

		void* GetTLSSlotValue(int inSlotNum);

		static bool FreeTLSSlot(int inSlotNum);

//--------------------------------------------------------------------------------


        /**
         * Returns true if a thread was created correctly
         */
        bool                            DidConstructOkay(const ThreadTemplate &tplate) const
		{
			return s_didConstructOK;
		}

        /**
         * Returns pointer to the specified DeviceControlBlock
         */
        VDK::DeviceControlBlock*        GetDCB(VDK::DeviceDescriptor inDD)
                                            {
                                                if ((inDD > kNumDCBs) || IsBitSet(m_DCBNotUsed, inDD))
                                                    return 0;
                                                return &(m_DCBs[inDD]);
                                            }

        /**
         * Used by OpenDevice(...) to get a DeviceControlBlock
         */
        VDK::DeviceDescriptor           AllocateDCB()
                                        {
                                            // Are there any free DCBs?
                                            DeviceDescriptor    ret_val = static_cast<DeviceDescriptor> (FindFirstSet(m_DCBNotUsed));

                                            // If we found a DCB that wasn't used, we want to allocate it.
                                            if (ret_val != static_cast<DeviceDescriptor>(VDK::kNoneSet))
                                                m_DCBNotUsed &= ~(1 << ret_val); // clear the bit
                                            return ret_val;
                                        }


        /**
         * Frees an allocated DCB
         */
        void                            FreeDCB(DeviceDescriptor inDD)
                                            { m_DCBNotUsed |= (1 << inDD); }

// END_INTERNALS_STRIP
	// Query functions only
        unsigned int                    StackSize() const
                                            { return m_StackSize; }
        unsigned int*                   Stack() const
                                            { return m_StackP; }
        unsigned int*                   StackBase() const
                                            { return m_StackP - (m_StackSize - 1); }		
#ifdef __ADSPTS__
        unsigned int                    Stack2Size() const
                                            { return m_StackSize2; }
        unsigned int*                   Stack2() const
                                            { return m_StackP2; }
#endif /*__ADSPTS__*/
        void**             				DataPtr() 
                                            { return &m_DataPtr; }
		bool                            StackGuardWordChanged();
// BEGIN_INTERNALS_STRIP
        const bool&                     ShouldDispatchError() const
                                            { return m_ShouldDispatchError; }
        bool&                           ShouldDispatchError()
                                            { return m_ShouldDispatchError; }
		DebugInfo*                      ThreadDebugInfo()
											{ return m_DebugInfoP; }


    private:
		void							AllocateThreadMemory(const ThreadTemplate &, ThreadCreationBlock &);
		void							FreeThreadMemory(const ThreadTemplate &tplate);


// END_INTERNALS_STRIP
    
    private:

        enum { kNumDCBs = 8 };      // This number must be less than machine word size!!!

        unsigned int            	m_StackSize;
        unsigned int            	*m_StackP;
#ifdef __ADSPTS__
        unsigned int            	m_StackSize2;
        unsigned int            	*m_StackP2;
#endif
        VDK::DebugInfo     			*m_DebugInfoP;
        unsigned int               m_DCBNotUsed;       // The bit will be cleared when the DCB is used
        VDK::DeviceControlBlock  	m_DCBs[kNumDCBs];
	VDK::SystemError 			m_LastErrorType;
	int							m_LastErrorValue;
	void						*m_SlotValue[kMaxNumDataSlots];
        VDK::MessageQueuePtr           m_MessageQueuePtr;
        ThreadTemplate*              	m_TemplatePtr;
        VDK::Priority				m_InitialPriority;
        int                     	m_BlockedOnIndex;
        bool                    	m_TimeoutOccurred;
        bool                        m_ShouldDispatchError;
#ifndef __ADSPTS__
        bool                        m_UserAllocatedStack;
#endif

		// Class variables for thread-local storage management
		static unsigned int         c_UnusedSlots;
		static void               (*c_CleanupFns[kMaxNumDataSlots]) (void *);

	protected:
		void						*m_DataPtr;

        // m_APIData is an API specific field that should only be 
        // used within the execution of a VDK API.  Once a thread
        // returns from that API then the data is no longer valid.
        // The field's use is API defined.
		void                        *m_APIData;

		static bool s_didConstructOK;
};
// BEGIN_INTERNALS_STRIP
extern Thread *g_CurrentThreadPtr;
// END_INTERNALS_STRIP


	/*******************************************************************************
	 * The ThreadTemplate class holds parameters declared by the user that define
	 * a thread subclass.  Parameters that are passed in through the IDE are placed
	 * in the members of the ThreadTemplate by its constructor and provide a 
	 * centralized place for initialization data used by CreateThread().
	 ******************************************************************************/    
        struct ThreadTemplate
    {
		char*                           name;
// Variables for choice of heaps in the different areas of the thread
#ifndef __ADSPBLACKFIN__
// blackfin does not have any context
		HeapID              context1Heap;
#endif
		HeapID                          stack1Heap;
#ifdef __ADSPTS__
		HeapID                          context2Heap;
		HeapID                          stack2Heap;
#endif
		HeapID                          threadStructHeap;

		VDK::Priority                   priority;
		unsigned int                    stackSize;
#ifdef __ADSPTS__
		unsigned int                    stackSize2;
#endif
		VDK::Thread::CreateFunctionP    createFunction;
		bool                            messagesAllowed;
#ifdef __ADSPBLACKFIN__
		unsigned int                    pad;  // pad out to 32 bytes
#endif
	};

	struct IdleThreadInfo
    {
		unsigned int			stackSize;
#ifdef __ADSPTS__
		unsigned int			stackSize2;
#endif
		HeapID				heap;
    };

} // namespace VDK

#endif // VDK_THREAD_H_





