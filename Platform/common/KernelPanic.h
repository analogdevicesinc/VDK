/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "VDK_API.h"

#ifdef __cplusplus
extern "C" void KernelPanic(VDK::PanicCode, VDK::SystemError , const int);
#else
void KernelPanic(VDK_PanicCode, VDK_SystemError , const int);
#endif
