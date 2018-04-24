/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: APIs that relate to the status information of device flags
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
#include "vdk_History.h"
#include "DeviceFlagTable.h"
#include "VDK_Thread.h"
#include "Heap.h"
#pragma file_attr("OS_Component=DeviceFlags")
#pragma file_attr("DeviceFlags")

namespace VDK
{

SystemError
GetDevFlagPendingThreads( const DeviceFlagID inDevFlagID, 
						  int				 *outNumThreads, 
						  ThreadID			 outThreadArray[],
						  int				 inArraySize )
{
	if NERRCHK(IS_USER_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		
	SystemError errorCode = kInvalidDeviceFlag;

	// This reduces to 'true' for non-error checking libs.
	if ERRCHK( (outNumThreads != NULL) && ((outThreadArray != NULL) || (inArraySize == 0)) )
	{
		// We are going to be reusing the passed-in ThreadID array to get
		// the the thread pointers from the Micro Kernel. 
		//
		// These are completely unrelated types, so trying to cast it in an
		// elegent way just isn't going to work. 
		TMK_Thread **transArray = reinterpret_cast<TMK_Thread **>(outThreadArray);

		// Check to see that the device flag we're looking at exists.
		TMK_AcquireMasterKernelLock();

		DeviceFlag *pDevFlag = g_DeviceFlagTable.GetObjectPtr(inDevFlagID);
		if (NULL != pDevFlag)
		{
			// The device flag exists, get the threadlist
			*outNumThreads = TMK_QueryThreadlist( pDevFlag,
													transArray,
													inArraySize);
			
			//Now we have to convert from TMK_Thread pointers to thread ID's.
			int numFilled = (*outNumThreads < inArraySize? *outNumThreads : inArraySize);
			
			for(int i = 0; i < numFilled; i++)
			{
				outThreadArray[i] = static_cast<Thread *>(transArray[i])->ID();
			}
			errorCode		= kNoError; 
		}
		TMK_ReleaseMasterKernelLock();
	}
	else
	{
		// We have been passed a NULL pointer, set the errorCode and return
		errorCode = kInvalidPointer;
	}

	return errorCode;
}

	int
	GetAllDeviceFlags( DeviceFlagID outDevFlagIDArray[], int inArraySize )
	{
		if NERRCHK(IS_USER_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		// If we are to fill an array, check the pointer is not NULL
		if NERRCHK( (inArraySize > 0) && (outDevFlagIDArray == NULL) )
			DispatchThreadError( kInvalidPointer, 0 );

		// Disable scheduling while we access a global table
		TMK_AcquireMasterKernelLock();
		int count = g_DeviceFlagTable.GetAllObjectIDs(outDevFlagIDArray, 
															inArraySize);
		TMK_ReleaseMasterKernelLock();

		return count;

	}

} // namespace VDK
