// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

.file_attr ErrorHandling;
.file_attr FuncName=_KernelPanic;
.file_attr FuncName=__KernelPanic;

.extern _g_KernelPanicCode__3VDK;
.extern _g_KernelPanicError__3VDK;
.extern _g_KernelPanicValue__3VDK;
.extern _g_KernelPanicPC__3VDK;
    .section program;
        .align 2;
    .global _KernelPanic;
_KernelPanic:
     CLI R4;
     I1.L = _g_KernelPanicCode__3VDK ; I1.H = _g_KernelPanicCode__3VDK ;
     [I1] = R0;
     I0.L = _g_KernelPanicError__3VDK; I0.H = _g_KernelPanicError__3VDK;
     [I0] = R1;
     I2.L = _g_KernelPanicValue__3VDK; I2.H = _g_KernelPanicValue__3VDK;
     [I2] = R2;
     I3.L = _g_KernelPanicPC__3VDK; I3.H = _g_KernelPanicPC__3VDK;
     R0 = RETS;
     [I3] = R0;
__KernelPanic:
    JUMP __KernelPanic;
._KernelPanic.end:
