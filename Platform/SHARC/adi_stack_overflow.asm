// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include <platform_include.h>
#include "ADI_attributes.h"
#include "asm_offsets.h"
.extern _KernelPanic;
.file_attr ErrorHandling;

#ifdef __SIMDSHARC__
.SWF_OFF;               // Code has been hand-checked for SWF anomaly cases
#endif

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.SECTION/PM seg_pmco;
#endif
.global _adi_stack_overflowed;

_adi_stack_overflowed:
    R12 = PCSTK;
    R8 = ENUM_kThreadStackOverflow_;
    JUMP (PC,_KernelPanic) (DB);
    R4 = ENUM_kStackCheckFailure_;
    DM(I7,M7) = pc;
._adi_stack_overflowed.end:
