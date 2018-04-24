/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *  Description: This is a C++ header file for Thread RThread
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#ifndef _RThread_H_    
#define _RThread_H_

#include "VDK_Thread.h"
#include "VDK_Message.h"

namespace VDK {

class RThread : public VDK::Thread 
{
public:
    RThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~RThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);

private:
	VDK::MsgThreadEntry *m_pParams;
};

};

#endif /* _RThread_H_ */

/* ========================================================================== */
