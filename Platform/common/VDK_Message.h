/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include <VDK_Thread.h>
namespace VDK{
	// New stuff for multiprocessor messaging

        struct MsgThreadEntry 
        {
		ThreadTemplate tplate;
                ThreadID threadId;
                IOID device;
                char *psDevArg;
                RoutingDirection direction;
        } ;
class RThread;
}
