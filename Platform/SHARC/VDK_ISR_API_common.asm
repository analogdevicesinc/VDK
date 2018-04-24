// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for source code formatting
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#include "asm_offsets.h"
#include "ADI_attributes.h"
.file_attr ISR;


/****************************************************************************/
.section/dm        internal_memory_data;
.var    _ISR_save_MODE1__3VDK;
.var    _ISR_save_ASTAT__3VDK;
.var    _ISR_save_I2__3VDK;
.var    _ISR_save_I8__3VDK;
.var    _ISR_save_R0__3VDK;
.var    _ISR_save_R1__3VDK;

.global _ISR_save_MODE1__3VDK;
.global _ISR_save_ASTAT__3VDK;
.global _ISR_save_I2__3VDK;
.global _ISR_save_I8__3VDK;
.global _ISR_save_R0__3VDK;
.global _ISR_save_R1__3VDK;

#if VDK_INSTRUMENTATION_LEVEL_ == 2
// Used for the internal history logging macro
.var VDK_INTERNAL_HISTORY_SV_PCTOP;
.var VDK_INTERNAL_HISTORY_SV_R1;
.var VDK_INTERNAL_HISTORY_SV_R4;
.var VDK_INTERNAL_HISTORY_SV_R8L;
.var VDK_INTERNAL_HISTORY_SV_R8H;
.var VDK_INTERNAL_HISTORY_SV_PX1;
.var VDK_INTERNAL_HISTORY_SV_PX2;

.global VDK_INTERNAL_HISTORY_SV_PCTOP;
.global VDK_INTERNAL_HISTORY_SV_R1;
.global VDK_INTERNAL_HISTORY_SV_R4;
.global VDK_INTERNAL_HISTORY_SV_R8L;
.global VDK_INTERNAL_HISTORY_SV_R8H;
.global VDK_INTERNAL_HISTORY_SV_PX1;
.global VDK_INTERNAL_HISTORY_SV_PX2;
#endif

.endseg;

