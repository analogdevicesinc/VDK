/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
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

#include "TMK.h"

#include "VDK_API.h"
#include "IODeviceTable.h"
#include "VDK_Abs_Device.h"
#include "vdk_macros.h"
#include "vdk_history.h"
#include "vdk_thread.h"
#include <limits.h>
#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")

namespace  VDK
{
extern IODeviceTable g_IOdevTable;
extern IODeviceTemplate g_IOTemplates[];
extern unsigned int kNumIOTemplates;

////////////////////////////////////////////////////////////////////////////////
//
//         ** 		I/O Object Management API's    **
//
//    General Note: All I/O objects operate on IOAbstractBase class pointers. 
//                  These APIs are not public so they do not log history 
//                  events or dispatch errors.
//
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// CreateIOObject(ioTemplateID)
//
// Description: Creates I/O object for the specified I/O template. The API
// OpenDevice uses the ID returned by CreateIOObject call. 
//
// In case of failure will return UINT_MAX. 
//
///////////////////////////////////////////////////////////////////////////////

IOID CreateIOObject(const IOTemplateID inIOTemplateID, void *pInitInfo)
{
    IOID theIOObjectID = static_cast <IOID> (UINT_MAX);
    IOAbstractBase *theIODevicePtr=NULL;
#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (IS_ISR_LEVEL ()) 
        KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

    // Check if its valid template
    if(inIOTemplateID < kNumIOTemplates)
    {
	// We want to be in unscheduled region
	//
	TMK_SuspendScheduling();
	
	// Get an empty slot in the table
	theIOObjectID = g_IOdevTable.GetNextObjectID();  

	if(theIOObjectID != UINT_MAX)
	{
	    // Create Object by calling static create function.
	    theIODevicePtr  = (g_IOTemplates[inIOTemplateID].CreateFuncP()); 	
	    if(theIODevicePtr)
	    {
		theIODevicePtr->ID() = theIOObjectID;   // Set device ID
		g_IOdevTable.AddObject(theIODevicePtr); // Add object to Device Table
			
		// Call init function
		//
		VDK::DispatchUnion temp_union={0,0};
		temp_union.Init_t.pInitInfo = pInitInfo;
		theIODevicePtr->DispatchFunction(VDK::kIO_Init,temp_union);
	    }
	    else
		theIOObjectID = static_cast <IOID> (UINT_MAX);
	}

	TMK_ResumeScheduling();
    }

     return theIOObjectID;
}

////////////////////////////////////////////////////////////////////////////////
//
// DestroyIOObject(inObjectID)
//
// Description: Removes the object from the Device table and frees up the memory
// used by the specified I/O object.
// Will not check if there is any other thread is using this object or not. No 
// reference count present for this release.
//
////////////////////////////////////////////////////////////////////////////////

void DestroyIOObject(const IOID inIOObjectID)
{
#if (VDK_INSTRUMENTATION_LEVEL_>0)
    if (IS_ISR_LEVEL ()) 
        KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	
#endif

    TMK_SuspendScheduling();

    IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(inIOObjectID);

    if (NULL != pIODevice)
    {
	g_IOdevTable.RemoveObject(inIOObjectID);
	delete(pIODevice);
    }
#if (VDK_INSTRUMENTATION_LEVEL_>0)
    else    // See the else statement in the debug build
    {
	TMK_ResumeScheduling();
	DispatchThreadError(kBadIOID, inIOObjectID);
	return;
    }
#endif /* VDK_INSTRUMENTATION_LEVEL_ */

    TMK_ResumeScheduling();
}



} /* namespcace VDK */


