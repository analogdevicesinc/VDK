/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Module for all the OSAL ISR APIs.
 *
 *
 *   Last modified $Date$
 *     Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments:  
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

#include "osal_vdk.h"

#include <ccblkfn.h>
#include <sys/exception.h>

/* This file only registers interrupts, so there is no need for the module to
 * be prefersMem=internal */
#pragma file_attr("ADI_OS=OSAL")
#pragma file_attr("libGroup=adi_osal.h")
#pragma file_attr("libName=libosal")
#pragma file_attr("OS_Component=ISR")
#pragma file_attr("ISR")


/* File misra suppressions */
#pragma diag(push) 

/* defined in the RTL */
void _unknown_exception_occurred(void);

/* This is an array of function pointers, with the void fn(void) prototype,
 * to be used for the addresses of the OSAL ISRs. */
void (*adi_osal_gIsrArray[num_interrupt_kind])(void) = 
    {NULL, /*ik_emulation*/
     NULL, /*ik_reset*/
     NULL, /*ik_nmi*/
     NULL, /*ik_exception*/
     NULL, /*ik_global_int_enable*/
     NULL, /*ik_hardware_err*/
     NULL, /*ik_timer*/
     NULL, /*ik_ivg7*/
     NULL, /*ik_ivg8*/
     NULL, /*ik_ivg9*/
     NULL, /*ik_ivg10*/
     NULL, /*ik_ivg11*/
     NULL, /*ik_ivg12*/
     NULL, /*ik_ivg13*/
     NULL, /*ik_ivg14*/
     NULL  /*ik_ivg15*/ };


/*****************************************************************************
adi_osal_ISRInstall 

Description
    The adi_osal_ISRInstall function is used to install a user-supplied 
    Interrupt Service Routine into the interrupt vector table for a specified 
    interrupt level.  This is done to ensure that the specified ISR is wrapped 
    with the appropriate prologue and epilogue for the underlying OS, to 
    perform the required context switching for the OS. This only necessary if 
    the ISR is intended to use operating system services.  

Conditions
    If ISRs are installed outside control of the OSAL/OS, then the ISR may not 
    be thread-safe, and without an appropriate level of context switch, 
    corruption of the scheduler or the running threads can occur.
    No ISR can be registered for the following interrupt levels (
    ADI_OSAL_BAD_IVG_LEVEL will result):
IVG0  Emulator Exception
IVG1  Reset
IVG3  Exception
IVG4  Reserved

    The OSAL does not allow more than one Interrupt Service Routine to be 
    mapped to a given level.  Hence it is the responsibility of the user-
    supplied ISR to perform interrupt sharing if this is required. 
    In the context of the user-supplied ISR function pointed by pISRFunc, 
    registers have not been saved and will not be restored. It is the 
    responsibility of the user-supplied ISR to save and restore the register 
    that it uses. 
    If an attempt is made to register the same ISR as already exists for a 
    given level, the OSAL does nothing and simply returns ADI_OSAL_SUCCESS .  
    This function does not affect the state of the interrupt masking register 
    in any way.
    Note: It does NOT change the nesting status under this circumstance. If it 
    is desired to change the nesting status only, then the user must first 
    uninstall the current handler and then re-install it with the nesting flag 
    change.  This is intentional to prevent unintended change to nesting 
    behaviour  normally nesting behaviour will not need to be changed for a 
    given ISR once it is installed.
    This function must not be called from within an ISR.

Arguments
    eIVGLevel   [In]    
        Specifies the interrupt level to which the ISR is to be installed (
        refer to Section 4.3.3.3 for a list of the allowed values).
    pISRFunc    [In]    
        The address of the user-supplied function for the ISR to be installed 
    at the interrupt level specified by eIVGLevel. This must have a prototype 
as defined in Section 4.3.1.1).
    bAllowNesting   [In]    true: Nesting of ISRs at this level is allowed
        false: No nesting allowed.

Return Values
ADI_OSAL_SUCCESS            The specified ISR has been installed successfully.
ADI_OSAL_IVG_LEVEL_IN_USE       The specified interrupt level is already in use.
ADI_OSAL_BAD_IVG_LEVEL  [D] The specified interrupt level is invalid. 
ADI_OSAL_CALLER_ERROR   [D] This function was called from an invalid location (such as an ISR)

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ISRInstall(ADI_OSAL_IVG eIvgLevel, 
                    ADI_OSAL_ISR_PTR pISRFunc, 
                    bool bAllowNesting)
{
    ADI_OSAL_STATUS eResult = ADI_OSAL_SUCCESS;

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* Cannot be called from ISR, but at startup is ok */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }

    /* Check that the given ivg level is valid.
     * Cannot be  =0, =1, =3, =4, >15  */
    if((eIvgLevel < ik_nmi) || (eIvgLevel >= num_interrupt_kind) ||
       (eIvgLevel == ik_exception) || (eIvgLevel == ik_global_int_enable) )
    {
        return ADI_OSAL_BAD_IVG_LEVEL;
    } 
