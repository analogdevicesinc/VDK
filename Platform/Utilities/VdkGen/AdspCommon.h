/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#ifndef __ADSPCOMMON_H__
#define __ADSPCOMMON_H__

// ADSPCOMMON_LOCAL_MACHINE_VALUE	- preference is stored on a per-machine basis
// ADSPCOMMON_USER_VALUE			- preference is stored on a per-user basis
typedef enum { ADSPCOMMON_LOCAL_MACHINE_VALUE, ADSPCOMMON_USER_VALUE } ADSPCOMMON_LOCATION;

// ADSPCOMMON_TYPE_UNKNOWN			- preference's type is not supported by the ADspCommon library
// ADSPCOMMON_TYPE_BINARY			- preference's type is a list of bytes
// ADSPCOMMON_TYPE_DWORD			- preference's type is DWORD
// ADSPCOMMON_TYPE_STRING			- preference's type is a string
typedef enum { ADSPCOMMON_TYPE_UNKNOWN, ADSPCOMMON_TYPE_BINARY,
			   ADSPCOMMON_TYPE_DWORD, ADSPCOMMON_TYPE_STRING } ADSPCOMMON_TYPE;

// component categories
#define ADSPCOMMON_DEBUG_TARGET_CATEGORY _T("Debug Targets")
#define ADSPCOMMON_PROC_DLL_CATEGORY _T("Processor DLLs")
#define ADSPCOMMON_SYM_MGR_CATEGORY _T("Symbol Managers")
#define ADSPCOMMON_COMPONENTS _T("Components")

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ADSPCOMMON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TEST_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef _ADSPCOMMON_DLL
	#ifdef ADSPCOMMON_EXPORTS
		#define ADSPCOMMON_DECLSPEC __declspec(dllexport)
	#else
		#define ADSPCOMMON_DECLSPEC __declspec(dllimport)
	#endif
#else
	#define ADSPCOMMON_DECLSPEC
#endif

// if ADspCommon is built using C++ linkage declare all API functions as extern "C"

#ifdef __cplusplus
	#define ADSPCOMMON_API extern "C" ADSPCOMMON_DECLSPEC
#else
	#define ADSPCOMMON_API ADSPCOMMON_DECLSPEC
#endif

typedef struct {
	TCHAR *pszPluginName;
	TCHAR *pszCLSID;
	BOOL bAllowMultipleInstances;
	TCHAR *pszDebugProcessorsSupported;
	TCHAR *pszDescription;
	BOOL bEnabled;
	TCHAR *pszFileExtensions;
	TCHAR *pszMenuDescription;
	TCHAR *pszMenuHelpPath;
	TCHAR *pszMenuItem;
	TCHAR *pszProjectProcessorsSupported;
	WORD  wStartupStyle;
	TCHAR *pszWindowHelpPath;
	WORD wWindowType;
} ADSPCOMMON_PLUGININFO;

// high-level API functions
ADSPCOMMON_API BOOL ADspCommonInitialize( const HINSTANCE hInstance, LPCTSTR pszSystemDirectory );
ADSPCOMMON_API void ADspCommonUninitialize();
ADSPCOMMON_API BOOL ADspGetHelpPath( TCHAR *pszHelpPath, WORD *pwSize );
ADSPCOMMON_API BOOL ADspGetInstallPath ( TCHAR *pszInstallPath, WORD *pwSize );
ADSPCOMMON_API BOOL ADspGetLicensePath ( TCHAR *pszLicensePath, WORD *pwSize );
ADSPCOMMON_API BOOL ADspGetProductVersion ( LPDWORD pdwVersionMajor,
										    LPDWORD pdwVersionMinor,
											LPTSTR pszPatch, LPWORD pwPatchSize );
ADSPCOMMON_API BOOL ADspRegisterComponent( LPCTSTR pszComponentName, LPCTSTR pszCLSID,
						   LPCTSTR pszCategory, LPCTSTR pszProgId, LPCTSTR pszVerIndProgId );
