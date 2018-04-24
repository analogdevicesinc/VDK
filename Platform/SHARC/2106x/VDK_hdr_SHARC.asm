// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

 /* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The Run-time Header and interrupt implementations
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 *  This file contains the interrupt table for the ADSP-21060
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef __ADSP21060__
/* removal of a warning about multiple definition of this macro. The assembler
   will define it */
#define __ADSP21060__	1
#endif
#undef  __ADSP21020__


#include <platform_include.h>
#include <VDK_CPP_Names.h>
#include <asm_sprt.h>
#include <asm_offsets.h>

.file_attr Startup;
.file_attr OS_Internals;

.global _abort;
.global _raise;
.global __exit;
.global __Exit;
.extern ___lib_stack_space;
.extern ___lib_stack_length;
.extern ___lib_start; /* from the crt */
.extern __hook_seg_int_all;
.extern ___lib_prog_term;

.extern _main;
.extern _KernelPanic;

///////////////////////////////////////////////////////////////////////
.SECTION/DM	   	seg_dmda;				/* Data used in this source file */
.var __all_ints = __hook_seg_int_all;
.var __invalid_int_save_R0;
.VAR MainStackPtr;
.GLOBAL MainStackPtr;

///////////////////////////////////////////////////////////////////////
.SECTION/PM	   	seg_RSTI;				/* Reset Vector 			*/
.global ___lib_RSTI;
___lib_RSTI:	NOP;						/* Not really executed		*/
				JUMP ___lib_start;
				CALL __InvalidAdr;
				CALL __InvalidAdr;

///////////////////////////////////////////////////////////////////////
/////  Stand alone interrupt service routine implementations
.SECTION/PM seg_pmco;
.global	__InvalidAdr;
// If we get to either of these routines, something is REALLY wrong.
__NotAFunction:
			Jump (PC, 0);			
__InvalidAdr:
			Jump (PC, 0);			

// If we get here via an interrupt, We have unmasked an interrupt without
// defining an interrupt service routine.  Otherwise something is REALLY
// wrong.
.global __InvalidInt;
__InvalidInt:
			push sts;
			dm(__invalid_int_save_R0) = R0;
			R0 = IMASKP;
			R0 = PASS R0;
			IF EQ CALL __InvalidAdr;		/* No, We didn't get here via an interrupt */
			R0 = dm(__invalid_int_save_R0); /* Yes, Re-enable Interrupts and return */
			pop sts;
			RTI (DB);						
		    BIT SET MODE1 IRPTEN;
			NOP;

// If we get here via an interrupt, We have unmasked an interrupt without
// defining an interrupt service routine.  Otherwise something is REALLY
// wrong.
// The IRQ and timer do not need to push the STS and should not have RTI(DB)
.global __InvalidIntIRQ;
__InvalidIntIRQ:
    dm(__invalid_int_save_R0) = R0;
    R0 = IMASKP;
    R0 = PASS R0;
    IF EQ CALL __InvalidAdr;    /* No, We didn't get here via an interrupt */
    R0 = dm(__invalid_int_save_R0); /* Yes, Re-enable Interrupts and return */
    BIT SET MODE1 IRPTEN;
    RTI;

// use the mechanism used in teh RTL to calculate I6 and I7 so we can store
// them and use that stack for the interrupts. The -2 between I6 and I7 are due
// to the arguments to main (that the RTL stores in there)
.global _InitialisePlatform;
_InitialisePlatform:
// we want to store the location of I7 after the call to lib_setup_c so
// we copied this from the RTL.
         PX=PM(___lib_stack_space);
         R0=PX2;
         PX=PM(___lib_stack_length);
         R2=PX2;
         /* stack start + stack length should be even as the length was 
	    retouched in the startup code by the RTL*/
         R1=R0+R2;
         R1=R1-1;
// VDK addition: The RTL then sets up 2 arguments so we keep the space for them
         R0 = -2;
	 R1 = R1 + R0;// two spaces for main arguments 
         DM(MainStackPtr) =R1;
	 I12=DM(M7,I6);
         JUMP(M14,I12)(db);
	 I7=I6;
	 I6=DM(0,I6);

_raise:
_abort:
			R4 = ENUM_kThreadError_;
			R8 = ENUM_kAbort_;
			JUMP (PC,_KernelPanic) (DB); //never comes back
			R12 = 0;
			DM(I7,M7) = PC;

HowDidWeGetHere:
        JUMP HowDidWeGetHere;
__exit:
__Exit:
    JUMP  ___lib_prog_term; /* Jump to program exit address */
.VAR __done_execution = 0;