#pragma diag(pop)
#endif /*OSAL_DEBUG*/
  
    /* The isrArray is only modified at thread level, although it is read 
     * at ISR level, so we only need to protect the array from other threads */ 
    VDK_PushUnscheduledRegion();

    if(adi_osal_gIsrArray[eIvgLevel] == NULL)
    {
        adi_osal_gIsrArray[eIvgLevel] = pISRFunc;

        if(bAllowNesting)
        {
            register_handler((interrupt_kind)eIvgLevel, adi_osal_nesteddispatcher);
        }
        else
        {
            register_handler((interrupt_kind)eIvgLevel, adi_osal_dispatcher);
        }  

    }
    else
    {
        /* If the user supplied an ISR that is already in use for that ivg,
         * then we don't do anything and return success.  If another ISR is in
         * place then this is an error */
        if(pISRFunc != adi_osal_gIsrArray[eIvgLevel])
        {
            eResult = ADI_OSAL_IVG_LEVEL_IN_USE;
        }
    }

    /* We have finished with adi_osal_gIsrArray, so release the lock */    
    VDK_PopUnscheduledRegion();

    return eResult;
}

/*****************************************************************************
adi_osal_ISRUninstall

Description
(Advanced User Only  Please refer to the Quick Start for more information)
The adi_osal_ISRUninstall function is used to remove an ISR for the specified interrupt level from the interrupt vector table.  This process returns the interrupt entry for this level to its default value (a function that generates an exception if an interrupt at this level occurs).
Conditions
The entries for the following interrupt levels cannot be altered (ADI_OSAL_BAD_IVG_LEVEL will result):
IVG0  Emulator Exception
IVG1  Reset 
IVG3  Exception
IVG4  Reserved
This function must not be called from within an ISR.
NO-OS
Arguments
eIVGLevel   [In]    Specifies the interrupt level for which the ISR is to be removed.
Return Values
ADI_OSAL_SUCCESS            The specified ISR has been removed successfully.
ADI_OSAL_CALLER_ERROR   [D] This function was called from an invalid location (such as an ISR)
ADI_OSAL_BAD_IVG_LEVEL  [D]     The specified interrupt level is invalid.

*****************************************************************************/
ADI_OSAL_STATUS 
adi_osal_ISRUninstall(ADI_OSAL_IVG eIvgLevel)
{
    ADI_OSAL_STATUS eResult = ADI_OSAL_SUCCESS;
    uint32_t state;  

#if defined(OSAL_DEBUG)

#pragma diag(push)
#pragma diag(suppress:misra_rule_14_7:"Allowing several points of exit for error checking within the OSAL Debug builds improves readability.")

    /* Cannot be called from ISR, but at startup is ok */
    if(InISR())
    {
        return ADI_OSAL_CALLER_ERROR;
    }

    /* Check that the given ivg level is valid 
     * Cannot be  =0, =1, =3, =4, >15  */
    if((eIvgLevel < ik_nmi) || (eIvgLevel >= num_interrupt_kind) ||
       (eIvgLevel == ik_exception) || (eIvgLevel == ik_global_int_enable) )
    {
        return ADI_OSAL_BAD_IVG_LEVEL;
    } 
#pragma diag(pop)
#endif /*OSAL_DEBUG*/
  
     
    VDK_PushUnscheduledRegion();

    if(adi_osal_gIsrArray[eIvgLevel] != NULL)
    {
        /* This should be the default case for when there is no ISR installed,
         * the imask bit is somehow enabled and the interrupt latches. 
         * The interrupt is disabled with this call.*/
         register_handler_ex((interrupt_kind)eIvgLevel, _unknown_exception_occurred, EX_INT_DISABLE); 

        /* We can now remove the OSAL Isr from the array. */
        adi_osal_gIsrArray[eIvgLevel] = NULL;
    }
    else
    {
#if defined(OSAL_DEBUG)
        eResult = ADI_OSAL_BAD_IVG_LEVEL;
#endif /*OSAL_DEBUG*/
    }

    VDK_PopUnscheduledRegion();

    return eResult;
}


