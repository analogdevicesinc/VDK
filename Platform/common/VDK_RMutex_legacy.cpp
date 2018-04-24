/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: Functions that used to be used by RTL in VDSP++ 4.5
 *   These functions should no longer be required but they are left in
 *   in case we are linking in with older libraries
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "VDK_RMutex.h"

#pragma file_attr("OS_Component=Mutex")
#pragma file_attr("Mutex")

namespace VDK
{

void InitMutex(Mutex *pMutex, unsigned size, bool inLogHistory);
void DeInitMutex(Mutex *pMutex);

// the pragma_linkage is required because the name of the class changed
// from RMutex to Mutex. We could have also defined our own class but this
// approach seemed easier

#pragma linkage_name _RMutexInit__3VDKFPQ2_3VDK6RMutexUi
void RMutexInit(Mutex *pMutex, unsigned size)
{
    InitMutex(pMutex,size,false);
}

#pragma linkage_name _RMutexDeInit__3VDKFPQ2_3VDK6RMutex
void RMutexDeInit(Mutex *pMutex)
{
	DeInitMutex(pMutex);
}

#pragma linkage_name _RMutexAcquire__3VDKFPQ2_3VDK6RMutex
void RMutexAcquire(Mutex *pMutex)
{
	AcquireMutex(static_cast<MutexID>(reinterpret_cast<unsigned int>(pMutex)));
}

#pragma linkage_name _RMutexRelease__3VDKFPQ2_3VDK6RMutex
void RMutexRelease(Mutex *pMutex)
{
	ReleaseMutex(static_cast<MutexID>(reinterpret_cast<unsigned int>(pMutex)));
}

}; // namespace VDK
