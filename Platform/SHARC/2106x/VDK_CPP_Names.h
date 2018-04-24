/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/**
 *  Aliases for C++ mangled names for access from C/asm
 *
 */

#ifndef __VDK_CPP_NAMES__
#define __VDK_CPP_NAMES__

#ifndef __cplusplus

#include <VDK_common_CPP_Names.h>

    /* ISR Mask */

#ifdef __ECC__
#define VDK_GetInterruptMask                  (GetInterruptMask__3VDKFv)
#else
#define _VDK_GetInterruptMask                 _GetInterruptMask__3VDKFv
#endif

#ifdef __ECC__
#define VDK_SetInterruptMaskBits              (SetInterruptMaskBits__3VDKFUi)
#else
#define _VDK_SetInterruptMaskBits             _SetInterruptMaskBits__3VDKFUi
#endif

#ifdef __ECC__
#define VDK_ClearInterruptMaskBits            (ClearInterruptMaskBits__3VDKFUi)
#else
#define _VDK_ClearInterruptMaskBits           _ClearInterruptMaskBits__3VDKFUi
#endif

#ifdef __ECC__
#define	 VDK_GetSharcThreadCycleData		(GetSharcThreadCycleData__3VDKF8ThreadIDPUiN32)
#else
#define	_VDK_GetSharcThreadCycleData		_GetSharcThreadCycleData__3VDKF8ThreadIDPUiN32
#endif

#endif /* __cplusplus */

#endif /* __VDK_CPP_NAMES__ */


