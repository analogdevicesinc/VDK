// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include <platform_include.h>
#include "ADI_attributes.h"
.file_attr ErrorHandling;

#ifdef __SIMDSHARC__
.SWF_OFF;               // Code has been hand-checked for SWF anomaly cases
#endif

.extern _g_KernelPanicCode__3VDK;
.extern _g_KernelPanicError__3VDK;
.extern _g_KernelPanicValue__3VDK;
.extern _g_KernelPanicPC__3VDK;
#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM seg_pmco;
#endif
.global _KernelPanic;

_KernelPanic:
                        // disable interrupts
                        BIT CLR MODE1 IRPTEN;
                        dm(_g_KernelPanicCode__3VDK)=r4;
                        dm(_g_KernelPanicError__3VDK)=r8;
                        dm(_g_KernelPanicValue__3VDK)=r12;
                        r12 = dm(m6,i7);
                        dm(_g_KernelPanicPC__3VDK)=r12;
__KernelPanic:
                        Jump (PC, 0);

