/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

////////////////////////////////////////////////////////////////////////////
//
//  VdkProjectWnd.cpp
//  CVdkProjectWnd Object Class Implementation File
//
//  The CVdkProjectWnd class implements the VDK element window in the
//	IDDE.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VdkPlatformInfo.h"
#include "StdString.h"
#include <algorithm>
#include <list>
#include <errno.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Constants, Globals and Enumerations
/////////////////////////////////////////////////////////////////////////////

CStdString g_VdspInstallDir="";

extern bool vdkgen_for_rt;


/////////////////////////////////////////////////////////////////////////////
// class CVdkPlatformInfo
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkPlatformInfo::FInit( LPCTSTR szPlatform )
{
	CStdString csFamily,csDir;
	m_csPlatform = szPlatform;
	m_bIsValid = FALSE;

	// find out first which platform we think we are

	if (m_csPlatform.Left(8).CompareNoCase("ADSP-BF5") == 0 || 
		m_csPlatform.Left(7).CompareNoCase("ADSP-DM") == 0 ||
		m_csPlatform.CompareNoCase("AD6531") == 0 ||
		m_csPlatform.CompareNoCase("AD6532") == 0 ||
		m_csPlatform.CompareNoCase("AD6900") == 0 || 
		m_csPlatform.CompareNoCase("AD6901") == 0 || 
		m_csPlatform.CompareNoCase("AD6902") == 0 ||
		m_csPlatform.CompareNoCase("AD6903") == 0 ||
		m_csPlatform.CompareNoCase("AD6904") == 0 ||
		m_csPlatform.CompareNoCase("AD6905") == 0 ||
		m_csPlatform.CompareNoCase("AD6723") == 0 || 
		m_csPlatform.CompareNoCase("MT6906") == 0)  {
		csFamily="Blackfin";
		csDir="Blackfin";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-BF6") == 0) {
		csFamily="Blackfin";
		csDir="BlackfinSPX";
	}
	else if (m_csPlatform.Left(7).CompareNoCase("ADSP-TS") == 0) {
		csFamily="TigerSHARC";
		csDir="TS";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-210") == 0) {
		csFamily="SHARC";
		csDir="21k";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-211") == 0) { 
		csFamily="SHARC";
		csDir = "211xx";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-212") == 0) {
		csFamily="SHARC";
		csDir = "212xx";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-213") == 0) {
		csFamily="SHARC";
		csDir = "213xx";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-214") == 0) {
		csFamily="SHARC";
		csDir = "214xx";
	}
	else if (m_csPlatform.Left(8).CompareNoCase("ADSP-219") == 0 ) {
		csFamily="21xx";
		csDir = "219x";
	}

	// for RT and building the examples we just read the registry 
	if (vdkgen_for_rt) {
			m_csBasePath = g_VdspInstallDir;
			m_csBasePath.ReleaseBuffer();
			m_csIniFilePath  = m_csBasePath + "\\" + csDir +"\\vdk\\" + m_csPlatform + ".ini";

	}
		if (m_csBasePath == "") {
				GetModuleFileName( NULL, szFullPath, MAX_PATH );
				// remove file name (VdkGen.exe)
				PathRemoveFileSpec( szFullPath );
				m_csIniFilePath = szFullPath; // to use as a temporary just now
				m_csIniFilePath +=  "\\" + m_csPlatform + ".ini";
				// remove the \vdk directory
				PathRemoveFileSpec( szFullPath );
				// remove the \platform directory
				PathRemoveFileSpec( szFullPath );
				m_csBasePath = szFullPath;
		}
		else  {
			m_csIniFilePath  = m_csBasePath + "\\" + csDir +"\\vdk\\" + m_csPlatform + ".ini";
			if (!g_suppress_warnings)
			{
				cerr << "VdkGen warning: Install directory specified in the command line" <<endl;
			}
		}


	m_csBasePath.ReleaseBuffer();

	// read NaturalWordSize and MaxUnsignedInt for this platform
	if( !ReadInt("NaturalWordSize", m_nMaxBitWidth) ) 
	{
		cerr << "VdkGen Error: Cannot read platform's word size" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadInt("MaxUnsignedInt", m_nMaxUInt) ) 
	{
		cerr << "VdkGen Error: Cannot read platform's maximum int" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadLong("MaxLong", m_nMaxLong) ) 
	{
		cerr << "VdkGen Error: Cannot read platform's maximum long" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadLong("MinLong", m_nMinLong) )  
	{
		cerr << "VdkGen Error: Cannot read platform's minimum long" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadInt("NumContextAreas", m_nContextAreas) )  
	{
		cerr << "VdkGen Error: Cannot read platform's number of context areas" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadInt("NumThreadStacks", m_nThreadStacks) )    
	{
		cerr << "VdkGen Error: Cannot read platform's number of stacks" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadInt("NumStackSizes", m_nStackSizes) )    
	{
		cerr << "VdkGen Error: Cannot read platform's number of stack sizes" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadString("LinkerFile", m_csLdfPath) )  
	{
		cerr << "VdkGen Error: Cannot read platform's linker file name" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadString("Templates", m_csRelTemplPath) )  
	{
		cerr << "VdkGen Error: Cannot read platform's template directory" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadString("Family", m_csFamily) )   
	{
		cerr << "VdkGen Error: Cannot read platform's family" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}
	if( !ReadString("HeapSupport", m_csHeapSupport) ) 
	{
		cerr << "VdkGen Error: Cannot read platform's heap support" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return (FALSE);
	}

	if( !ReadString("DefaultTimerInterrupt", m_csDefaultTimer))
	{
		cerr << "VdkGen Error: Cannot read platform's default timer" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
	}


	m_csLdfPath = m_csBasePath + "\\" + m_csLdfPath;
	m_csTemplatePath = m_csBasePath + "\\" + m_csRelTemplPath;


	CStdString csValue;

	// read the ISR table for this platform
	if( !ReadString( "Interrupts", csValue ) )
	{
		cerr << "VdkGen Error: Cannot read platform's interrupt list" << endl;
		cerr << "              VDSP Install Dir = "  << (LPCSTR)m_csBasePath <<endl;
		cerr << "              Ini file =" << (LPCSTR) m_csIniFilePath <<endl;
		return( FALSE );
	}

	// ignore spaces
	csValue.Remove( ' ' );

	int nStart = 0, nEnd = csValue.Find( ',' );

	// parse the comma delimited isr names
	while( nEnd != -1 )
	{
		m_vInterrupts.push_back( csValue.Mid(nStart, nEnd-nStart) );
		nStart = nEnd+1;
		nEnd = csValue.Find( ',', nStart );
	}

	// add the last name in the list
	m_vInterrupts.push_back( csValue.Right(csValue.GetLength()-nStart) );

	//we do not give an error for Interrupts2 as it is not there for all processors (only for 
	// ADSP-2116x ADSP-2126x ADSP-2136x and ADSP-2146x at the moment. It indicates the list of interrupts
	// that are in the LMASK
	if( ReadString( "Interrupts2", csValue ) ) {
		CStdString csInterruptName;
		nStart = 0, nEnd = csValue.Find( ',' );
		// read the interrupts extended list
		while( nEnd != -1 )
		{
			csInterruptName = csValue.Mid(nStart, nEnd-nStart);
			csInterruptName.TrimLeft();
			csInterruptName.TrimRight();
			m_vInterrupts2.push_back( csInterruptName );
			nStart = nEnd+1;
			nEnd = csValue.Find( ',', nStart );
		}

		// add the last name in the list
		csInterruptName = csValue.Right(csValue.GetLength()-nStart);
		csInterruptName.TrimLeft();
		csInterruptName.TrimRight();
		m_vInterrupts2.push_back( csInterruptName );
	}

	if (ReadString( "AllowedTimerInterrupts", csValue ) ) {
		CStdString csInterruptName;
		nStart = 0, nEnd = csValue.Find( ',' );
		// read the interrupts extended list
		while( nEnd != -1 )
		{
			csInterruptName = csValue.Mid(nStart, nEnd-nStart);
			csInterruptName.TrimLeft();
			csInterruptName.TrimRight();
			m_vTimerInterrupts.push_back( csInterruptName );
			nStart = nEnd+1;
			nEnd = csValue.Find( ',', nStart );
		}

		// add the last name in the list
		csInterruptName = csValue.Right(csValue.GetLength()-nStart);
		csInterruptName.TrimLeft();
		csInterruptName.TrimRight();
		m_vTimerInterrupts.push_back( csInterruptName );
	}
    	// Set the stack overflow interrupt to the right thing for the platform (only SHARC has it)
	CStdString StackOverflowInterrupt="";
	if (GetFamily() == "SHARC") 	StackOverflowInterrupt = "CB7I";
	SetStackOverflowInterrupt(StackOverflowInterrupt);

	// Set the reschedule interrupt to the right thing for the platform
	CStdString RescheduleInterrupt, RescheduleInterrupt2="";
	if (GetFamily() == "Blackfin") 	RescheduleInterrupt = "EVT_IVG14";
	if (GetFamily() == "TigerSHARC")	{
		if (m_csPlatform == "ADSP-TS101")
			RescheduleInterrupt = "INT_TIMER0L"; 
		else 
			RescheduleInterrupt = "INT_KERNEL";
	}
	// for SHARC we now need two interrupts, SFT3I and SFT2I
	if (GetFamily() == "SHARC")		{
		RescheduleInterrupt = "SFT3I";
		RescheduleInterrupt2 = "SFT2I";
	}
	
	SetRescheduleInterrupt(RescheduleInterrupt);
	SetRescheduleInterrupt2(RescheduleInterrupt2);
	return( m_bIsValid = TRUE );
}

