// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Semaphore APIs used from the interrupt domain in C
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The Semaphore APIs used from the interrupt domain in C (for SHARC)
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include <VDK_ISR_API.h>
#include <ASM_SPRT.H>
#include "ADI_attributes.h"

.file_attr ISR;
.file_attr OS_Component=Semaphores;
.file_attr Semaphores;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_int_code_sw;
#else
.SECTION/PM seg_int_code;
#endif

.global _C_ISR_PostSemaphore__3VDKF11SemaphoreID;
_C_ISR_PostSemaphore__3VDKF11SemaphoreID:
leaf_entry;
	VDK_ISR_POST_SEMAPHORE_(R4)
leaf_exit;
._C_ISR_PostSemaphore__3VDKF11SemaphoreID.end:



