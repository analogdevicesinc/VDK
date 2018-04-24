/* 
***************************************************************************
Copyright (c), 2009 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
***************************************************************************

Title:   Common OSAL definitions

         This file contains definitions that are common to all OSAL implementations
         It is suppose to be included in the implementation file. 
         IT IS NOT A PUBLIC FILE.

*****************************************************************************/

#ifndef __OSAL_COMMON_H__
#define __OSAL_COMMON_H__

/*=============  I N C L U D E S   =============*/
#include <osal/adi_osal.h>

/*==============  D E F I N E S  ===============*/


/* "ADI_MAX_NUM_IVG_LEVELS" is used in both asm and C file for declaring the buffer.
   It represents the maximum number of Interrupt Vector Levels on the Hardware which 
   is 16 on the Blackfin processor. */    
#define     ADI_MAX_NUM_IVG_LEVELS          (16)



#if defined(__GNUC__) || defined(_LANGUAGE_C)
/* C language specific macros and declarations*/


/* Maximum number of thread slots available. DO NOT CHANGE THIS VALUE without changing
   the code because the assumption is that we can bit-map these slots. */
#define     ADI_OSAL_MAX_THREAD_SLOTS       (31u)

/* number of microseconds per second */
#define     USEC_PER_SEC                    (1000000u)

/* The highest bit in the event groups is reserved by VDK */
#define     ADI_RESERVED_EVENT_BIT         0x80000000

/* 
    Maximum value for a 16-bit variable
*/
#define OSAL_MAX_UINT16                 (0xFFFFu)    



/*  Structure:  ADI_OSAL_ISR_INFO
    This structure is defined to store the ISR information such as the function to be
    called after storing the processor context.
*/
typedef struct __AdiOsalIsrInfo
{
    /* Variable : pIsrFunction   
       Function pointer to actual ISR. This will get initialized
       during the registration of ISR
       */
    ADI_OSAL_ISR_PTR pIsrFunction;

} ADI_OSAL_ISR_INFO, *ADI_OSAL_ISR_INFO_PTR;


#else
/* assembly language specific macros and declarations*/


#endif  /* if !defined(__GNUC__) && !defined(_LANGUAGE_C) */


#endif /*__OSAL_COMMON_H__ */


