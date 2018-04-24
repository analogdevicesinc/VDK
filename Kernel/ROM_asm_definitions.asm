// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

/* =============================================================================
 *
 *  Description: These are the assembly external definitions required for the 
 *  delta ROM.
 *  For the creation of the ROM image they need to be a dummy functions of the 
 *  same size as the result function will be in the library that defines them.
 *
 * ===========================================================================*/

#if !defined (BUILD_ROM_IMAGE) && !defined (BUILD_ROM_DEFINITION)
#error "This object should only be built when building a ROM image or a ROM definition"
#endif

.file_attr prefersMemNum="30";
.file_attr prefersMem="internal";
.file_attr ISR;

.section/doubleany L1_code;

#if defined (BUILD_ROM_DEFINITION)
.align 2; 

.EXTERN UserExceptionHandler
/******************************************************************************
 * _tmk_UserExceptionHandler
 * we cannot use jump.x because we don't return and we cannot risk corrupting
 * P1 
 */

.GLOBAL     _tmk_UserExceptionHandler;
_tmk_UserExceptionHandler:
    P5 = [SP++];
    JUMP.L UserExceptionHandler;
._tmk_UserExceptionHandler.end:

#endif
