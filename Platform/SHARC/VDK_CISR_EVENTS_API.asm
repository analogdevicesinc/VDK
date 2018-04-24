// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Event APIs used from the interrupt domain in C
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The Event APIs used from the interrupt domain in C (for SHARC)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include <VDK_ISR_API.h>
#include <ASM_SPRT.H>
#include "ADI_attributes.h"

.file_attr ISR;
.file_attr OS_Component=Events;
.file_attr Events;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_int_code_sw;
#else
.SECTION/PM seg_int_code;
#endif

.global _C_ISR_SetEventBit__3VDKF10EventBitID;
_C_ISR_SetEventBit__3VDKF10EventBitID:
leaf_entry;
	VDK_ISR_SET_EVENTBIT_(R4)
leaf_exit;
._C_ISR_SetEventBit__3VDKF10EventBitID.end:

.global _C_ISR_ClearEventBit__3VDKF10EventBitID;

_C_ISR_ClearEventBit__3VDKF10EventBitID:
leaf_entry;
        VDK_ISR_CLEAR_EVENTBIT_(R4)
leaf_exit;
._C_ISR_ClearEventBit__3VDKF10EventBitID.end:


