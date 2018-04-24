/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public API functions
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


#include "VDK_API.h"
#include "BuildNumber.h"
#pragma file_attr("System")


// This file is part of the CORE library and not the main VDK library
namespace VDK
{
    VersionStruct g_VDKVersion={API_VERSION_NUMBER_,VERSION_FAMILY_,VERSION_PRODUCT_,VERSION_BUILD_NUMBER_};

	VersionStruct
	GetVersion( void )
	{
		return g_VDKVersion;		
	}


} // namespace VDK

