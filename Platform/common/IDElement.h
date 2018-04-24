/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: 
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: IDElement is the base class for all objects that can be indexed
 *	in an IDTable or its subclasses.  Any VDK object that requires a unique 
 *	integer ID should subclass IDElement rather than add it as a private member.
 *  
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef __IDELEMENT__
#define __IDELEMENT__


namespace VDK
{

	class IDElement	
	{


		protected:
			unsigned int				m_ID;
	};

template <class IDType> 
	class IDElementTemplate:public IDElement {
		public:
	        inline IDType& ID()
	    		{ return *(reinterpret_cast <IDType *> (&m_ID)); }
	    
	        inline const IDType& ID() const
	    		{ return *(reinterpret_cast <IDType *> (&m_ID)); }
	};

}  // namespace VDK


#endif  // __IDELEMENT__





