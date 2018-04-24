/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

///////////////////////////////////////////////////////////////////////////////
//                                     
//  StdAfx.h
//
//	Include file for standard system include files, or project specific 
//	include files that are used frequently, but are changed infrequently.
//   
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STDAFX_H__
#define __STDAFX_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#if _MSC_VER < 1400

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif
#endif

#include <iostream>
#include <shlwapi.h>

#endif // __STDAFX_H__
