/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Messages form the basis for interprocess and interprocessor
 *		communication.
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/




#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "PriorityListElement.h"
#include "IDElement.h"
#include "VDK_API.h"

#include <new>

namespace VDK
{
	extern PoolID g_MessagePoolID ; // if there is a message pool, the 
				     // IDDE will put it in 0 

	class Message : public IDElementTemplate<MessageID>, public PriorityListElement
	{
		public:
			Message(MessageID 	inMsgID, 
					int			inPayloadType, 
					int 		inPayloadSize, 
					void 	   *inPayloadAddr):	m_PayloadP(inPayloadAddr),
												m_PayloadType(inPayloadType),
												m_NumElements(inPayloadSize)	
												{ 	m_ID = inMsgID;	
													m_Sender = static_cast<VDK::ThreadID>(0);	// == idle thread
													m_Owner  = static_cast<VDK::ThreadID>(0);	// == idle thread
													m_Target = static_cast<VDK::ThreadID>(0);	// == idle thread
												}
// BEGIN_INTERNALS_STRIP
												
			void* 							operator new(size_t s)
												{ // s = fixed block size
												  return MallocBlock(g_MessagePoolID); }

			void 							operator delete(void *p, size_t s)
												{ // s = fixed block size
												   FreeBlock(g_MessagePoolID, p); }

#ifdef VDK_INCLUDE_MP_MESSAGES_
										// returns the value of the processor ID encoded in the high
										// bits of the MessageID (this->m_ID).
	        inline const int&	    		ProcessorID() const
	                                        	{ return ... }
#endif	// VDK_INCLUDE_MP_MESSAGES_


			void*&							PayloadPtr()
												{ return m_PayloadP; }
			const void*  					PayloadPtr() const
												{ return m_PayloadP; }

			int&							PayloadType()
												{ return m_PayloadType; }
			const int& 						PayloadType() const
												{ return m_PayloadType; }

			unsigned int&					PayloadSize()
												{ return m_NumElements; }
			const unsigned int&				PayloadSize() const
												{ return m_NumElements; }

	        ThreadID&    					Sender()
	                                 			{ return m_Sender; }
	        const ThreadID&    				Sender() const
	                                 			{ return m_Sender; }
	        VDK::MsgChannel			        Channel() const
	                                 			{ return static_cast<VDK::MsgChannel>(1 << Priority()); }
	        ThreadID&    					Owner()
	                                 			{ return m_Owner; }
	        const ThreadID&    				Owner() const
	                                 	      	{ return m_Owner; }
	        ThreadID&    					Target()
	                                 			{ return m_Target; }
	        const ThreadID&    				Target() const
	                                 	      	{ return m_Target; }

// END_INTERNALS_STRIP


		private:
			
			int					m_PayloadType;
			void				*m_PayloadP;
			unsigned int		m_NumElements;
			VDK::ThreadID		m_Sender;
			VDK::ThreadID		m_Owner;
			VDK::ThreadID		m_Target;
	};

} // namespace VDK


#endif //  __MESSAGE_H__



