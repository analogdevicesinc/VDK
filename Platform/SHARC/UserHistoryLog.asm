// ******************************************************************
//  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
// ******************************************************************

#include "ADI_attributes.h"
.file_attr HistoryAndStatus;
.file_attr ISR;

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.SECTION/SW seg_swco;
#else
.section/pm seg_pmco;
#endif
.global _UserHistoryLog;

_UserHistoryLog:
  RTS;
._UserHistoryLog.end:
