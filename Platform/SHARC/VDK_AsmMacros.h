/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Defines macros used during a self test build
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

#include <platform_include.h>  

#include <sysreg.h>

#if defined(__ECC__)
namespace VDK
{
    extern void Schedule();
    extern int g_SchedulerRegionCount;
    extern int g_CriticalRegionCount;
	extern unsigned int g_InitialISRMask;
}

#include <TMK.h>

// We need two levels of substitution in order to prevent
// the symbols IRPTEN and SFT2I finding their way into the
// generated assembly code.
//
#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

#ifdef __2116x__
inline int IS_USER_ISR_LEVEL (){
        return (sysreg_read(sysreg_IMASKP) & (~(SFT2I|LPISUMI)) );
}

inline int IS_ISR_LEVEL (){
        return (sysreg_read(sysreg_IMASKP) & ~LPISUMI) ;
}

#else
inline int IS_USER_ISR_LEVEL (){
        return (sysreg_read(sysreg_IMASKP) & ~SFT2I ) ;
}

inline int IS_ISR_LEVEL (){
        return (sysreg_read(sysreg_IMASKP)) ;
}
#endif


/**
 * Stores the current cycle count into a array of ints
 */
#define GET_CYCLE_COUNT_( CYC_CNT_MSW_ADDR_ )	    \
        do                                          \
    {                                               \
                asm( "R2 = EMUCLK2;		    \
                      R1 = EMUCLK;		    \
                      R0 = EMUCLK2;		    \
                      COMP(R0, R2);		    \
		      IF NE R1 = EMUCLK;	    \
                      DM(0,%0) = R0;		    \
                      DM(1,%0) = R1;"		    \
                        : : "w" (CYC_CNT_MSW_ADDR_):"R0", "R1", "R2"); \
        } while (0)

#endif /* __ECC__ */

#endif /* VDK_ASMMACROS_H_ */



