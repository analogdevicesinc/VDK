/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <sys/exception.h>
#pragma file_attr("Startup")

extern "C"{
    EX_INTERRUPT_HANDLER(VDK_Reschedule_ISR__3VDK);
    EX_EXCEPTION_HANDLER(VDK_Exception__3VDK);
    EX_INTERRUPT_HANDLER(_InvalidInt);
    extern void UserDefinedInterruptServiceRoutines__3VDKFv();
    void InitialisePlatform(){
	// we want the reschedule interrupt to be disabled now. IMASK will be
	// changed later. The exception handler and the global enabled we can
	// leave as they were
	UserDefinedInterruptServiceRoutines__3VDKFv();
    }
	
}
