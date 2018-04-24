/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _STACK_MEASURE_
#define _STACK_MEASURE_

#define STACK_FILL_VALUE 0xDEADBEEF

#if (VDK_INSTRUMENTATION_LEVEL_==2)
namespace VDK
{
void FillMemory(unsigned  int *inStartAddr,const unsigned int inMemSize, const unsigned int inFillData);
unsigned int CheckMemory(unsigned int *inStartAddr,unsigned int inMemSize, const unsigned int inFillData);

}      /* namespace */
#endif /* VDK_INSTRUMENTATION_LEVEL_==2 */

#endif /* _STACK_MEASURE_ */
