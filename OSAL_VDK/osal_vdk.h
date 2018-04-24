/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Contains the VDK definitions required for building the OSAL 
 *   libs for a VDK-based project.  
 *
 *
 *   Last modified $Date$
 *   Last modified by $Author$
 *   $Revision$
 *   $Source$
 *
 * -----------------------------------------------------------------------------
 *  Comments: This file should be revised with any modification to VDK.h/.cpp
 * -----------------------------------------------------------------------------
 *  Copyright (c) 2009 Analog Devices, Inc.,  All rights reserved
 *
 *  This software is the property of Analog Devices, Inc.
 *
 *  ANALOG DEVICES (ADI) MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE.  IN NO EVENT SHALL ADI BE LIABLE FOR INCIDENTAL
 *  OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING,
 *  PERFORMANCE, OR USE OF THIS SOFTWARE.
 * ===========================================================================*/


#ifndef OSAL_VDK_H_    
#define OSAL_VDK_H_

#ifdef __ADSPBLACKFIN__
#include <blackfin.h>
#else
#error OSAL on VDK is only supported for Blackfin processors
#endif   /* __ADSPBLACKFIN__ */

#include "osal_common.h" 

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress: misra_rule_19_1:"VDK needs the enum type definitions for the particular project before including VDK_Public.h")
#endif


/**
 * The following enums should be defined before including VDK_Public.h.
 * For building the OSAL libs they need to be defined, but their values
 * are never used.
 */
 
#ifdef __ECC__	/* for C/C++ access */
enum ThreadType         
{
	kDynamicThreadType = -1
};
enum ThreadID           {kA};
enum SemaphoreID        {kB};
enum PoolID             {kC};
enum EventID            {kD};
enum EventBitID         {kE};
enum IOID               {kF};
enum IOTemplateID       {kG};
enum DeviceFlagID       {kH};
enum MessageID          {kI};
enum MutexID            {kJ};
enum FifoID             {kK};
enum HeapID             {kL};
enum RoutingNodeID      {kM};
enum MarshalledTypeID   {kN};
enum EventGroupID       {kO};

/* We need to suppress all misra rules when including VDK_Public.h as it is
 * not misra complient */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all)
#endif

/* include the VisualDSP Kernel API and library */
#include <VDK_Public.h>

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

/* Common internal definitions for OSAL on VDK */

/* Used in all Pend APIs and adi_osal_ThreadSleep */
#define VDK_MAX_TIMEOUT ((unsigned int)INT_MAX)
#define VDK_SEMAPHORE_MAX_COUNT (UINT_MAX - 1u)
#define ADI_OSAL_PATCH_VER 0u

/*  OSAL on VDK internal inline functions. Not part of th OSAL API. */

#pragma pure
#pragma inline
#pragma always_inline
bool InISR(void);

#pragma diag(push)
#pragma diag(suppress: misra_rule_8_5:"Allow function definitions in headers")

bool InISR(void) 
{
	/* If any bits other than the "Global interrupt enable" and 
	 * "supervisor mode" are set in IPEND then we're at an ISR level.
	 */ 
	return (0ul != (*pIPEND & ~( (1ul<<EVT_IRPTEN_P) | (1ul<< EVT_IVG15_P) )));
}

#pragma diag(pop)


/* ISR dispatchers - nested and non-nested */
void adi_osal_nesteddispatcher(void);
void adi_osal_dispatcher(void);

typedef struct 
{
	    ADI_OSAL_THREAD_PTR pfRun; 
        void *pParam;
} OSALRunData; 

#endif /* __ECC__ */

#endif /* OSAL_VDK_H_ */



/* ========================================================================== */
