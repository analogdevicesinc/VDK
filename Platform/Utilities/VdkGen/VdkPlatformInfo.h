/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

////////////////////////////////////////////////////////////////////////////
//                                     
//  VdkPlatform.h
//  CVdkPlatform Object Class Definition File
//   
//  The CVdkPlatform class represents the DSP information necessary obtained
//  from ini file
// 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VDKPLATFORM_H__
#define __VDKPLATFORM_H__

#include <vector>
#include "StdString.h"

#ifndef MAINWIN
using namespace std;
#endif

class CVdkConfiguration;
extern CVdkConfiguration* g_VdkData;
extern bool g_suppress_warnings;
extern bool error_found;
extern char szFullPath[];
extern CStdString VdkGenVersion;
extern CStdString VdkGenCompany;
extern CStdString VdkGenCopyright;

#define VDKGEN_ERROR_NO_FILE(str)	\
		{	error_found = true; \
			cerr << "VdkGen Error: " << str << endl; \
		}
	
#define VDKGEN_ERROR(nstr) \
	{	if (!m_bErrorDumped && m_ImportedConfiguration) \
			{ cerr << "Processing imported file "<<(LPCSTR) m_csFileName << endl;} \
		m_bErrorDumped=true;  \
		VDKGEN_ERROR_NO_FILE(nstr); \
	}
	

///////////////////////////////////////////////////////////////////////////////
// constants and enumerations
///////////////////////////////////////////////////////////////////////////////


#define VDK_STRINGIZE(L)	#L
#define VDK_MAKESTRING(M,L)	M(L)
#define VDK_LINE			VDK_MAKESTRING(VDK_STRINGIZE,__LINE__)
#define VDK_NOTE(X)			message(__FILE__"("VDK_LINE") : VDK NOTE: " #X)
#define VDK_TODO(X)			message(__FILE__"("VDK_LINE") : VDK TODO: " #X)


///////////////////////////////////////////////////////////////////////////////
// class CVdkPlatformInfo
//
// This class encapsulates the processor specific information that is needed
// by the VDK such as bit width, ISR names, etc...  This information is parsed
// on a per-processor basis from the ini file  which must be in the same
// directory as the VDkGen executable.
//
///////////////////////////////////////////////////////////////////////////////

class CVdkPlatformInfo
{

public:

	CVdkPlatformInfo()
	{
		Reset();
	}

	CVdkPlatformInfo(CStdString inVDSPVersion) {
		SetVDSPVersion(inVDSPVersion);
	}

	BOOL FInit( LPCTSTR );
	void Reset();
	BOOL IsValid()				{ return( m_bIsValid ); }
	UINT GetMaxUInt()		    { return( m_nMaxUInt ); }
	LONG GetMinLong()			{ return( m_nMinLong ); }
	LONG GetMaxLong()			{ return( m_nMaxLong ); }
	UINT GetNumContextAreas()	{ return( m_nContextAreas ); }
	UINT GetNumThreadStacks()	{ return( m_nThreadStacks ); }
	UINT GetNumStackSizes()	{ return( m_nStackSizes ); }
	UINT GetMaxBitWidth()	    { return( m_nMaxBitWidth ); }
	UINT GetNumInterrupts()	    { return( m_vInterrupts.size()); }
	CStdString GetInterrupt(int i) { return( m_vInterrupts[i] ); }
	UINT GetNumTimerInterrupts()	{ return( m_vTimerInterrupts.size()); }
	CStdString GetTimerInterrupt(int i) { return( m_vTimerInterrupts[i] ); }


	BOOL FindInInterrupt2(CStdString str) {
		for (unsigned int i=0;i< m_vInterrupts2.size(); i++)
		{
			if (m_vInterrupts2[i] == str)
				return (TRUE);
		}
		return (FALSE);
	}
	BOOL FindInTimerInterrupt(CStdString str) {
		for (unsigned int i=0;i< m_vTimerInterrupts.size(); i++)
		{
			if (m_vTimerInterrupts[i] == str)
				return (TRUE);
		}
		return (FALSE);
	}
	CStdString GetPlatform()		{ return( m_csPlatform ); }
	CStdString GetBasePath()		{ return( m_csBasePath ); }
	CStdString GetDefaultTimer()   { return( m_csDefaultTimer);}
	CStdString GetRescheduleInterrupt()   { return( m_csRescheduleInterrupt);}
	CStdString GetRescheduleInterrupt2()  { return( m_csRescheduleInterrupt2);}
	CStdString GetStackOverflowInterrupt()  { return( m_csStackOverflowInterrupt);}
	CStdString GetTemplatePath()	{ return( m_csTemplatePath ); }
	CStdString GetRelativeTemplatePath()	{ return( m_csRelTemplPath ); }
	CStdString GetLinkerFile()		{ return( m_csLdfPath ); }
	CStdString GetFamily()			{ return( m_csFamily); }
	CStdString GetHeapSupport()	{ return( m_csHeapSupport);}
	CStdString GetVDSPVersion()	{ return( m_csVDSPVersion);}
	CStdString GetSiliconRevision() {return(m_csSiliconRevision);}
	void SetVDSPVersion(CStdString inVersion) { m_csVDSPVersion = inVersion;}
	void SetInitConfig(bool inInitial_config) {m_bInitConfig = inInitial_config;}
	void SetSiliconRevision(CStdString inSiRevision) { m_csSiliconRevision = inSiRevision;}
	void SetRescheduleInterrupt(CStdString inInterrupt)   { m_csRescheduleInterrupt = inInterrupt;}
	void SetRescheduleInterrupt2(CStdString inInterrupt)   { m_csRescheduleInterrupt2 = inInterrupt;}
	void SetStackOverflowInterrupt(CStdString inInterrupt)  { m_csStackOverflowInterrupt = inInterrupt;}
	bool IsInitConfig() { return  (m_bInitConfig);}

	void RemoveInterrupt(int i);
	vector<CStdString> m_vInterrupts;
	vector<CStdString> m_vInterrupts2;
	vector<CStdString> m_vTimerInterrupts;
protected:

	BOOL ReadInt( LPCTSTR, UINT& );
	BOOL ReadLong( LPCTSTR, LONG& );
	BOOL ReadString( LPCTSTR, CStdString& );

protected:

	BOOL m_bIsValid;
	UINT m_nMaxUInt;
	LONG m_nMaxLong;
	LONG m_nMinLong;
	UINT m_nContextAreas;
	UINT m_nThreadStacks;
	UINT m_nStackSizes;
	UINT m_nMaxBitWidth;
	CStdString m_csLdfPath;
	CStdString m_csPlatform;
	CStdString m_csDefaultTimer;
	CStdString m_csRescheduleInterrupt;
	CStdString m_csRescheduleInterrupt2;
	CStdString m_csStackOverflowInterrupt;
	CStdString m_csBasePath;
	CStdString m_csTemplatePath;
	CStdString m_csRelTemplPath;
	CStdString m_csIniFilePath;
	CStdString m_csFamily;
	CStdString m_csHeapSupport;
	CStdString m_csVDSPVersion;
	CStdString m_csSiliconRevision;
	bool m_bInitConfig;

};

#endif // __VDKPLATFORM_H__
