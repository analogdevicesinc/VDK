/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef _RMUTEX_H_
#define _RMUTEX_H_

#include "TMK.h"

#include "VDK_API.h"
#include "IDElement.h"

namespace VDK
{

typedef struct Mutex:public IDElementTemplate<MutexID>
{
#ifdef VDK_API_H_
	TMK_Thread		   *pOwner;
	unsigned			level;
	TMK_ThreadList		pendingThreads;
#ifdef PRIORITY_INHERITANCE
	unsigned       ownerOriginalPriority;
#endif
#else
	unsigned int vDummy[5];
#endif
} Mutex;

void    InitMutex   (Mutex *pMutex, unsigned size,bool);
void    DeInitMutex (Mutex *pMutex);
MutexID CreateMutex (void);
void    DestroyMutex(Mutex *pMutex);
void    AcquireMutex(Mutex *pMutex);
void    ReleaseMutex(Mutex *pMutex);


} // namespace VDK

#if __ADSP21000__ || __ADSPTS__
#define MUTEX_SIGNATURE (static_cast<MutexID>(0x414d5458))
#define RTL_MUTEX_SIGNATURE (static_cast<MutexID>(0x524d5458))
#define DEAD_MUTEX_SIGNATURE (static_cast<MutexID>(0x444d5458))
#else
#define MUTEX_SIGNATURE (static_cast<MutexID>('AMTX'))
#define RTL_MUTEX_SIGNATURE (static_cast<MutexID>('RMTX'))
#define DEAD_MUTEX_SIGNATURE (static_cast<MutexID>('DMTX'))
#endif

#define MTX_TST(pMutex) ( (pMutex != NULL) && ((pMutex)->ID() == MUTEX_SIGNATURE || (pMutex)->ID() == RTL_MUTEX_SIGNATURE))
#define USER_MTX(pMutex) ((pMutex != NULL) && ((pMutex)->ID() == MUTEX_SIGNATURE))


#endif // _RMUTEX_H_
