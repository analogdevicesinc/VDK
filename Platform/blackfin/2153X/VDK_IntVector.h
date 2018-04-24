/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _VDK_INTVECTOR_H_
#define _VDK_INTVECTOR_H_

#ifdef __ECC__
#include <sys/exception.h>

enum {
seg_EVT_NMI=ik_nmi,
seg_EVT_IVHW=ik_hardware_err,
seg_EVT_IVG7=ik_ivg7,
seg_EVT_IVTMR=ik_timer,
seg_EVT_IVG8=ik_ivg8,
seg_EVT_IVG9=ik_ivg9,
seg_EVT_IVG10=ik_ivg10,
seg_EVT_IVG11=ik_ivg11,
seg_EVT_IVG12=ik_ivg12,
seg_EVT_IVG13=ik_ivg13
};

#define INT_CPP_ROUT(ServiceRoutine) \
EX_INTERRUPT_HANDLER(ServiceRoutine);

#define INT_C_ROUT(ServiceRoutine) \
extern "C" {EX_INTERRUPT_HANDLER(ServiceRoutine);}

#define INT_ASM_ROUT(ServiceRoutine) \
extern "asm" {EX_INTERRUPT_HANDLER(ServiceRoutine);}

#define INTVECTOR(SegIntVector, ServiceRoutine) \
{ register_handler_ex((interrupt_kind)SegIntVector,ServiceRoutine,EX_INT_DISABLE); }

#define INTVECTOR_C(SegIntVector, ServiceRoutine) \
INTVECTOR(SegIntVector, ServiceRoutine)

#define INTVECTOR_CPP(SegIntVector, ServiceRoutine) \
INTVECTOR(SegIntVector, ServiceRoutine)

#endif /* __ECC__ */

#endif /* VDK_INTVECTOR_H */

