// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include "asm_offsets.h"
.file_attr ErrorHandling;
.file_attr FuncName=adi_stack_overflowed;
.file_attr FuncName=_adi_stack_overflowed;

.section program;
.align 2;
.global _adi_stack_overflowed;
_adi_stack_overflowed:
    R0.L =ENUM_kStackCheckFailure_ & 0xFFFF;
    R0.H =(ENUM_kStackCheckFailure_ >> 16) & 0xFFFF;
    R1.L =ENUM_kThreadStackOverflow_ & 0xFFFF;
    R1.H =(ENUM_kThreadStackOverflow_ >> 16) & 0xFFFF;
    R2 = RETS; 
    JUMP.X _KernelPanic;

._adi_stack_overflowed.end:
