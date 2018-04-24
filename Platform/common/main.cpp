/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The main entry point
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The entry point for the entire VDK framework. 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "VDK_API.h"
#pragma file_attr("Startup")

/*
 * main() has been moved to its own module so that it can be replaced by
 * user-written versions.
 */
int main()
{
	VDK::Initialize();
	VDK::Run();
}

