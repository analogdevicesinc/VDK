// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include <platform_include.h>

#if defined (__ADSP21369__)
// by mistake we did not include SPERRI in the original release of the
// processor so now we have to include it in seg_pmco in case the LDF does not
// map the correct segment
// If the interrupt is meant to be used then the LDF must have the
// segment seg_SPERRI in the LDF

.SECTION/PM     seg_pmco;
.extern __InvalidInt;
.extern __InvalidAdr;
.global __seg_SPERRI;
__seg_SPERRI: JUMP __InvalidInt (DB);
                    BIT CLR MODE1 IRPTEN;
                    NOP;
                    CALL __InvalidAdr;
.__seg_SPERRI.end:



#endif