/*****************************************************************************
adi_osal_nested_dispatcher

Description
Internal dispatcher for all OSAL nested (reentrant) ISRs.  When a nested ISR 
is installed then this is the handler that is inserted into the EVT.  The 
purpose of the handler is to find which IVG level the code is running on, and
then call the ISR installed for that ivg level.

*****************************************************************************/
EX_REENTRANT_HANDLER(adi_osal_nesteddispatcher)
{
    uint32_t currentIVG;

    /* Get the current value of ipend, ignoring the unused GBLDIS bit */
    uint32_t ipend = *pIPEND & ~(1ul << ik_global_int_enable);

    /* 
     * The following deterministic sequence provides the index of the
     * highest priority set ivg level in the IPEND register.The sequence
     * removes any lower priority bits from the IPEND word, leaving the 
     * highest priority ivg bit as the MSB. A builtin is then used to get
     * the position of that bit.
     * As it is quite tricky to understand, an example of the bit
     * manipulation is given for an example IPEND value.
     * e.g.          IPEND = ivg8, ivg10, ivg11, ivg15 - ivg8 is the highest
     *               IPEND = 0x00008D00
     *                     = 0000 0000 0000 0000 1000 1101 0000 0000 
     *
     * (ipend)             = 0000 0000 0000 0000 1000 1101 0000 0000 
     * (ipend - 1)         = 0000 0000 0000 0000 1000 1100 1111 1111 
     * (ipend - 1) ^ ipend = 0000 0000 0000 0000 0000 0001 1111 1111 */

    ipend = (ipend - 1ul) ^ ipend;

    /* Get the position of the fist set bit. The ISR array is indexed from
     * high priority ivg to low priority ivg, so currentIVG needs to be the 
     * bit position with respect to the LSB of the ipend word. */
    currentIVG = 30ul- (uint32_t)__builtin_norm_fr1x32((fract32)ipend);

    /* Call the ISR */
    (*adi_osal_gIsrArray[currentIVG])();
}


/*****************************************************************************
adi_osal_dispatcher

Description
Internal dispatcher for all OSAL non-nested ISRs.  When a non-nested ISR is 
installed then this is the handler that is inserted into the EVT.  The 
purpose of the handler is to find which IVG level the code is running on, and
then call the ISR installed for that ivg level.

*****************************************************************************/
EX_INTERRUPT_HANDLER(adi_osal_dispatcher)
{
    uint32_t currentIVG;

    /* Get the current value of ipend, ignoring the unused GBLDIS bit */
    uint32_t ipend = *pIPEND & ~(1ul << ik_global_int_enable);

    /* 
     * The following deterministic sequence provides the index of the
     * highest priority set ivg level in the IPEND register.The sequence
     * removes any lower priority bits from the IPEND word, leaving the 
     * highest priority ivg bit as the MSB. A builtin is then used to get
     * the position of that bit.
     * As it is quite tricky to understand, an example of the bit
     * manipulation is given for an example IPEND value.
     * e.g.          IPEND = ivg8, ivg10, ivg11, ivg15 - ivg8 is the highest
     *               IPEND = 0x00008D00
     *                     = 0000 0000 0000 0000 1000 1101 0000 0000 
     *
     * (ipend)             = 0000 0000 0000 0000 1000 1101 0000 0000 
     * (ipend - 1)         = 0000 0000 0000 0000 1000 1100 1111 1111 
     * (ipend - 1) ^ ipend = 0000 0000 0000 0000 0000 0001 1111 1111 */

    ipend = (ipend - 1ul) ^ ipend;

    /* Get the position of the fist set bit. The ISR array is indexed from
     * high priority ivg to low priority ivg, so currentIVG needs to be the 
     * bit position with respect to the LSB of the ipend word. */
    currentIVG = 30ul- (uint32_t)__builtin_norm_fr1x32((fract32)ipend);

    /* Call the ISR */
    (*adi_osal_gIsrArray[currentIVG])();
}



#pragma diag(pop)  /* File Misra suppressions */
