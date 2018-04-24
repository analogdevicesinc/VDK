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
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "IDTable.h"
#pragma file_attr("OS_Internals")

////////////////////////////////////////
// Gets the next objectID
////////////////////////////////////////
int
VDK::IDTableMgr::GetNextObjectID(IDTableElement *pIDArray, int arraySize)
{
    // Find the first available entry
    for (int i = 0; i < arraySize; i++)
        if (pIDArray[i].m_ObjectPtr == 0)	
        {
            return ObjectID_From_Index( i, pIDArray ); 
        }

    return (int) UINT_MAX;
}


////////////////////////////////////////
// Removes the object from the internal lists
////////////////////////////////////////
void
VDK::IDTableMgr::RemoveObject( const int inObjectID, IDTableElement *pIDArray )
{
    // Extract information from ObjectID
    int index = Index_From_ObjectID(inObjectID);

#if (VDK_INSTRUMENTATION_LEVEL_>0)
    // If the count is the same, delete the object
    if (pIDArray[index].m_IndexCount != Count_From_ObjectID(inObjectID))
		return;

	// The shifts here are needed to mask off the upper bits so that
	// the count doesn't overflow the reduced range available in the
	// handle.
    pIDArray[index].m_IndexCount = 
		(static_cast<unsigned int>(pIDArray[index].m_IndexCount + 1) << kCountShift) >> kCountShift;
#endif

    pIDArray[index].m_ObjectPtr = 0;
}

////////////////////////////////////////
// Counts all the objects in a table, and optionally inserts the
// the object IDs into a given array.
////////////////////////////////////////
int
VDK::IDTableMgr::GetAllObjectIDs( IDTableElement *pIDTable, 
								  int			 tableSize, 
								  int			 outArray[], 
								  int			 arraySize )
{
	int count = 0;

	// Step through the table, checking for non-zero object pointers
	for (int i = 0; i < tableSize; i++)
	{
		if (pIDTable[i].m_ObjectPtr != 0)	
		{
			//We have found an object, do we need to add it to the array?
			if(count < arraySize)
			{
				outArray[count] =  ObjectID_From_Index( i, pIDTable );
			}
			count++;
		}
	}
	return count;
}


