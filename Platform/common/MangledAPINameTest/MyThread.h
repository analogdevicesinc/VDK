/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is a C++ header file for Thread MyThread
 *
 *   Created on:  Monday, May 14, 2001 - 02:11:15 PM
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef _MyThread_H_    
#define _MyThread_H_

#include "VDK.h"

class MyThread : public VDK::Thread 
{
public:
    MyThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~MyThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _MyThread_H_ */

