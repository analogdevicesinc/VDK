/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Defines macros that must be written in assembly
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: Provides Processor specific macro implementations for enabling/disabling
 *            Interrupts, setting a label, and switching C runtime stacks.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#ifndef VDK_ASMMACROS_H_
#define VDK_ASMMACROS_H_

#include <sys/platform.h>
#if defined(__ECC__)
#include <ccblkfn.h>
#include <cdefBF53X.h>
#endif

#define VDK_EXCEPTION_NUM_ 0

#if defined(__ECC__)
#include <TMK.h>

#undef pIPEND
#define pIPEND ((volatile unsigned long *)IPEND)

inline int IS_USER_ISR_LEVEL (){
        return (*pIPEND & ~( (1<<EVT_IRPTEN_P) | (1<< EVT_IVG15_P) | (1 << VDK_INT_P_))) ;
}

inline int IS_ISR_LEVEL (){
        return (*pIPEND & ~( (1<<EVT_IRPTEN_P) | (1<< EVT_IVG15_P) )) ;
}

//  CYCLES and CYCLES2 each having 32 bits. So the passed variable is assumed
//  to occupy two locations lower 32 bit  will get CYCLES2 and upper 32 bit
//  will get CYCLES
//  
#define GET_CYCLE_COUNT_( CYC_CNT_MSW_ADDR_ )                           \
	do                                                                   \
	{                                                                    \
   	   asm("P0 = %0;"                                                \
		       "R0 = CYCLES;"                                            \
	           "[P0 + 4] =  R0;"                                         \
	           "R0 = CYCLES2;"                                           \
	           "[P0] = R0;" : :  "d" (CYC_CNT_MSW_ADDR_) : "P0","R0");   \
	} while(0)

#endif /* __ECC__ */

#endif /* VDK_ASMMACROS_H_ */



