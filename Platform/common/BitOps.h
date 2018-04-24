/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _BITOPS_H_
#define _BITOPS_H_

#if defined(__ADSPTS__)
#include <builtins.h>
#elif defined(__ADSP21000__)
#elif !defined(__ADSPBLACKFIN__)
#error Unknown processor family
#endif


namespace VDK {

    enum { kNoneSet = -1 };

inline bool IsBitSet(unsigned field, int bit)
{
    return 0 != ((field) & (1 << (bit)));
}

inline bool IsBitClr(unsigned field, int bit)
{
    return 0 == ((field) & (1 << (bit)));
}

/**
 * Returns the bit number of the first set bit. 
 */
#if defined(__ADSPBLACKFIN__)
inline int FindFirstSet(unsigned field)
{
    return 30 - __builtin_norm_fr1x32(field);
}
#elif defined(__ADSPTS__)
inline int FindFirstSet(unsigned field)
{
    return 30 + __builtin_exp(field);
}
#elif defined(__ADSP21000__)
inline int FindFirstSet(unsigned field)
{
    int ret_val;
    
    asm("%0 = LEFTZ %1;"	// ret_val = # of leading sign bits
	: "=d" (ret_val) : "d" (field));
    return (31 - ret_val);
}
#endif



} // namespace VDK

#endif // _BITOPS_H_
