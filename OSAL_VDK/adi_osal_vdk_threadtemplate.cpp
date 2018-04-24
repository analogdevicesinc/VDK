/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: 
 *
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 *  Copyright (c) 2009 Analog Devices, Inc.,  All rights reserved
 *
 *  This software is the property of Analog Devices, Inc.
 *
 *  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
 *  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
 *  PERFORMANCE, OR USE OF THIS SOFTWARE.
 * ===========================================================================*/

#include "osal_vdk.h"

#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")


/**
 * class OSALThread is the thread class used by OSAL for all its
 * threads.  
 */
class OSALThread : public VDK::Thread 
{
    public:
        /* OSALRunData struct defined in osal_vdk.h */  
        OSALRunData m_ThreadData;
    
    void Run()
    {
        /* Call the user-supplied run function - should never return*/
        (*m_ThreadData.pfRun)(m_ThreadData.pParam);
    
        /* If the user follows the OSAL guidelines then this point should
         * not be reached.  The behaviour at this point should be
         * undefined, but in VDK we will destroy this thread. 
         * Destruction and cleanup are not deferred, destruction is now. */
        VDK::DestroyThread(VDK::GetThreadID(), true);
        
        /* We really should never get here */    
    }
    
    int
    ErrorHandler()
    {
        return (0); 
    }
    
    OSALThread(VDK::Thread::ThreadCreationBlock &tcb)
        : VDK::Thread(tcb)
    {
        /* We need to store the run function and param as part of the
         * thread object.  */
        OSALRunData 
        *createData = static_cast<OSALRunData *>(tcb.user_data_ptr);
        
        m_ThreadData.pfRun   = createData->pfRun;
        m_ThreadData.pParam  = createData->pParam;
    }
 
    ~OSALThread()
    {
        /* Do nothing */
    }

    static VDK::Thread*
    Create(VDK::Thread::ThreadCreationBlock &tcb)
    {
            return new (tcb) OSALThread(tcb);
    }

};  /* OSALThread */

/*  The thread template priority  and stack size are never used during thread 
 *  creation as they will be overridden on thread creation. The thread stack 
 *  heap is used in the 5.0 port.
 *  Messaging is disabled, as OSAL uses Fifos for messaging - not VDK Messages*/
VDK::ThreadTemplate 
g_OSALThreadTemplate = INIT_THREADTEMPLATE_("OSALThread",VDK::kPriority30,255,OSALThread::Create,(VDK::HeapID)0, (VDK::HeapID)0, false);

