// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Run-time Header and interrupt implementations
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *  When the C program exits either by returning from main() or by an
 *  explicit or implicit call to exit(), control will transfer to the
 *  label ___lib_prog_term.  Currently, the ___lib_prog_term label is
 *  defined at the end of the reset vector as an IDLE instruction.
 *  If your application needs to perform some operation AFTER the C\C++
 *  program has finished executing, remove the ___lib_prog_term label
 *  from the runtime header, and place it at the beginning of your   
 *  code.
 *
 * ===========================================================================*/

#include <asm_offsets.h>
#include <sys/platform.h>

.file_attr OS_Internals;

    .section program;
    .align 2;

// the exception handler, reschedule interrupt  and global enable bit are set 
// in InitialisePlatform.cpp in C and not here anymore

_raise:
        .global _raise;
        .type _raise,STT_FUNC;
_abort:
        .global _abort;
        .type _abort,STT_FUNC;
        R0.L =ENUM_kThreadError_ & 0xFFFF;
        R0.H =(ENUM_kThreadError_ >> 16) & 0xFFFF;
        R1.L =ENUM_kAbort_ & 0xFFFF;
        R1.H =(ENUM_kAbort_ >> 16) & 0xFFFF;
        R2 = 0;
        CALL.X _KernelPanic;
._abort.end:
._raise.end:

	.section data1;
nullp:
	.align 4;
	.byte4=0;				// no environment pointers
_environ:
	.align 4;
	.byte4=nullp;
	.global _environ;
	.type _environ,STT_OBJECT;


