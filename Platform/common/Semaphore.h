/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: A single semaphore with a list of pending threads & timeouts
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Semaphores provide a mechanism that prevents multiple clients 
 *  from accessing the same system resource inappropriately..
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "TMK.h"
#include "VDK_API.h"
#include "IDElement.h"

namespace VDK{

    class Thread;
    extern HeapID g_SemaphoreHeap;

    class Semaphore;

// BEGIN_INTERNALS_STRIP
    class Semaphore :
        public IDElementTemplate<SemaphoreID>, public TMK_TimeElement, public TMK_DpcElement
    {
		friend bool	PendSemaphore (const SemaphoreID inSemaphoreID, const Ticks inTimeout);
		friend void	PostSemaphore (const SemaphoreID inSemaphoreID);
		friend unsigned GetSemaphoreValue(const SemaphoreID inSemaphoreID);
		friend void     DestroySemaphore( const SemaphoreID inSemaphoreID);
		friend void	MakePeriodic( const SemaphoreID inSemID, const Ticks inDelay, const Ticks inPeriod );
		friend void	RemovePeriodic( const SemaphoreID inSemID );
		friend SystemError GetSemaphoreDetails( const SemaphoreID inSemID, 
												Ticks			*outPeriod, 
												unsigned int	*outMaxCount );
		friend SystemError GetSemaphorePendingThreads( const SemaphoreID inSemID, 
													   int		*outNumThreads, 
													   ThreadID outThreadArray[],
													   int		inArraySize );


    public:    
    
		Semaphore( SemaphoreID inEnumValue,  unsigned int inInitialValue, unsigned int inMaxCount,
								Ticks inInitialDelay, Ticks inPeriod);

    

        /**
         * Puts the thread onto the Waiting Thread list based on its priority
         */
        void            AddPending(Thread *inThreadP);

    protected:
        /**
         * Your comment here!!!
         */
        static void Timeout(TMK_TimeElement *);
    

        /**
         * Your comment here!!!
         */
        static void Invoke(TMK_DpcElement*);
    
    private:
		TMK_Lock            m_lock;
        TMK_ThreadList  	m_PendingThreads;
        Ticks       	m_InitialDelay;
        Ticks       	m_Period;
		unsigned int		m_MaxCount;
		unsigned int		m_Value;
    };
// END_INTERNALS_STRIP

    struct SemaphoreInfo {
	public:
// BEGIN_INTERNALS_STRIP
	    Semaphore* CreateSemaphore();
// END_INTERNALS_STRIP
	   SemaphoreID m_ID;
	   unsigned int m_InitialValue;
	   unsigned int m_MaxCount;
	   Ticks m_InitialDelay;
	   Ticks m_Period;
    } ;
    
} // namespace VDK

#endif //SEMAPHORE_H_





