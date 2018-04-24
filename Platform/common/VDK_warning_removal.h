/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* 
 * This file removes the required warnings in compiler generated code where
 * the compiler has more information than the assembler.
 * Under normal circumstances the warnings do not come up - the compiler 
 * disables them - but -anomaly-detect all makes them appear 
 */

/* 
 * Anomaly 05-00-0165 for BF535 
 * "Data Cache Dirty Bit Set when a Load-Miss-Fill Is in Progress"
 */

#if defined (__ADSPBF535__)
asm(".message/suppress 5504;"); 
#endif

