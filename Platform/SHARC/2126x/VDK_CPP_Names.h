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

#ifdef __ECC__
#define	 VDK_GetSharcThreadCycleData		(GetSharcThreadCycleData__3VDKF8ThreadIDPUiN32)
#else
#define	_VDK_GetSharcThreadCycleData		_GetSharcThreadCycleData__3VDKF8ThreadIDPUiN32
#endif

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
#define  VDK_GetInterruptMaskEx             (GetInterruptMaskEx__3VDKFPUiT1)
#else
#define _VDK_GetInterruptMaskEx              _GetInterruptMaskEx__3VDKFPUiT1
#endif

#ifdef __ECC__
#define	 VDK_SetInterruptMaskBitsEx			(SetInterruptMaskBitsEx__3VDKFUiT1)
#else
#define	_VDK_SetInterruptMaskBitsEx			_SetInterruptMaskBits_Ex_3VDKFUiT1
#endif

#ifdef __ECC__
#define	 VDK_ClearInterruptMaskBitsEx		(ClearInterruptMaskBitsEx__3VDKFUiT1)
#else
#define	_VDK_ClearInterruptMaskBitsEx		_ClearInterruptMaskBitsEx__3VDKFUiT1
#endif

#endif /* __cplusplus */

#endif /* __VDK_CPP_NAMES__ */


