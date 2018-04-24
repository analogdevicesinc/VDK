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
 *  Comments: IDTable and IDTableMgr form the base classes for all of the tables
 *  that register subclasses of IDElement.  These tables serve as a cross
 *  reference between ID and a ptr to the element.  Since the public API's take
 *	IDs as arguments rather than object ptrs, and the ID is stored as part of the
 *	object, the user is discouraged from storing pointer that may become stale
 *	when another thread deletes an object.  Also, the table is designed to issue
 *	pseudo-unique ID numbers, making coincidental ID reuse unlikely.
 *	The table should be used as follows:
 *		(1) Get a new ID from the table for the new object.
 *		(2) Create the object, allocating any memory the object might need.
 *		(3) If the construction was successful, add the object to the IDTable.
 *	The table holds an array of IDTableElements, each of which has a ptr and a 
 *	count.  Therefore, the table requires kMaxNumElements*2 words of memory.
 *
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef __IDTABLE__
#define __IDTABLE__

#include <string.h>
#include <limits.h>
#include "VDK_API.h"
#include "IDElement.h"
#include "KernelPanic.h"

namespace VDK
{
	struct IDTableElement
	{
		IDElement		    *m_ObjectPtr;
		int					m_IndexCount;
	};


	// This class is never actually instantiated, it's just a namespace for a set of
	// functions. It's like this for historical reasons.
	//
    class IDTableMgr
    {
	    public:

	        /**
	         * Returns the next valid ObjectID
	         *
	         * SHOULD BE CALLED AND USED FROM WITHIN AN UNSCHEDULED REGION so that
	         * another thread doesn't come in and try and use the Object before it
	         * goes invalid
	         */
		    static int    GetNextObjectID(IDTableElement *pIDArray, int arraySize);
	    
	        /**
	         * Adds a thread to the thread table.  Returns the ObjectID
	         * Should be called in the Object::Object() or OS:CreateObject()
	         */
	        static void   AddObject(IDElement* inObjectPtr, int inID, IDTableElement *pIDArray)
			{
				pIDArray[Index_From_ObjectID(inID)].m_ObjectPtr = inObjectPtr;
			}

			/**
			 * Counts all the objects in a table, and optionally inserts the
			 * IDs into a given array.
			 */
			static int	GetAllObjectIDs( IDTableElement *pIDTable, 
										 int			tableSize, 
										 int			outArray[], 
										 int			arraySize );
	    
	        /**
	         * Removes a thread from the table
	         * Should be called in DestroyObject
	         */
	        static void   RemoveObject( const int inObjectID, IDTableElement *pIDArray );
	    
	        /**
	         * Returns a pointer to the specified thread given its Object
	         */
	        static IDElement*  GetObjectPtr( const int inObjectID, IDTableElement *pIDArray, int arraySize )
			{
				// Extract information from ObjectID
				int index = Index_From_ObjectID(inObjectID);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
				int object_count = Count_From_ObjectID(inObjectID);

				// If the count is the same, return the Object*
				if ((index >= arraySize) || (pIDArray[index].m_IndexCount != object_count))
					return static_cast<IDElement*>(0); 
#endif
				return pIDArray[index].m_ObjectPtr;
			}
	    
		protected:
	    
	    private:
	        IDTableMgr(); // declared but not defined

	        enum { kIndexBits = 9 };
	        enum { kNodeBits  = 5 };
	        enum { kIndexMask = ~ (-1 << kIndexBits) };
	        enum { kNodeMask  = ~ (-1 << kNodeBits) };
	        enum { kCountShift = ( kIndexBits + kNodeBits) };

	    public:
	        /**
	         * Used internally in ObjectList to abstract away Word math.
	         * See also: Count_From_ObjectID
	         */
	        static int Index_From_ObjectID(const int inObjectID)
				{ return inObjectID & static_cast<int>(kIndexMask); }
	    						// compiler warning: integer conversion resulted in a change of sign
	    
	        static int Node_From_ObjectID(const int inObjectID)
				{ return (static_cast<unsigned int>(inObjectID) >> kIndexBits) & static_cast<int>(kNodeMask); }
	    
	        /**
	         * Used internally in ObjectList to abstract away word math.
	         * See also: Index_From_ObjectID
	         */
	        static int Count_From_ObjectID(const int inObjectID)
				{ return (static_cast<unsigned int>(inObjectID) >> (kIndexBits + kNodeBits)); }

			static int Count_For_Index(int i, IDTableElement *pIDArray)
				{ return pIDArray[i].m_IndexCount << kCountShift; }

			/**
			 * This function calculates a pseudo-unique ID for error checking
			 * and fully instrumented libraries based on the position within
			 * the IDTable. A pseudo-unique ID is not issued for non-error
			 * checking libraries.
			 */
			static int ObjectID_From_Index( int index, IDTableElement *pIDArray )
			{ 
#if (VDK_INSTRUMENTATION_LEVEL_>0)
				return (index | (pIDArray[index].m_IndexCount << kCountShift));
#else
				return index;
#endif
			}

	};
    


//------------------------------------------------------------------------------------------------------------

	template <class T, class Tid> class IDTable
	{
		public:
	        Tid				GetNextObjectID() const
								{ return static_cast<Tid>(IDTableMgr::GetNextObjectID(m_IDTable, m_TableSize)); }

	        void            AddObject(T* inObjectP)
								{ IDTableMgr::AddObject(static_cast<T*>(inObjectP), static_cast<int>(inObjectP->ID()), m_IDTable); }

	        void            RemoveObject( const Tid inTid )
								{ IDTableMgr::RemoveObject(static_cast<Tid>(inTid), m_IDTable); }

	        T*				GetObjectPtr( const Tid inTid )
								{ return static_cast<T*>(IDTableMgr::GetObjectPtr(static_cast<Tid>(inTid), m_IDTable, m_TableSize)); }

			int             Count_For_Index(int i)
				                { return IDTableMgr::Count_For_Index(i, m_IDTable); }

            int             GetAllObjectIDs(Tid outArray[], int inArraySize)
				                { return IDTableMgr::GetAllObjectIDs(m_IDTable, 
                                                                    m_TableSize, 
                                                                    reinterpret_cast<int*>(outArray), 
                                                                    inArraySize ); }

			IDTableElement		*m_IDTable;
			int					m_TableSize;
	};

//------------------------------------------------------------------------------------------------------------


} // namespace VDK


#endif // __IDTABLE__


