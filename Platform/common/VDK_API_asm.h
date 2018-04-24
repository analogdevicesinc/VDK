/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef VDK_API_ASM_H_
#define VDK_API_ASM_H_
#ifndef __ECC__

/** History enumeration values **/

#define		_VDK_kThreadCreated         0x80000000 
#define		_VDK_kThreadDestroyed       0x80000001 
#define		_VDK_kSemaphorePosted       0x80000002 
#define		_VDK_kSemaphorePended       0x80000003 
#define		_VDK_kEventBitSet           0x80000004 
#define		_VDK_kEventBitCleared       0x80000005
#define		_VDK_kEventPended           0x80000006 
#define		_VDK_kDeviceFlagPended      0x80000007
#define		_VDK_kDeviceFlagPosted      0x80000008
#define		_VDK_kDeviceActivated       0x80000009
#define		_VDK_kThreadTimedOut        0x8000000a 
#define		_VDK_kThreadStatusChange    0x8000000b
#define		_VDK_kThreadSwitched        0x8000000c
#define		_VDK_kMaxStackUsed          0x8000000d
#define		_VDK_kPoolCreated           0x8000000e
#define		_VDK_kPoolDestroyed         0x8000000f
#define		_VDK_kDeviceFlagCreated     0x80000010
#define		_VDK_kDeviceFlagDestroyed   0x80000011
#define		_VDK_kMessagePosted         0x80000012
#define		_VDK_kMessagePended         0x80000013
#define		_VDK_kSemaphoreCreated      0x80000014
#define		_VDK_kSemaphoreDestroyed    0x80000015
#define		_VDK_kMessageCreated        0x80000016
#define		_VDK_kMessageDestroyed      0x80000017
#define		_VDK_kMessageTakenFromQueue 0x80000018
#define		_VDK_kThreadResourcesFreed  0x80000019
#define		_VDK_kThreadPriorityChanged 0x8000001a
#define		_VDK_kMutexCreated          0x8000001b
#define		_VDK_kMutexDestroyed        0x8000001c
#define		_VDK_kMutexAcquired         0x8000001d
#define		_VDK_kMutexReleased         0x8000001e
#define		_VDK_kUserEvent             0x1

/** End of history enumeration values **/

/** Declaration of history related APIs **/

#if (VDK_INSTRUMENTATION_LEVEL_==2)
.extern         VDK_ISRAddHistoryEvent;
.extern         _VDK_ISRAddHistoryEvent;
#endif

/** End of declaration of history related APIs **/

/** System Errors dispatched from user code written in assembly **/
#define _VDK_kUncaughtException_ (-2147483597)
#define _VDK_kUnhandledExceptionError_ (-2147483565)
#define _VDK_kUnhandledException_ (7)
/** End of System Errors dispatched from user code written in assembly **/


#endif /* __ECC__ */

#endif /* VDK_API_ASM_H_ */
