// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

.file_attr ErrorHandling;

    .section program;
    .align 2;
    .extern _KernelPanic;
    .global _ExceptionPanic;

_ExceptionPanic:
    R3 = RETI;
    RETS = R3;
    JUMP.X _KernelPanic;
._ExceptionPanic.end:

