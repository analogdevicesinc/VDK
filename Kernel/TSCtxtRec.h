/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _TS_CREC_
#define _TS_CREC_

typedef struct TSContext
{
// Comp unit data registers
    __builtin_quad XYR3_0;
    __builtin_quad XYR7_4;
    __builtin_quad XYR11_8;
    __builtin_quad XYR15_12;
    __builtin_quad XYR19_16;
    __builtin_quad XYR23_20;
    __builtin_quad XYR27_24;
    __builtin_quad XYR31_28;

// J and K IALU Registers
    __builtin_quad JK3_0;
    __builtin_quad JK7_4;
    __builtin_quad JK11_8;
    __builtin_quad JK15_12;
    __builtin_quad JK19_16;
    __builtin_quad JK23_20;
    __builtin_quad JK27_24;
    __builtin_quad JK31_28;

// Circular Buffer Registers
    __builtin_quad B3_0;
    __builtin_quad L3_0;

// Data Alignment buffer
    __builtin_quad DAB;

// Multiply result registers
    __builtin_quad MR3_0;

#ifdef __ADSPTS101__
// The Parallel Results "TH" registers
    __builtin_quad PR1_0_THR1_0;

// "Enhanced Communication" Registers (a.k.a. Viterbi/Trellis accellerators)
    __builtin_quad TR3_0;
    __builtin_quad TR7_4;
    __builtin_quad TR11_8;
    __builtin_quad TR15_12;

// IMASKH, for anomaly 03000268 workaround
  unsigned IMASK; // IMASKH in J context, IMASKL (if used) in K context

#else  // TS20x
// The Parallel Results and Bit FIFO Overflow registers
    __builtin_quad PR1_0_BFOTMP;

// "Enhanced Communication" Registers (a.k.a. Viterbi/Trellis accellerators)
    __builtin_quad THR3_0;
    __builtin_quad TR3_0;
    __builtin_quad TR7_4;
    __builtin_quad TR11_8;
    __builtin_quad TR15_12;
    __builtin_quad TR19_16;
    __builtin_quad TR23_20;
    __builtin_quad TR27_24;
    __builtin_quad TR31_28;

// Communication Control register
    unsigned CMCTL;
#endif

// Another multiply result register
    unsigned MR4;

// Zero-overhead Loop registers
    unsigned LC;

// X/Y arithmetic status
    unsigned STAT;

  //
  // The registers above are all in J/K or X/Y pairs and hence are
  // used in both the J and K contexts. The registers below are not
  // in pairs on the hardware and so these save slots are only used
  // in the J context.
  //

// The status flag register
    unsigned SFREG;

// Interrupt return address
    unsigned RETI;

// Function return (or computed jump) address
    unsigned CJMP;
} TSContext;

#endif /* _TS_CREC_ */