ADSPCOMMON_API BOOL ADspUnregisterComponent( LPCTSTR pszComponentName, LPCTSTR pszCLSID, LPCTSTR pszCategory );
ADSPCOMMON_API BOOL ADspRegisterPlugin( const ADSPCOMMON_PLUGININFO *pPluginInfo );
ADSPCOMMON_API BOOL ADspUnregisterPlugin( LPCTSTR pszPluginName );

// low-level API functions
ADSPCOMMON_API BOOL ADspResolvePath ( LPCTSTR szRelativePath, LPTSTR szFullPath );
ADSPCOMMON_API BOOL ADspGetVisualDSPKey ( TCHAR *pszVisualDSPKey, WORD *pwSize );
ADSPCOMMON_API BOOL ADspGetToolManagerProcessorCount ( long *plCount );
ADSPCOMMON_API BOOL ADspGetToolManagerProcessorName ( const long lProcessorIndex, TCHAR *pszProcessorName,
									   WORD *pwSize);
ADSPCOMMON_API BOOL ADspIsToolManagerProcessorInstalled ( const TCHAR *pszProcessorType );
ADSPCOMMON_API BOOL ADspGetStringValue ( const ADSPCOMMON_LOCATION Location,
						 LPCTSTR pszValueName, TCHAR *pszValue, WORD *pwSize);
ADSPCOMMON_API BOOL ADspSetStringValue ( const ADSPCOMMON_LOCATION Location,
						 LPCTSTR pszValueName, LPCTSTR pszValue);
ADSPCOMMON_API BOOL ADspGetDWORDValue ( const ADSPCOMMON_LOCATION Location,
						 LPCTSTR pszValueName, DWORD *pdwValue);
ADSPCOMMON_API BOOL ADspSetDWORDValue ( const ADSPCOMMON_LOCATION Location,
						 LPCTSTR pszValueName, const DWORD dwValue);
ADSPCOMMON_API BOOL ADspGetByteValues ( const ADSPCOMMON_LOCATION Location,
						LPCTSTR pszValueName, unsigned char *pucByteValues, WORD *pwSize);
ADSPCOMMON_API BOOL ADspSetByteValues ( const ADSPCOMMON_LOCATION Location,
						LPCTSTR pszValueName, const unsigned char *pucByteValues, const WORD wSize);
ADSPCOMMON_API BOOL ADspDeleteValue ( const ADSPCOMMON_LOCATION Location,
					  LPCTSTR pszValueName);
ADSPCOMMON_API BOOL ADspGetValueTotal ( const ADSPCOMMON_LOCATION Location,
						 LPCTSTR pszSubKeyName, DWORD *pdwSubKeyValueTotal);
ADSPCOMMON_API BOOL ADspEnumValue ( const ADSPCOMMON_LOCATION Location,
					LPCTSTR pszSubKeyName, const DWORD dwValueIndex,
					TCHAR *pszValueName, WORD *pwSize, ADSPCOMMON_TYPE *pType);
ADSPCOMMON_API BOOL ADspDeleteAllValues ( const ADSPCOMMON_LOCATION Location,
						  LPCTSTR pszSubKeyName);
ADSPCOMMON_API BOOL ADspGetSubKeyTotal ( const ADSPCOMMON_LOCATION Location,
						  LPCTSTR pszSubKeyName, DWORD *pdwSubKeyTotal);
ADSPCOMMON_API BOOL ADspEnumSubKey ( const ADSPCOMMON_LOCATION Location,
					  LPCTSTR pszSubKeyName, const DWORD dwSubKeyIndex,
					  TCHAR *pszSubKeyChildName, WORD *pwSize);
ADSPCOMMON_API BOOL ADspDeleteSubKey ( const ADSPCOMMON_LOCATION Location,
					    LPCTSTR pszSubKeyName);
ADSPCOMMON_API BOOL ADspGetToolManagerString( LPCTSTR pszProcessorName, LPCTSTR pszValueName, TCHAR *pszValue, WORD *pwSize );
ADSPCOMMON_API BOOL ADspGetToolManagerDWORD( LPCTSTR pszProcessorName, LPCTSTR pszValueName, DWORD *pdwValue );

#endif

