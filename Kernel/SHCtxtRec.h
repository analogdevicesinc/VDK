/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _SH_CREC_
#define _SH_CREC_

typedef struct SHContext
{
    unsigned PC_STACK_COUNT;
    unsigned LOOP_STACK_COUNT;

    unsigned MR0F;
    unsigned MS0F;
    unsigned MR1F;
    unsigned MS1F;
    unsigned MR2F;
    unsigned MS2F;
    unsigned MR0B;
    unsigned MS0B;
    unsigned MR1B;
    unsigned MS1B;
    unsigned MR2B;
    unsigned MS2B;

    unsigned R0;
    unsigned R1;
    unsigned R2;
    unsigned R3;
    unsigned R4;
    unsigned R5;
    unsigned R6;
    unsigned R7;
    unsigned R8;
    unsigned R9;
    unsigned R10;
    unsigned R11;
    unsigned R12;
    unsigned R13;
    unsigned R14;
    unsigned R15;

#ifdef __2106x__
    unsigned R0_3L;
    unsigned R4_7L;
    unsigned R8_11L;
    unsigned R12_15L;
#else
    unsigned S0;
    unsigned S1;
    unsigned S2;
    unsigned S3;
    unsigned S4;
    unsigned S5;
    unsigned S6;
    unsigned S7;
    unsigned S8;
    unsigned S9;
    unsigned S10;
    unsigned S11;
    unsigned S12;
    unsigned S13;
    unsigned S14;
    unsigned S15;

    unsigned SR0_3L[2];
    unsigned SR4_7L[2];
    unsigned SR8_11L[2];
    unsigned SR12_15L[2];
#endif

    // Need to be double-word aligned here
    unsigned I0;
    unsigned I1;
    unsigned I2;
    unsigned I3;
    unsigned I4;
    unsigned I5;
    unsigned I6;
    unsigned I7;
    unsigned I8;
    unsigned I9;
    unsigned I10;
    unsigned I11;
    unsigned I12;
    unsigned I13;
    unsigned I14;
    unsigned I15;
    unsigned M0;
    unsigned M1;
    unsigned M2;
    unsigned M3;
    unsigned M4;
    unsigned M5;
    unsigned M6;
    unsigned M7;
    unsigned M8;
    unsigned M9;
    unsigned M10;
    unsigned M11;
    unsigned M12;
    unsigned M13;
    unsigned M14;
    unsigned M15;
    unsigned L0;
    unsigned L1;
    unsigned L2;
    unsigned L3;
    unsigned L4;
    unsigned L5;
    unsigned L6;
    unsigned L7;
    unsigned L8;
    unsigned L9;
    unsigned L10;
    unsigned L11;
    unsigned L12;
    unsigned L13;
    unsigned L14;
    unsigned L15;
    unsigned B0;
    unsigned B1;
    unsigned B2;
    unsigned B3;
    unsigned B4;
    unsigned B5;
    unsigned B6;
    unsigned B7;
    unsigned B8;
    unsigned B9;
    unsigned B10;
    unsigned B11;
    unsigned B12;
    unsigned B13;
    unsigned B14;
    unsigned B15;

    unsigned MODE1;
    unsigned PX1;
    unsigned PX2;
    unsigned LCNTR;
    unsigned USTAT1;
    unsigned USTAT2;
#ifdef __2106x__
    unsigned ASTAT;
    unsigned STKY;
#else
    unsigned ASTAT, ASTATy;
    unsigned STKY, STKYy;
    unsigned USTAT3;
    unsigned USTAT4;
#endif                                

#ifdef __214xx__ /* Phoenix only */
    unsigned _BFFWRP;
    unsigned BitFIFO_0, BitFIFO_1;
#endif

/* PC Stack is 30 words */
    unsigned PC_STACK[30];

/* Loop stack is (loop address + loop count) * 6 levels = 12 words */
    unsigned LOOP_STACK[12];

} SHContext;

#endif /* _SH_CREC_ */

