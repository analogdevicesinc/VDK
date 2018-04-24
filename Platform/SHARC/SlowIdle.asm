// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This file inplements the 2106x function used to bring the DSP 
 *                out of disabled interrupts one cycle after entering a low 
 *                power idle mode in the IDLE thread.
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

#include <platform_include.h>
#include <ASM_SPRT.h>
#include "ADI_attributes.h"
.file_attr OS_Component=Threads;
.file_attr Threads;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.section/pm seg_pmco;
#endif

.global _SlowIdle;
_SlowIdle:
	leaf_entry;

	// Setting the IRPTEN bit has a one cycle latency, so interrupts should 
	// be enable the cycle AFTER we enter IDLE.
	bit set MODE1 IRPTEN;
	IDLE;

	leaf_exit;	
._SlowIdle.end:
