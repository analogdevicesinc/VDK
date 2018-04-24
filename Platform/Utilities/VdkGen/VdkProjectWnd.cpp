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
#include "vdkprojectwnd.h"
#include "VdkConfiguration.h"
#include "StdString.h"
#include <algorithm>
#include <list>
#include <errno.h>
#include <io.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Constants, Globals and Enumerations
/////////////////////////////////////////////////////////////////////////////

CVdkConfiguration* g_VdkData;
CVdkConfiguration* g_first_config_file=g_VdkData;
CVdkPlatformInfo g_VdkPlatformInfo;

extern bool vdkgen_for_rt;


/////////////////////////////////////////////////////////////////////////////
// CVdkSystemData
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkSystemData::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Instrumented Libraries" )
	{
		csaValues.push_back( "true" );
		csaValues.push_back( "false" );
	}
	else
	if( csProperty == "Instrumentation Level" )
	{
        	csaValues.push_back( "0" );
        	csaValues.push_back( "1" );
        	csaValues.push_back( "2" );
	}
	else
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkSystemData::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;
	UINT nMaxUInt = g_VdkPlatformInfo.GetMaxUInt();

	ASSERT( IsValidProperty(szName) );

	if( csName == "Maximum Running Threads" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		// we only have 9 bits for IDs so we cannot allow any more than 512
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > 512 )
			return( FALSE );
	}
	else if( csName == "Clock Frequency (MHz)" || csName == "Tick Period (ms)" )
	{
		double d = atof( szValue );
		if( d <= 0 || d >= nMaxUInt )
			return( FALSE );
	}
	else if( csName == "History Buffer Size" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Instrumented Libraries" )
	{
		if( strcmp(szValue,"true") && strcmp(szValue,"false") )
			return( FALSE );
	}
	else if( csName == "Instrumentation Level" )
	{
		if( stricmp(szValue,"Full Instrumentation") && stricmp(szValue,"Error Checking") && stricmp(szValue,"None") )
			return( FALSE );
	}
	else if ( csName == "Maximum Messages" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if ( csName == "Maximum Active Semaphores" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		// we only have 9 bits for IDs so we cannot allow any more than 512
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > 512 )
			return( FALSE );
	}
	else if ( csName == "Maximum Active Memory Pools" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		// we only have 9 bits for IDs so we cannot allow any more than 512
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > 512 )
			return( FALSE );
	}

	else if (csName == "Timer Interrupt")
	{
		CStdString csProcessor = g_VdkPlatformInfo.GetPlatform();
		if (csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || csProcessor == "ADSP-TS203") 
		{
			if ((csValue == "INT_TIMER0H_64" ||csValue == "INT_TIMER0L_64") ) {
				CStdString  SiRev = g_VdkPlatformInfo.GetSiliconRevision();
				if (  SiRev.CompareNoCase("automatic") != 0 && ((atof((LPCSTR) SiRev) < 1) || SiRev.CompareNoCase("any") == 0)) 
				{
					VDKGEN_ERROR_NO_FILE((LPCSTR)(csValue + " can only be used in si-revision 1.0 and higher" ));
					return (false);
				}
			}	
		}
		if (g_VdkPlatformInfo.GetNumTimerInterrupts() > 0 ) 
			// if there are timer interrupts check only them. Otherwise check the whole list of 
			// interrupts
		{
			for ( unsigned int j = 0 ; j<g_VdkPlatformInfo.GetNumTimerInterrupts();j++) 
			{
				if (g_VdkPlatformInfo.GetTimerInterrupt(j) == szValue) 
				{ 
					// found it in the timer interrupts list. Now remove it from the list of
					// available interrupts
						for ( unsigned int k = 0 ; k<g_VdkPlatformInfo.GetNumInterrupts();k++) 
						{
							if (g_VdkPlatformInfo.GetInterrupt(k) == szValue) 
							{ 
								g_VdkPlatformInfo.RemoveInterrupt(k); // an interrupt can only be used once
								return (TRUE);
							}
						}
				}
			}
		}
			else
		{
			for ( unsigned int j = 0 ; j<g_VdkPlatformInfo.GetNumInterrupts();j++) {
				if (g_VdkPlatformInfo.GetInterrupt(j) == szValue) { 
					g_VdkPlatformInfo.RemoveInterrupt(j); // an interrupt can only be used once
					return (TRUE);
				}
			}
		}
		if (csValue == "None") // None is now an accepted value
			return (TRUE);

	return (FALSE);
	VDKGEN_ERROR_NO_FILE ((LPCTSTR) (csName + " is not recognised or already in use"));

	}
	else if (csName == "Semaphores Heap" || csName == "Device Flags Heap" || csName == "Messages Heap" 
		|| csName == "I/0 Objects Heap") {
		if ( (g_VdkData->GetNumHeaps() == 0) && !strcmp (szValue , "system_heap"))
				return (TRUE);
		else if ( (g_VdkData->GetNumHeaps() == 0) && (g_VdkPlatformInfo.GetNumThreadStacks() > 1 || g_VdkPlatformInfo.GetNumContextAreas() > 1) &&
			(!strcmp (szValue , "system_heap0") || !strcmp (szValue , "system_heap1")))
				return (TRUE);
		else  if (g_VdkData->FindHeap(szValue))
			return (TRUE);
		else return (FALSE);
	}
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkThreadType
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkThreadType::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Priority" )
	{
		UINT nBits = g_VdkPlatformInfo.GetMaxBitWidth();

		for( unsigned int i=1; i<nBits; i++ )
		{
			cs.Format( "%d", i );
			csaValues.push_back( cs );
		}
	}
	else if( csProperty == "Stack Size" || csProperty == "Stack0 Size" || csProperty == "Stack1 Size")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Source File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Header File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Message Enabled" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Context0 Heap" || csProperty == "Context1 Heap" ||
			csProperty == "Stack0 Heap" || csProperty == "Stack1 Heap" || csProperty == "Stack Heap" || 
			csProperty == "Context Heap" || csProperty == "Thread Structure Heap")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkThreadType::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;

	ASSERT( IsValidProperty(szName) );

	if( csName == "Priority" )
	{
		int n = atoi( szValue );
		if( n < 1 || n >= (int) (g_VdkPlatformInfo.GetMaxBitWidth() - 1) )
			return( FALSE );
	}
	else if( csName == "Stack Size" || csName == "Stack0 Size" || csName == "Stack1 Size")
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Message Enabled" )
	{
		if( stricmp(szValue,"true") && stricmp(szValue,"false") )
			return( FALSE );
	}
	else if (csName == "Context0 Heap" || csName == "Context1 Heap" ||
			csName == "Stack0 Heap" || csName == "Stack1 Heap" || 
			csName == "Context Heap" || csName == "Stack Heap" || 
			csName == "Thread Structure Heap")
	{
		if ( (g_VdkData->GetNumHeaps() == 0) && !strcmp (szValue , "system_heap"))
				return (TRUE);
		else  if (g_VdkData->FindHeap(szValue))
			return (TRUE);
		else return (FALSE);
	}

	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkBootThread
/////////////////////////////////////////////////////////////////////////////

CVdkBootThread::CVdkBootThread()
{
	ASSERT( g_VdkData->GetNumThreadTypes() > 0 );
	AddProperty( "Thread Type", g_VdkData->GetThreadType(0).GetName() );
	AddProperty("Initializer","");
}

BOOL CVdkBootThread::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Thread Type" )
	{
		int nTypes = g_VdkData->GetNumThreadTypes();

		ASSERT( nTypes > 0 );

		if( nTypes == 1 ) csaValues.push_back( "" );

		for( int i=0; i<nTypes; i++ )
		{
			csaValues.push_back( g_VdkData->GetThreadType(i).GetName() );
		}
	}
	if (csProperty == "Initializer" )
		{
			VERIFY( GetProperty(szProperty, cs) );
			csaValues.push_back( cs );
		}

	return( TRUE );
}

BOOL CVdkBootThread::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName ), csValue(szValue);

	ASSERT( IsValidProperty(szName) );

	if( csName == "Thread Type" )
	{
		if( NULL == g_VdkData->FindThreadType( szValue ) )
		{
			return( FALSE );
		}
	}
	if (csName == "Initializer")
	{
		long result;

		result =strtol(csValue,NULL,0) ;
		if (result == LONG_MAX || result == LONG_MIN)
			return (FALSE);

		else if (result == 0)
		{

			bool invalid_string=false;
			int i;
			if (csValue.Left(2) == "0x")
			{
				for (i= 2; i<csValue.GetLength();i++)
				{
					if (!isxdigit(csValue[i])) 
					return (FALSE);
				}
			}
			else 
			{
				for (i= 0; i<csValue.GetLength();i++)
				{
					if (!isdigit(csValue[i])) 
					return (FALSE);
				}
			}
		}
			// check the values are in range for the platform
			if (result > g_VdkPlatformInfo.GetMaxLong()  || 
				result < g_VdkPlatformInfo.GetMinLong()) 
				return (FALSE);
			else
				return (TRUE);
	}

	return( TRUE );
}
/////////////////////////////////////////////////////////////////////////////
// CVdkIdleThread
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkIdleThread::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Stack Size" || csProperty == "Stack0 Size" || csProperty == "Stack1 Size")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Heap")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkIdleThread::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;

	ASSERT( IsValidProperty(szName) );

	if( csName == "Stack Size" || csName == "Stack0 Size" || csName == "Stack1 Size" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if (csName == "Heap" )
	{
		if ( (g_VdkData->GetNumHeaps() == 0) && !strcmp (szValue , "system_heap"))
				return (TRUE);
		else  if (g_VdkData->FindHeap(szValue))
			return (TRUE);
		else return (FALSE);
	}

	return( TRUE );
}
/////////////////////////////////////////////////////////////////////////////
// CVdkPriority
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkPriority::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Period" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkPriority::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;

	ASSERT( IsValidProperty(szName) );

	if( csName == "Period" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}

	return( TRUE );
}

void CVdkPriority::GetValidNameList( CStdStringVector& csa ) const
{
	CStdString cs;
	UINT nMaxBits = g_VdkPlatformInfo.GetMaxBitWidth();

	// The only valid priority names are those that are not
	// already in use.

	for( unsigned int i=1; i<nMaxBits-1; i++ )
	{
		cs.Format( "Priority%d", i );

		if( cs == GetName() || NULL == g_VdkData->FindPriority(cs) )
			csa.push_back( cs );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVdkRoutingNode
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkRoutingNode::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Routing Thread" )
	{
		int nThreads = g_VdkData->GetNumRoutingThreads();

		ASSERT( nThreads > 0 );

		if( nThreads == 1 ) csaValues.push_back( "" );

		for( int i=0; i<nThreads; i++ )
		{
			csaValues.push_back( g_VdkData->GetRoutingThread(i).GetName() );
		}

		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkRoutingNode::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CVdkRoutingThread* rthread;
	CStdString csValue(szValue);
	CStdString csLocalProcessor;

	g_first_config_file->GetMultiprocessorInfo().GetProperty("Local Processor ID",csLocalProcessor);

	ASSERT( IsValidProperty(szName) );

	if( csName == "Routing Thread" )
	{
		if (csValue == "0") {
			return (TRUE);
		}

		rthread = g_VdkData->FindRoutingThread( szValue );
		if( NULL == rthread )
		{
			return( FALSE );
		}

		rthread->GetProperty("Message Flow",csValue);
		if (csValue.CompareNoCase("incoming") == NULL)
			return (FALSE);

	}
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkRoutingThread
/////////////////////////////////////////////////////////////////////////////

CVdkRoutingThread::CVdkRoutingThread()
{
	AddProperty( "I/O Object", 		"" );
	AddProperty( "OpenDevice String", "");
	AddProperty( "Message Flow", "outgoing");
	AddProperty( "Priority", "5");
	if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
		if ( g_VdkPlatformInfo.GetNumContextAreas() == 1) {
		AddProperty( "Context Heap","system_heap");
	}	
	else if ( g_VdkPlatformInfo.GetNumContextAreas()>1){
		AddProperty( "Context0 Heap","system_heap0");
		AddProperty( "Context1 Heap","system_heap1");
	}
	if ( g_VdkPlatformInfo.GetNumThreadStacks() == 1) {
		AddProperty( "Stack Heap","system_heap");
	}
	else if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1) {
		AddProperty( "Stack0 Heap","system_heap0");
		AddProperty( "Stack1 Heap","system_heap1");	
	}

	if ( g_VdkPlatformInfo.GetNumStackSizes() == 1) {
		AddProperty( "Stack Size","255");		
	}
	else if ( g_VdkPlatformInfo.GetNumStackSizes() > 1) {
		AddProperty( "Stack0 Size","255");	
		AddProperty( "Stack1 Size","255");			
	}

	if ( g_VdkPlatformInfo.GetNumContextAreas()>1 || 
		g_VdkPlatformInfo.GetNumThreadStacks() > 1)
	{
		AddProperty( "Thread Structure Heap" , "system_heap0");
	}
	else
	{
		AddProperty( "Thread Structure Heap" , "system_heap");
	}
	}
	SetName( "RoutingThread" );
	// don't expand this node by default
	m_bExpanded = FALSE;
}

BOOL CVdkRoutingThread::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Priority" )
	{
		UINT nBits = g_VdkPlatformInfo.GetMaxBitWidth();

		for( unsigned int i=1; i<nBits; i++ )
		{
			cs.Format( "%d", i );
			csaValues.push_back( cs );
		}
	}
	else if( csProperty == "Stack Size" || csProperty == "Stack0 Size" || csProperty == "Stack1 Size")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "I/O Object" )
	{
		int nDrivers = g_VdkData->GetNumBootIOObjects();

		ASSERT( nDrivers > 0 );

		if( nDrivers == 1 ) csaValues.push_back( "" );

		for( int i=0; i<nDrivers; i++ )
		{
			csaValues.push_back( g_VdkData->GetBootIOObject(i).GetName() );
		}

		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Opendevice String" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Message Flow" )
	{
		csaValues.push_back( "incoming" );
		csaValues.push_back( "outgoing" );
	}
	else if( csProperty == "Context0 Heap" || csProperty == "Context1 Heap" ||
			csProperty == "Stack0 Heap" || csProperty == "Stack1 Heap" || csProperty == "Stack Heap" || 
			csProperty == "Context Heap" || csProperty == "Thread Structure Heap")
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkRoutingThread::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;
	UINT nMaxUInt = g_VdkPlatformInfo.GetMaxUInt();

	ASSERT( IsValidProperty(szName) );

		if( csName == "Priority" )
	{
		int n = atoi( szValue );
		if( n < 1 || (unsigned int) n >= g_VdkPlatformInfo.GetMaxBitWidth() - 1 )
			return( FALSE );
	}
	else if( csName == "Stack Size" || csName == "Stack0 Size" || csName == "Stack1 Size")
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if (csName == "Context0 Heap" || csName == "Context1 Heap" ||
			csName == "Stack0 Heap" || csName == "Stack1 Heap" || 
			csName == "Context Heap" || csName == "Stack Heap" || 
			csName == "Thread Structure Heap")
	{
		if ( (g_VdkData->GetNumHeaps() == 0) && !strcmp (szValue , "system_heap"))
				return (TRUE);
		else  if (g_VdkData->FindHeap(szValue))
			return (TRUE);
		else return (FALSE);
	}
	else if( csName == "I/O Object" )
	{
		if( NULL == g_VdkData->FindBootIOObject( szValue ) )
		{
			return( FALSE );
		}
	}
	else if( csName == "Opendevice String" )
	{
		if (strchr(szValue,'"')) return( FALSE );
	}

	else if( csName == "Message Flow" )
	{
		if( stricmp(szValue,"incoming") && stricmp(szValue,"outgoing") )
			return( FALSE );
	}

	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkBootIOObject
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkBootIOObject::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "TemplateName" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if (csProperty == "Initializer" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkBootIOObject::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName ),csValue(szValue);

	ASSERT( IsValidProperty(szName) );

	if( csName == "Template" )
	{
	return( TRUE );
	}
	else 
	if (csName == "Initializer")
	{
		long result;

		result =strtol(csValue,NULL,0) ;
		if (result == LONG_MAX || result == LONG_MIN)
			return (FALSE);

		else if (result == 0)
		{

			bool invalid_string=false;
			int i;
			if (csValue.Left(2) == "0x")
			{
				for (i= 2; i<csValue.GetLength();i++)
				{
					if (!isxdigit(csValue[i])) 
					return (FALSE);
				}
			}
			else 
			{
				for (i= 0; i<csValue.GetLength();i++)
				{
					if (!isdigit(csValue[i])) 
					return (FALSE);
				}
			}
		}
			// check the values are in range for the platform
			if (result > g_VdkPlatformInfo.GetMaxLong()  || 
				result < g_VdkPlatformInfo.GetMinLong()) 
				return (FALSE);
			else
				return (TRUE);
	}

 	return (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CVdkSemaphore
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkSemaphore::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Initial Value" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Initial Delay" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Periodic" )
	{
		csaValues.push_back( "true" );
		csaValues.push_back( "false" );
	}
	else if( csProperty == "Period" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Delay" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Max Count" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	return( TRUE );
}

BOOL CVdkSemaphore::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue(szValue);
	unsigned int n;

	ASSERT( IsValidProperty(szName) );

	if( csName == "Initial Value" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if((csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Initial Delay" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if((csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Periodic" )
	{
		if( stricmp(szValue,"true") && stricmp(szValue,"false") )
			return( FALSE );
	}
	else if( csName == "Period" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Delay" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Max Count" )
	{
		if (strcmp(szValue,"")) n = g_VdkPlatformInfo.GetMaxUInt();
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkHeap
// Right now heaps don't have properties
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkHeap::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
		return (TRUE);
}

BOOL CVdkHeap::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	// on SHARC the string that describes a heap cannot be longer than
	// 8 characters
	if (g_VdkPlatformInfo.GetFamily() == "SHARC")
		if (strcmp (szName,"ID") == 0) 
			if ( strlen(szValue) >8 ) return (FALSE);
		return (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CVdkMemoryPool
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkMemoryPool::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Block Size" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Number of Blocks" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Initialize on Pool Creation" )
	{
		csaValues.push_back( "true" );
		csaValues.push_back( "false" );
	}
	else if( csProperty == "Heap" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	return( TRUE );
}

BOOL CVdkMemoryPool::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue (szValue);
	unsigned int n;


	ASSERT( IsValidProperty(szName) );

	if( csName == "Block Size" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 ) ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Number of Blocks" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 ) ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	else if( csName == "Initialize on Pool Creation" )
	{
		if( stricmp(szValue,"true") && stricmp(szValue,"false") )
			return( FALSE );
	}
	else if( csName == "Heap" )
	{
		if (csValue.FindOneOf("-") != -1) return (FALSE);
		n = strtoul( szValue,NULL,10 );
		if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
			return( FALSE );
	}
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkMultiprocessorInfo
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkMultiprocessorInfo::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Local Processor ID" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Multiprocessor Messages Allowed" )
	{
		csaValues.push_back( "true" );
		csaValues.push_back( "false" );
	}

	return( TRUE );
}

BOOL CVdkMultiprocessorInfo::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	UINT nMaxUInt = g_VdkPlatformInfo.GetMaxUInt();

	ASSERT( IsValidProperty(szName) );

	if( csName == "Local Processor ID" )
	{
		double d = atof( szValue );
		if( d < 0 || d >= 31 )
			return( FALSE );
	}
	else if( csName == "Multiprocessor Messages Allowed" )
	{
		if( stricmp(szValue,"true") && stricmp(szValue,"false") )
			return( FALSE );
	}

	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkMarshalledMessage
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkMarshalledMessage::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Marshalling Function" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if(csProperty == "Alloc Type")
	{
		csaValues.push_back( "heap" );
		csaValues.push_back( "pool" );
		csaValues.push_back( "clusterbus");
		csaValues.push_back( "custom" );
	}
	else if( csProperty == "Alloc Area" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Source File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkMarshalledMessage::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue;
	CStdString csType;
	unsigned int n;
	UINT nMaxUInt = g_VdkPlatformInfo.GetMaxUInt();

	ASSERT( IsValidProperty(szName) );

	if( csName == "Marshalling Function" )
	{
		if (!IsCIdentifier(szValue)) 
			return (FALSE);
	}
	else if( csName == "Alloc Area" )
	{
		GetProperty("Alloc Type",csType);

		if (csType.CompareNoCase("heap")==NULL ) {
			if( NULL == g_VdkData->FindHeap( szValue ) )
			{
				return( FALSE );
			}
		}
		else if (csType.CompareNoCase("pool")==NULL )	{
			if( NULL == g_VdkData->FindMemoryPool( szValue ) )
			{
				return( FALSE );
			}
		}
		// a cluster bus message type does not have an alloc area
		else if (csType.CompareNoCase("clusterbus") == NULL) {
			csValue = szValue;
		    if (csValue != "") return (FALSE);
		}
		else {
			csValue = szValue;
			if (csValue.FindOneOf("-") != -1) return (FALSE);
			n = strtoul( szValue,NULL,10 );
			if( (csValue != "UINT_MAX") && (csValue != "INT_MAX") && ( n == 0 && csValue !="0") ||  n > g_VdkPlatformInfo.GetMaxUInt() )
				return( FALSE );
		}
	}
	else if( csName == "Alloc Type" )
	{
		if( stricmp(szValue,"pool") && stricmp(szValue,"heap") && stricmp(szValue,"custom") 
			&& stricmp(szValue,"clusterbus"))
			return( FALSE );
	}


	return( TRUE );
}
/////////////////////////////////////////////////////////////////////////////
// CVdkMessagePort
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkMessagePort::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty(szProperty);
	csaValues.clear();

	if( csProperty == "Flags" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Remote Processors" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkMessagePort::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	UINT nMaxUInt = g_VdkPlatformInfo.GetMaxUInt();

	ASSERT( IsValidProperty(szName) );

	if( csName == "Flags" )				// valid C value
	{
		int n = atoi( szValue );
		if( n <= 0 || (unsigned int) n >= nMaxUInt )
			return( FALSE );
	}
	else if( csName == "Remote Processors" )	// comma delimited list of uints
	{
		double d = atof( szValue );
		if( d <= 0 || d >= 31 )
			return( FALSE );
	}

	return( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CVdkEvent
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkEvent::GetPropertyValues( LPCTSTR szName, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csName( szName );
	int  bitNum = -1;

	csaValues.clear();

	if( csName == "Type" )
	{
		csaValues.push_back( "Any" );
		csaValues.push_back( "All" );
		csaValues.push_back( "any" );
		csaValues.push_back( "all" );
	}
	else if( (bitNum = FindBit(szName)) != -1 )
	{
		csaValues.push_back( "0" );
		csaValues.push_back( "1" );
	}

	return( TRUE );
}

BOOL CVdkEvent::SetProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CVdkStringPair* p = NULL;
	int bitNum = -1;
	if( csName == "Type" )
	{
		return( CVdkElement::SetProperty(szName, szValue) );
	}
	else if( (bitNum = FindBit(szName)!= -1) )
	{
		if( ValidateProperty(szName, szValue) )
		{
			m_vBits[bitNum].second = szValue;
			return( TRUE );
		}
	}

	return( FALSE );
}

BOOL CVdkEvent::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CVdkStringPair* p = NULL;
	int bitNum = -1;

	if( csName == "Type" )
	{
		if( stricmp(szValue,"Any") && stricmp(szValue,"All") )
			return( FALSE );
	}
	else if( (bitNum = FindBit(szName)!= -1))
	{
		if( strcmp(szValue,"0") && strcmp(szValue,"1") )
			return( FALSE );
	}

	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkEventBit
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkEventBit::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Initial Value" )
	{
		csaValues.push_back( "0" );
		csaValues.push_back( "1" );
	}

	return( TRUE );
}

BOOL CVdkEventBit::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );

	ASSERT( IsValidProperty(szName) );

	if( csName == "Initial Value" )
	{
		if( strcmp(szValue,"0") && strcmp(szValue,"1") )
			return( FALSE );
	}

	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CVdkISR
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkISR::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs;
	CStdString csProperty( szProperty );

	ASSERT( IsValidProperty(szProperty) );

	csaValues.clear();

	if( csProperty == "Enabled at Boot" )
	{
		csaValues.push_back( "true" );
		csaValues.push_back( "false" );
	}
	else if( csProperty == "Source File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Entry Point" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkISR::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	CStdString csName( szName );
	CStdString csValue(szValue);

	CStdString csProcessor = g_VdkPlatformInfo.GetPlatform();
	if (csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || csProcessor == "ADSP-TS203") 
	{
		if ((csValue == "INT_TIMER0H_64" ||csValue == "INT_TIMER0L_64") ) {
			CStdString  SiRev = g_VdkPlatformInfo.GetSiliconRevision();
			if (  SiRev.CompareNoCase("automatic") != 0 && ((atof((LPCSTR) SiRev) < 1) || SiRev.CompareNoCase("any") == 0)) 
			{
				VDKGEN_ERROR_NO_FILE((LPCSTR)(csValue + " can only be used in si-revision 1.0 and higher" ));
				return (false);
			}
		}	
	}

	ASSERT( IsValidProperty(szName) );

	if( csName == "Enabled at Boot" )
	{
		if( stricmp(szValue,"true") && stricmp(szValue,"false") )
			return( FALSE );
	}

	return( TRUE );
}

void CVdkISR::GetValidNameList( CStdStringVector& csa ) const
{
	CStdString cs;
	UINT nISRs = g_VdkPlatformInfo.GetNumInterrupts();

	// The only valid ISR names are those that are not
	// already in use.

	for( unsigned int i=0; i<nISRs; i++ )
	{
		cs = g_VdkPlatformInfo.GetInterrupt( i );

		if( cs == GetName() || (NULL == g_VdkData->FindISR(cs) && NULL == g_VdkData->FindISR2(cs)) )
			csa.push_back( cs );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVdkDeviceDriver
/////////////////////////////////////////////////////////////////////////////

BOOL CVdkDeviceDriver::GetPropertyValues( LPCTSTR szProperty, CStdStringVector& csaValues )
{
	CStdString cs, csProperty( szProperty );
	csaValues.clear();

	if( csProperty == "Source File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}
	else if( csProperty == "Header File" )
	{
		VERIFY( GetProperty(szProperty, cs) );
		csaValues.push_back( cs );
	}

	return( TRUE );
}

BOOL CVdkDeviceDriver::ValidateProperty( LPCTSTR szName, LPCTSTR szValue )
{
	return( TRUE );
}

//////////////////////////////////////////////////////////////////////
// class CVdkTemplateFile
//////////////////////////////////////////////////////////////////////

BOOL CVdkTemplateFile::Create( LPCTSTR szFileName, LPCTSTR szTemplateFile )
{
	// close the file if it's already open
	if( m_pFile != NULL ) Close();

	char nFlagsStd[4];

		if (!strcmp (szTemplateFile,"") ) {
			// generate a new file
				strcpy(nFlagsStd, "w+b");

		}
		else {
			// make a copy of the template file to create our new file
				strcpy(nFlagsStd,"r+b");
				if( !CopyFile(szTemplateFile, szFileName, FALSE) )
					return( FALSE );
		}

	// open the new file

	if( (m_pFile = fopen(szFileName, nFlagsStd)) == NULL)
		return( FALSE );

	int iFileDescriptor = _fileno (m_pFile);
	ftell(m_pFile);

	// read its contents into m_csData
	ULONGLONG dwLength = _filelengthi64(iFileDescriptor);
		fseek(m_pFile,0,SEEK_SET);

	int charRead = fread( m_csData.GetBuffer((int) dwLength), sizeof(char), (UINT) dwLength, m_pFile );
	if (charRead) m_csData[charRead]=0;
	m_csData.ReleaseBuffer( (int) dwLength );
	fseek(m_pFile,0,SEEK_SET);

	return( TRUE );
}

UINT CVdkTemplateFile::ReplaceToken( LPCTSTR szToken, LPCTSTR szValue )
{
	// replace all occurances of szToken with szValue
	UINT nReplaced = m_csData.Replace( szToken, szValue );

	if( nReplaced > 0 && !IsBufferingEnabled() )
	{
		// and write the new data to disk
		rewind(m_pFile);
		fwrite( m_csData.GetBuffer(), sizeof(char), m_csData.GetLength(), m_pFile );
		int iFileDescriptor = _fileno (m_pFile);

		_chsize( iFileDescriptor, m_csData.GetLength() );
	}
			rewind(m_pFile);

	return( nReplaced );
}

void CVdkTemplateFile::Flush()
{
	if( !m_csData.IsEmpty()  )
	{
		// write the new data to disk
		fseek(m_pFile,0,SEEK_SET);
		fwrite( m_csData.GetBuffer(), sizeof(char), m_csData.GetLength(), m_pFile );
		int iFileDescriptor = _fileno (m_pFile);
		_chsize( iFileDescriptor, m_csData.GetLength() );
	}

	fflush(m_pFile);

}

void CVdkTemplateFile::Close()
{
	// if buffering is enabled and there is data to write
	// then we need to flush it to disk before closing

	if( !m_csData.IsEmpty() && IsBufferingEnabled() )
		Flush();

	fclose(m_pFile);
}
