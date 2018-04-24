// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This file inplements the 219x function used to bring the DSP 
 *					out of disabled interrupts one cycle after entering a low 
 * 					power idle mode in the IDLE thread.
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * ===========================================================================*/

.file_attr OS_Component=Threads;
.file_attr Threads;

.section program;
.align 2;

.global _SlowIdle;
_SlowIdle:

    NOP;
    NOP;
    NOP;
    NOP;
    RTS;
