/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _VDK_INTVECTOR_H_
#define _VDK_INTVECTOR_H_

#include <platform_include.h>

#ifndef __ECC__
#undef  __ADSP21020__	 
#define INTVECTOR(IntVectorSeg, ServiceRoutine)\
												\
.SECTION/pm	   	IntVectorSeg;			   	\
.extern ServiceRoutine;					   	\
.extern __InvalidAdr;							\
.global __ ## IntVectorSeg;				   	\
__ ## IntVectorSeg: JUMP ServiceRoutine (DB);  \
				    BIT CLR MODE1 IRPTEN;		\
					NOP;				 		\
					CALL __InvalidAdr; 


#else
#define INT_CPP_ROUT(ServiceRoutine) \
void ServiceRoutine(void);

#define INT_C_ROUT(ServiceRoutine) \
extern "C" void ServiceRoutine(void);

#define INT_ASM_ROUT(ServiceRoutine) \
extern "asm" void ServiceRoutine(void);

#define QUOTED_(X) QUOTED_ARG_(X)
#define QUOTED_ARG_(X) #X
/* The magic number in the BIT CLR instruction is IRPTEN. It is hardcoded
 * until we have a def2116X.h
 */

#define INTVECTOR(IntVectorSeg, ServiceRoutine) \
asm(".SECTION/pm/doubleany " QUOTED_(IntVectorSeg) ";	\
.extern " QUOTED_(ServiceRoutine) ";	\
.extern __InvalidAdr;			\
.global __" QUOTED_(IntVectorSeg) ";	\
__" QUOTED_(IntVectorSeg) ": JUMP " QUOTED_(ServiceRoutine) " (DB);	\
 BIT CLR MODE1 0x00001000;		\
 NOP;					\
 CALL __InvalidAdr;			\
 .PREVIOUS ;");

#define INTVECTOR_BASE(IntVectorSeg, ServiceRoutine) \
asm(".SECTION/pm " QUOTED_(IntVectorSeg) ";	\
.extern " QUOTED_(ServiceRoutine) ";	\
.extern __InvalidAdr;			\
.global __" QUOTED_(IntVectorSeg) ";	\
__" QUOTED_(IntVectorSeg) ": JUMP " QUOTED_(ServiceRoutine) " (DB);	\
 NOP;		\
 NOP;					\
 CALL __InvalidAdr;			\
 .PREVIOUS ;");

#define MANGLED_C_NAME(X) _ ##X
#define MANGLED_CPP_NAME(X) _ ##X## __Fv

#define INTVECTOR_C(IntVectorSeg, ServiceRoutine) \
 INTVECTOR_BASE(IntVectorSeg, MANGLED_C_NAME(ServiceRoutine))

#define INTVECTOR_CPP(IntVectorSeg, ServiceRoutine) \
 INTVECTOR_BASE(IntVectorSeg, MANGLED_CPP_NAME(ServiceRoutine))


#endif /* __ECC__ */

#endif /* VDK_INTVECTOR_H */

