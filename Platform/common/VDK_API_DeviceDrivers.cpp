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
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#include "VDK_Macros.h"
#include "VDK_History.h"
#include "vdk_abs_device.h"
#include "vdk_driver.h"
#include "IODeviceTable.h"
#pragma file_attr("OS_Component=DeviceDrivers")
#pragma file_attr("DeviceDrivers")


namespace VDK
{
    extern int        		g_SchedulerRegionCount;

	// IO device table and template tables
	//
	extern IODeviceTable g_IOdevTable;
	extern IODeviceTemplate g_IOTemplates[];

	// Used to get the objects, will be initialized to NULL if no IO stuff is used.
	// see vdk_thread.cpp
	//
    extern IOAbstractBase* (*GetIOObjectPtr) (IOID inDevID);
	IOAbstractBase* IOObjectAccessFunc(IOID inDevID);

	//
	// Constructor
	//
        DeviceDriver::DeviceDriver()
	{
	    TMK_InitDpcElement(this, Invoke);         // set up DPC invoke function pointer
	}

	//
	// Invoke() static member function for class DeviceDriver. Since
	// DeviceDriver has no other members functions we put this here
	// rather than create another source module just to hold it.
	//
#pragma param_never_null pDpcQE
	void
	DeviceDriver::Invoke(TMK_DpcElement *pDpcQE)
	{
		static VDK::DispatchUnion dispatchUnion = {0,0};
		DeviceDriver *pSelf = static_cast<DeviceDriver*>(pDpcQE);

		
		pSelf->DispatchFunction(kIO_Activate, dispatchUnion);
	}


	DeviceDescriptor
	OpenDevice(IOID inIDNum, char *inFlags)
	{
	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

	    // Find a valid device control block within the currently running thread
		DeviceDescriptor ret_val = pThread->AllocateDCB();

	    if ERRCHK(ret_val != static_cast<DeviceDescriptor>(VDK::kNoneSet))
	    {
			// Get the object pointer from the device table.
			IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(inIDNum);

	        if ERRCHK(NULL != pIODevice)
	        {
				DispatchUnion t;
        	    DeviceControlBlock  *pDCB = pThread->GetDCB(ret_val);

	            // Setup the device control block
	            pDCB->WhichDevice() = inIDNum;

	            // Pass the parameters to the dispatch routine
	            t.OpenClose_t.dataH = pDCB->DD_DataPointer();
	            t.OpenClose_t.flags = inFlags;
				pIODevice->DispatchFunction(kIO_Open,t);
				// Set the GetObjectPtr to point to appropriate place
				GetIOObjectPtr =  IOObjectAccessFunc;
	        }
	        else
            {
	            // We were passed an unknown device ID
                // Free up the allocated DCB
                pThread->FreeDCB(ret_val);

                // Call the error function
	            ret_val = static_cast<DeviceDescriptor> (DispatchThreadError(kBadIOID, inIDNum));
            }
	    }
	    else
	        // We passed a bad dcb, so throw the error function
	        ret_val = static_cast<DeviceDescriptor> (DispatchThreadError(kOpenFailure, inIDNum));

	    return ret_val;
	}


	unsigned int
	SyncRead(DeviceDescriptor inDD, char *outBuffer, const unsigned int inSize, Ticks inTimeout)
	{
	    void *read_amnt = 0;
		
	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

	    // Get the device control block
	    DeviceControlBlock  *pDCB = pThread->GetDCB(inDD);
		
		// Get the pointer to the I/O object
		//
		IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(pDCB->WhichDevice());

	    // Make sure the device control block is valid
	    if ERRCHK(pDCB != 0 && pIODevice)
	    {
			DispatchUnion t;

	        // Call the device dispatch routine
	        t.ReadWrite_t.dataH = pDCB->DD_DataPointer();
	        t.ReadWrite_t.timeout = inTimeout;
	        t.ReadWrite_t.dataSize = inSize;
	        t.ReadWrite_t.data = outBuffer;
			read_amnt =  pIODevice->DispatchFunction(kIO_SyncRead,t);
	    }
	    else
	        // we were passed a bad dcb
	        read_amnt = reinterpret_cast<void *> (DispatchThreadError(kBadDeviceDescriptor, inDD));

	    return reinterpret_cast<unsigned int>(read_amnt);
	}


		  
	unsigned int
	SyncWrite(DeviceDescriptor inDD, char *inBuffer, const unsigned int inSize, Ticks inTimeout)
	{
	    DispatchUnion t;
	    void *write_amnt = 0;

	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());

#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

	    // Get the device control block
	    DeviceControlBlock *pDCB = pThread->GetDCB(inDD);

		// Get the pointer to the I/O object
		//
		IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(pDCB->WhichDevice());

	    // Make sure the device control block is valid
	    if ERRCHK(pDCB != 0 && pIODevice)
	    {
	        // Call the device dispatch routine
	        t.ReadWrite_t.dataH = pDCB->DD_DataPointer();
	        t.ReadWrite_t.timeout = inTimeout;
	        t.ReadWrite_t.dataSize = inSize;
	        t.ReadWrite_t.data = inBuffer;
			write_amnt = pIODevice->DispatchFunction(kIO_SyncWrite,t);
	    }
	    else
	        // we were passed a bad dcb
	        write_amnt = reinterpret_cast<void *> (DispatchThreadError(kBadDeviceDescriptor, inDD));

	    return reinterpret_cast<unsigned int>(write_amnt);
	}


	int
	DeviceIOCtl(DeviceDescriptor inDD, void *inCommand, char *inParameters)
	{
	    DispatchUnion t;
	    void *ret_val = 0;

	    if NERRCHK(IS_ISR_LEVEL ()) 
		KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());
		
#pragma suppress_null_check
		Thread *pThread = static_cast<Thread*>(TMK_GetCurrentThread());

	    // Get the device control block
	    DeviceControlBlock  *pDCB = pThread->GetDCB(inDD);

		// Get the pointer to the I/O object
		IOAbstractBase *pIODevice = g_IOdevTable.GetObjectPtr(pDCB->WhichDevice());

	    // Make sure the device control block is valid
	    if ERRCHK(pDCB != 0 && pIODevice)
	    {
	        // Call the device dispatch routine
	        t.IOCtl_t.dataH = pDCB->DD_DataPointer();
	        t.IOCtl_t.command = inCommand;
	        t.IOCtl_t.parameters = inParameters;

	        ret_val = pIODevice->DispatchFunction(kIO_IOCtl, t);
	    }
	    else
	        // we were passed a bad dcb
	        ret_val = reinterpret_cast<void *> (DispatchThreadError(kBadDeviceDescriptor, inDD));

	    return reinterpret_cast<int>(ret_val);
    }
//
//  This function  is used to get the IO device object pointer. GetIOObjectPtr function 
//  pointer will be pointed to this when device drivers are used.
//  
//  inDevID is the device opened by OpenDevice Call
//
IOAbstractBase* IOObjectAccessFunc(IOID inDevID)
{
	IOAbstractBase *pIOObject = g_IOdevTable.GetObjectPtr(inDevID);
	return(pIOObject);
}

//   overloaded new operator so we can allocate from the heap we need to use.

void * DeviceDriver::operator new (size_t allocate_size,  VDK::HeapID heap){
         return _heap_malloc(g_Heaps[heap].m_Index,allocate_size);
}

void * DeviceDriver::operator new (size_t allocate_size){
         return ::new char[allocate_size];
}


} // namespace VDK