void CVdkPlatformInfo::Reset()
{
	m_bIsValid = FALSE;
	m_nMaxUInt = 0;
	m_nContextAreas = 0;
	m_nThreadStacks = 0;
	m_nStackSizes = 0;
	m_nMaxBitWidth = 0;
	m_csLdfPath.Empty();
	m_csPlatform.Empty();
	m_csBasePath.Empty();
	m_csTemplatePath.Empty();
	m_csIniFilePath.Empty();
	m_vInterrupts.clear();
	m_vInterrupts2.clear();
	m_csFamily.Empty();
	m_csHeapSupport = "true";
	m_bInitConfig = false;
	m_csSiliconRevision = "automatic";
}

void CVdkPlatformInfo::RemoveInterrupt(int i) {
		m_vInterrupts.erase(m_vInterrupts.begin() + i);
}

BOOL CVdkPlatformInfo::ReadString( LPCTSTR szKey, CStdString& csResult )
{
	LPTSTR szValue = csResult.GetBuffer( 512 );

	DWORD dwResult = GetPrivateProfileString( m_csPlatform, szKey, "", szValue, 512, m_csIniFilePath );

	csResult.ReleaseBuffer();

	return( dwResult != 0 );
}

BOOL CVdkPlatformInfo::ReadInt( LPCTSTR szKey, UINT& nResult )
{
	CStdString csValue;

	if( !ReadString(szKey, csValue) )
		return( FALSE );

	nResult =  strtoul( csValue, NULL,10 );

	return( TRUE );
}

BOOL CVdkPlatformInfo::ReadLong( LPCTSTR szKey, LONG& nResult )
{
	CStdString csValue;

	if( !ReadString(szKey, csValue) )
		return( FALSE );

	nResult = atol( csValue );

	return( TRUE );
}

