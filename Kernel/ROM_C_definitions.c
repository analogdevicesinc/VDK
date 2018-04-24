/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/


/* =============================================================================
 *
 *  Description: These are the C external definitions required for the 
 *  delta ROM.
 *  For the creation of the ROM image they need to be a dummy function/variable
 *  of the same size as the result function will be in the library that defines 
 *  them.
 *
 * ===========================================================================*/

#if !defined (BUILD_ROM_IMAGE) && !defined (BUILD_ROM_DEFINITION)
#error "This object should only be built when building a ROM image or a ROM definition"
#endif

#ifdef BUILD_ROM_IMAGE

int    *tmk_SystemStackEnd=0;

#else /* BUILD_ROM_IMAGE */
#if OLD_ROM_TOOL 
/* The old ROM tool does not output this symbol with the right value and we 
 * need to do a resolve ourselves 
 */

/* we need the address of the end of the stack -12. The variable is a pointer
 * so pointer arithmetic will multiply the 3 by the size of pointer (4)
 */

#pragma diag(push)
#pragma diag(suppress:170:"we need to go back in the address and the compiler warns that we are pointing outside the object")
extern "asm" int ldf_stack_end[];
int    *tmk_SystemStackEnd=(ldf_stack_end -3);
#pragma diag(pop)
#endif /* OLD_ROM_TOOL */

#endif /* BUILD_ROM_IMAGE */

/******************************************************************************
 * tmk_pUserExceptionHandler
 * we don't want to jump to a fixed address in RAM so, instead, we load in a
 * variable the address that we need to go to and we use it
 * P1
 */

#ifdef BUILD_ROM_IMAGE

void (*tmk_pUserExceptionHandler)(void) = 0; 

#else /* BUILD_ROM_IMAGE */

#if OLD_ROM_TOOL 
/* The old ROM tool does not output this symbol with the right value and we 
 * need to do a resolve ourselves 
 */


extern void tmk_UserExceptionHandler(void);
void (*tmk_pUserExceptionHandler) (void) = tmk_UserExceptionHandler;

#endif /* OLD_ROM_TOOL */

#endif /* BUILD_ROM_IMAGE */
