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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Constants, Globals and Enumerations
/////////////////////////////////////////////////////////////////////////////

extern CVdkConfiguration* g_VdkData;
extern CVdkConfiguration* g_first_config_file;
extern CVdkPlatformInfo g_VdkPlatformInfo;
list <CStdString> g_all_messages_list;


CStdString choose_heap_string (CStdString str, bool comma_added=false)
{
	CStdString characters_added="k";
	if (str=="") return str;
	if (comma_added) characters_added=" ," + characters_added;
	if (g_VdkData->GetNumHeaps() == 0 && str == "system_heap")
		return ( (characters_added + "0"));
	else if (g_VdkData->GetNumHeaps() == 0 && str == "system_heap0")
		return ( (characters_added + "0"));
	else if (g_VdkData->GetNumHeaps() == 0 && str == "system_heap1")
		return ( (characters_added + "1"));
	else  if (g_VdkData->FindHeap(str))
		return ( (characters_added + str));
	else {
		VDKGEN_ERROR_NO_FILE( (LPCTSTR) (str + " is not a valid heap"));
		return ("NULL");
	}
}

/////////////////////////////////////////////////////////////////////////////
// BOOL GenerateConfigSource()
/////////////////////////////////////////////////////////////////////////////

BOOL GenerateConfigSource( LPCTSTR szProjectPath )
{
	CVdkTemplateFile SourceFile;

	CStdString csProjectPath( szProjectPath );
	csProjectPath.ReleaseBuffer();
	CVdkConfiguration *libConfig;
	
	// variable to calculate the max num of threads, semaphores, mempools,...
	UINT total_number = 0; 
	// string to hold the maximum number of threads, semaphores, mempools,...
	char max_active[10]; 

	UINT nElements = 0;
	UINT i = 0;
	CStdString cs1, cs2, cs3, cs4, cs5, cs6, cs7, cs8,cs9,cs10, cs11, cs12, cs13, cs14, csName;
	CStdString csTemplateBase = g_VdkPlatformInfo.GetTemplatePath();
	CStdString csProcessor = g_VdkPlatformInfo.GetPlatform();
	CStdString csFamily =  g_VdkPlatformInfo.GetFamily();


	/////////////////////////////////////////////////////////////////////////
	//  Generate the Source File
	/////////////////////////////////////////////////////////////////////////

	if( !SourceFile.Create( csProjectPath + "VDK.cpp", csTemplateBase + "VDK.cpp.tf") )
	{
		CStdString csMsg;
		VDKGEN_ERROR_NO_FILE("An error has occured while trying to create the source file ");
		cerr<<(LPCSTR)("              " + csProjectPath + "VDK.cpp\n");
		cerr<<"              Please make sure that the path is valid and that the directory is writable."<<endl;
		return( FALSE );
	}

	SourceFile.EnableBuffering( TRUE );

	/////////////////////////////////////////////////////////////////////////
	//  %CreationTime%  %Year% and %VDKGenVersion
	/////////////////////////////////////////////////////////////////////////

	time_t sTime;
	time(&sTime);

	struct tm *pTime = localtime( &sTime );
	char strTime[MAX_PATH], strTime2[MAX_PATH];

	strftime(strTime,MAX_PATH,"%A, %B %d, %Y - %I:%M:%S %p", pTime);
	strftime(strTime2,MAX_PATH,"%Y", pTime);

	SourceFile.ReplaceToken( "%CreationTime%", strTime );
	SourceFile.ReplaceToken( "%Year%", strTime2 );

	SourceFile.ReplaceToken( "%VDKGenVersion%", VdkGenVersion );

	/////////////////////////////////////////////////////////////////////////
	//  %IncludeThreadHeaderFiles%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumThreadTypes();

	for( i=0; i<nElements; i++ )
	{
		VERIFY( g_VdkData->GetThreadType(i).GetProperty("Header File", cs1) );
		cs2.Format( "#include \"%s\"", cs1 );
		cs3 += cs2;
		if( i < nElements-1 ) { cs3 += "\r\n"; }
	}

	SourceFile.ReplaceToken( "%IncludeThreadHeaderFiles%", cs3 );

	/////////////////////////////////////////////////////////////////////////
	//  %IncludeDeviceDriverHeaderFiles%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumDeviceDrivers();
	cs1 = cs2 = cs3 = "";

	for( i=0; i<nElements; i++ )
	{
		VERIFY( g_VdkData->GetDeviceDriver(i).GetProperty("Header File", cs1) );
		cs2.Format( "#include \"%s\"", cs1 );
		cs3 += cs2;
		if( i < nElements-1 ) { cs3 += " \r\n"; }
	}

	SourceFile.ReplaceToken( "%IncludeDeviceDriverHeaderFiles%", cs3 );
	
	/////////////////////////////////////////////////////////////////////////
	//  %IdleThreadTemplate%
	/////////////////////////////////////////////////////////////////////////
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = "";
	CVdkIdleThread& IdleThread= g_VdkData->GetIdleThread( 0 );

	if (g_VdkPlatformInfo.GetNumThreadStacks() == 1) {
		VERIFY( IdleThread.GetProperty("Stack Size",     cs1) );
	}else {
		VERIFY(IdleThread.GetProperty("Stack0 Size",     cs1));
		cs1 += ",";
		VERIFY(IdleThread.GetProperty("Stack1 Size",     cs2));
	}
		VERIFY(IdleThread.GetProperty("Heap",     cs3));

	// in the idle thread we only get one heap and everything goes in it
	if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1)  cs4 = cs3;
	if ( g_VdkPlatformInfo.GetNumContextAreas()> 0)  {
		cs5 = cs3;
		if (g_VdkPlatformInfo.GetNumContextAreas()> 1)
			cs6 =cs3;
	}

	cs7.Format( " INIT_THREADTEMPLATE_(\"Idle Thread\",static_cast<VDK::Priority>(0), %s %s, NULL %s%s%s%s%s,false)",
			cs1, cs2 /* stack size 2 if there is one */ , choose_heap_string(cs5,true) , choose_heap_string(cs3,true), 
			choose_heap_string(cs6,true), choose_heap_string(cs4,true), choose_heap_string(cs3,true) );

	SourceFile.ReplaceToken( "%IdleThreadTemplate%", cs7 );

	/////////////////////////////////////////////////////////////////////////
	//  %ThreadTemplateConstructors%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumThreadTypes();
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = cs8 = cs9 = cs10 = cs11 = cs12 = cs13 ="";

	for( i=0; i<nElements; i++ )
	{
		CStdString csFunctionName;
		CVdkThreadType& Thread = g_VdkData->GetThreadType( i );
		csName = Thread.GetName();

		if ( g_VdkPlatformInfo.GetNumStackSizes() == 1) {
			VERIFY( Thread.GetProperty("Stack Size",  cs1) );
		}
		else if (g_VdkPlatformInfo.GetNumStackSizes() > 1) {
			VERIFY( Thread.GetProperty("Stack0 Size",  cs1) );
			VERIFY( Thread.GetProperty("Stack1 Size", cs13));
			cs13 +=",";
		}
		VERIFY( Thread.GetProperty("Priority",    cs2) );
		VERIFY( Thread.GetProperty("Source File", cs3) );
		VERIFY( Thread.GetProperty("Message Enabled", cs6) );
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			if ( g_VdkPlatformInfo.GetNumContextAreas() == 1) {
				VERIFY( Thread.GetProperty("Context Heap",cs7));
			}
			else if ( g_VdkPlatformInfo.GetNumContextAreas()> 1) {
				VERIFY( Thread.GetProperty("Context0 Heap",cs7));
				VERIFY( Thread.GetProperty("Context1 Heap",cs9));
			}

			if ( g_VdkPlatformInfo.GetNumThreadStacks() == 1) {
				VERIFY( Thread.GetProperty("Stack Heap",cs9));
			}
			else if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1) {
				VERIFY( Thread.GetProperty("Stack0 Heap",cs8));
			 	VERIFY( Thread.GetProperty("Stack1 Heap",cs10));
			}
			VERIFY( Thread.GetProperty("Thread Structure Heap",cs11));
		}
		if( cs3.Find(".cpp") == -1 && cs3.Find(".cxx") == -1 )
			csFunctionName = csName + "_Wrapper";
		else
			csFunctionName = csName;

		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) // 219x does not have multiple heaps
			cs4.Format( "        INIT_THREADTEMPLATE_(\"k%s\",kPriority%s, %s, %s ::%s::Create %s%s%s%s%s,%s)",
				csName, cs2, cs1, cs13 /* stack size 2 if there is one */ , csFunctionName, choose_heap_string(cs7,true) , choose_heap_string(cs8,true), 
				choose_heap_string(cs9,true), choose_heap_string(cs10,true), choose_heap_string(cs11,true), cs6 );
		else
			cs4.Format( "        INIT_THREADTEMPLATE_(\"k%s\", kPriority%s, %s, ::%s::Create,%s)",
				csName, cs2, cs1, csFunctionName, cs6 );

		cs5 += cs4; 
		if( i < nElements-1 ) { cs5 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%ThreadTemplateConstructors%", cs5 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumThreads%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of threads we need to go through all the 
	// libraries we are including and see how many threads they needed so we
	// can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Running Threads", cs1) );
		total_number += libConfig->GetMaxRunningThreads();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);

	SourceFile.ReplaceToken( "%MaxNumThreads%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumSemaphores%
	/////////////////////////////////////////////////////////////////////////
	
	// To know the maximum number of semaphores we need to go through all the 
	// libraries we are including and see how many semaphores they needed so we
	// can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Active Semaphores", cs1) );
		total_number += libConfig->GetMaxActiveSemaphores();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	SourceFile.ReplaceToken( "%MaxNumActiveSemaphores%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumMemoryPools%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of mempools we need to go through all the 
	// libraries we are including and see how many mempools they needed so we
	// can add them to the total

	bool messages_pool_required = false;
	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ( messages_pool_required== false && libConfig->GetNumMessages() >0) {
			messages_pool_required = true;
			total_number +=1;
		}
		total_number += libConfig->GetMaxActiveMemoryPools();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	SourceFile.ReplaceToken( "%MaxNumActiveMemoryPools%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumDeviceFlags%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of device flags we need to go through all the 
	// libraries we are including and see how many device flags they needed so 
	// we can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Active Device Flags", cs1) );
		total_number += libConfig->GetMaxActiveDevFlags();
	}
	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	SourceFile.ReplaceToken( "%MaxNumActiveDevFlags%", cs1 );


	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumIOObjects%  
	/////////////////////////////////////////////////////////////////////////
	VERIFY( g_VdkData->GetSystemData().GetProperty("Maximum I/O Objects", cs1) );
	SourceFile.ReplaceToken( "%MaxNumIOObjects%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %NumBootDeviceFlags%
	/////////////////////////////////////////////////////////////////////////
	cs1 = "";
	cs1 = itoa(g_VdkData->GetNumDevFlags(), max_active, 10);
	SourceFile.ReplaceToken( "%NumBootDevFlags%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %BootThreadIDs% and %NumBootThreads%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumBootThreads();
	cs1 = "";
	cs1 = itoa(nElements, max_active, 10);
	SourceFile.ReplaceToken( "%NumBootThreads%", cs1 );

	cs1 = cs2 = cs3 ="";

	for( i=0; i<nElements; i++ )
	{
		VERIFY( g_VdkData->GetBootThread(i).GetProperty("Thread Type", csName) );
		VERIFY( g_VdkData->GetBootThread(i).GetProperty("Initializer", cs1) );
		cs2.Format( "        { &g_ThreadTemplates[k%s], %s }",	csName, cs1 );
		cs3 += cs2;
		if( i < nElements-1 ) { cs3 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%BootThreadIDs%", cs3 );

	/////////////////////////////////////////////////////////////////////////
	//  %RoundRobinPriorities%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumPriorities();
	cs1 = cs2 = "";

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetPriority(i).GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Priority with no name found");
		cs1.Format( "        1 << k%s        | // k%s is round robin", csName, csName );
		cs2 += cs1;
		if( i < nElements-1 ) { cs2 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%RoundRobinPriorities%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//  %RoundRobinPeriods%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkPlatformInfo.GetMaxBitWidth();
	cs1 = cs2 = cs3 = "";

	for( i=nElements-2; i>0; i-- )
	{
		csName.Format( "Priority%d", i );
		CVdkPriority* p = g_VdkData->FindPriority( csName );

		if( p ) VERIFY( p->GetProperty("Period", cs1) );
		else    cs1 = "1";

		cs2.Format( "        %s,        // k%s", cs1, csName );
		cs3 += cs2;
		if( i > 1 ) { cs3 += "\r\n"; }
	}

	SourceFile.ReplaceToken( "%RoundRobinPeriods%", cs3 );

	/////////////////////////////////////////////////////////////////////////
	//  %ClockFrequency%, %TickPeriod% and %HistoryBufferSize%
	/////////////////////////////////////////////////////////////////////////

	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = cs8 =  "";

	char chClockPeriod[MAX_PATH], chClockPrescale[MAX_PATH];

	VERIFY( g_VdkData->GetSystemData().GetProperty("Clock Frequency (MHz)", cs1) );
	VERIFY( g_VdkData->GetSystemData().GetProperty("Tick Period (ms)", cs2) );
	VERIFY( g_VdkData->GetSystemData().GetProperty("History Buffer Size", cs3) );
	VERIFY( g_VdkData->GetSystemData().GetProperty("Instrumentation Level", cs4) );
	if (atoi(cs3) == 0 && cs4 == "Full Instrumentation")
		VDKGEN_ERROR_NO_FILE("0 is not a valid value for History Buffer Size");
	VERIFY( g_VdkData->GetSystemData().GetProperty("Timer Interrupt",cs5) );

//	if (csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || csProcessor == "ADSP-TS203") 
//	{
//		if ((cs5 == "INT_TIMER0H_64" ||cs5 == "INT_TIMER0L_64") ) {
//			CStdString  SiRev = g_VdkPlatformInfo.GetSiliconRevision();
//			if ( (atof((LPCSTR) SiRev) < 1) || SiRev.CompareNoCase("any") == 0) 
//			{
//				VDKGEN_ERROR_NO_FILE((LPCSTR)(cs5 + " can only be used in si-revision 1.0 and higher" ));

//			}
//		}
//	}

	// INTVECTOR in TS does not take the leading _ that all the other platforms take
	cs6 = g_VdkPlatformInfo.GetFamily();
	CStdString csTimerFunc;
	csTimerFunc = "_tmk_TimerISR";

	float fp;
	double dFrequency;
	sscanf( cs1, "%lf", &dFrequency ); // we want to read the variable as a double
	fp = (float) (dFrequency*1000) ; // truncating the result to a float which is what frequency is in VDK.cpp
						 // we need the calculation to be done in double to maintain
                         // the precision
	cs1.Format( "%.03f", fp);

	SourceFile.ReplaceToken( "%ClockFrequency%", cs1 );
	SourceFile.ReplaceToken( "%TickPeriod%", cs2 );
	SourceFile.ReplaceToken( "%HistoryBufferSize%", cs3 );

	// calculate the timing parameters so we don't always have to link in the 
	// floating point libraries. These will be linked in if users change the
	// timing parameters with the APIs.

	double fTickPeriod; 
	unsigned int iClockFrequency, iClockPeriod, iClockPrescale;
	fTickPeriod = atof(LPCSTR(cs2));
	iClockFrequency = atoi(cs1);
	if (g_VdkPlatformInfo.GetPlatform().Left(8).CompareNoCase("ADSP-TS2") == 0)
		iClockPeriod   = (unsigned int) (((long)( iClockFrequency * fTickPeriod / 2 )) % g_VdkPlatformInfo.GetMaxUInt());
	else
		iClockPeriod   = (unsigned int) (((long)( iClockFrequency * fTickPeriod )) % g_VdkPlatformInfo.GetMaxUInt());
	iClockPrescale = (unsigned int) (((long)( iClockFrequency * fTickPeriod )) / g_VdkPlatformInfo.GetMaxUInt());

	// only in blackfin we need to know the clock prescale. It doesn't exist
	// in the other platforms

	if (g_VdkPlatformInfo.GetFamily().CompareNoCase("Blackfin") == 0)
		sprintf(chClockPrescale,"    unsigned int g_ClockPrescale = %d;\0",iClockPrescale);
	else 
		sprintf(chClockPrescale, "\0");

	sprintf(chClockPeriod, "%d\0",iClockPeriod);
	SourceFile.ReplaceToken( "%ClockPeriod%", chClockPeriod );
	SourceFile.ReplaceToken( "%ClockPrescale%", chClockPrescale );

	CStdString csTimerInterruptRoutine = "";
	if (cs5 == "None")  // if no timer specified we blank the entry
	{
		csTimerInterruptRoutine = "";
		SourceFile.ReplaceToken( "%TimerInterruptRoutine%", "" );
		SourceFile.ReplaceToken( "%TimerInterrupt%", "" );
	}
	else 
	{
		csTimerInterruptRoutine = "        INTVECTOR(seg_"+ cs5 + ", " + csTimerFunc + ")" ;
		SourceFile.ReplaceToken( "%TimerInterruptRoutine%", "        INTVECTOR(seg_"+ cs5 + ", " + csTimerFunc + ")" );
		SourceFile.ReplaceToken( "%TimerInterrupt%", cs5 + " |" );
	}

	cs6 = cs7 = cs8 = "";
	if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
		VERIFY( g_VdkData->GetSystemData().GetProperty("Semaphores Heap",cs6) );
		VERIFY( g_VdkData->GetSystemData().GetProperty("Device Flags Heap",cs7) );
		VERIFY( g_VdkData->GetSystemData().GetProperty("Messages Heap",cs8) );
		VERIFY( g_VdkData->GetSystemData().GetProperty("I/O Objects Heap",cs9) );
	}
		SourceFile.ReplaceToken( "%SemaphoresHeap%", choose_heap_string(cs6) );
		SourceFile.ReplaceToken( "%DeviceFlagsHeap%", choose_heap_string(cs7) );
		SourceFile.ReplaceToken( "%MessagesHeap%", choose_heap_string(cs8) );
		SourceFile.ReplaceToken( "%IOObjectsHeap%", choose_heap_string(cs9) );

	


	/////////////////////////////////////////////////////////////////////////
	//  %SemaphoreConstructors%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumSemaphores();

	cs1 = cs2 = cs3 = cs4 = cs5 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkSemaphore& Semaphore = g_VdkData->GetSemaphore( i );

		csName = Semaphore.GetName();
		// the default Max Count is UINT_MAX so, if there is not a value, then UINT_MAX 
		// will be used
		VERIFY( Semaphore.GetProperty("Max Count",     cs2) );
		VERIFY( Semaphore.GetProperty("Initial Value", cs1) );
		VERIFY( Semaphore.GetProperty("Initial Delay", cs2) );
		VERIFY( Semaphore.GetProperty("Period",        cs3) );

		cs4.Format( "        Semaphore( k%s, %s, %s, %s )",
					csName, cs1, cs2, cs3 );
		cs5 += cs4;
		if( i < nElements-1 ) { cs5 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%SemaphoreConstructors%", cs5 );

	/////////////////////////////////////////////////////////////////////////
	//  %BootSemaphores% and %NumBootSemaphores%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumSemaphores();
	cs1 = "";
	cs1 = itoa(nElements, max_active, 10);
	SourceFile.ReplaceToken( "%NumBootSemaphores%", cs1 );

	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkSemaphore& Semaphore = g_VdkData->GetSemaphore( i );

		csName = Semaphore.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Boot semaphore with no name found");
		VERIFY( Semaphore.GetProperty("Initial Value", cs1) );
		// the default Max Count is UINT_MAX so, if there is not a value, then UINT_MAX 
		// will be used
		VERIFY( Semaphore.GetProperty("Max Count", cs2) );
		VERIFY( Semaphore.GetProperty("Initial Delay", cs3) );
		VERIFY( Semaphore.GetProperty("Period",        cs4) );

		cs5.Format( "        { k%s, %s, %s, %s, %s }",
					csName, cs1, cs2, cs3, cs4 );
		cs6 += cs5;
		if( i < nElements-1 ) { cs6 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%BootSemaphores%", cs6 );

	/////////////////////////////////////////////////////////////////////////
	//  %MemoryPools% and %NumBootMemPools%
	/////////////////////////////////////////////////////////////////////////
	nElements = g_VdkData->GetNumMemoryPools();
	
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = "";

    int message_pool = 0;
	unsigned int number_of_messages=0;
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
		 number_of_messages += libConfig->GetNumMessages();

	if (number_of_messages >0)
	
	{ 
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			VERIFY( g_VdkData->GetSystemData().GetProperty("Messages Heap",cs7) );
			cs5.Format("        { kReservedPool, sizeof(VDK::Message), %d, false, %s }",number_of_messages,choose_heap_string(cs7)); 
		}
		else {
			cs5.Format("        { kReservedPool, sizeof(VDK::Message), %d, false }",number_of_messages); 
		}
		cs6+=cs5;
		if (nElements >0) cs6 += ",\r\n";
		message_pool = 1;
	}

	cs1 = "";
	cs1 = itoa((message_pool == 1)? nElements + 1 : nElements, max_active, 10);
	SourceFile.ReplaceToken( "%NumBootMemPools%", cs1 );

	for( i=0; i<nElements; i++ )
	{
		CVdkMemoryPool& MemoryPool = g_VdkData->GetMemoryPool( i );

		csName = MemoryPool.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Boot memory pool with no name found");
		VERIFY( MemoryPool.GetProperty("Block Size", cs1) );
		VERIFY( MemoryPool.GetProperty("Number of Blocks", cs2) );
		VERIFY( MemoryPool.GetProperty("Initialize on Pool Creation",    cs3) );
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			VERIFY( MemoryPool.GetProperty("Heap",    cs4) );

			cs5.Format( "        { k%s, %s, %s, %s, %s }",
						csName, cs1, cs2, cs3, cs4 );
		}
		else { 
			cs5.Format( "        { k%s, %s, %s, %s }",
						csName, cs1, cs2, cs3 );
		}
		cs6 += cs5;
		if( i < nElements-1 ) { cs6 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%MemoryPools%", cs6 );

	/////////////////////////////////////////////////////////////////////////
	//  %InitialEventBitStates%, %NumEventBits% and %EventMask%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumEventBits();
	cs4.Format( "%d", nElements );
	cs1 = cs2 = cs3 = cs5 = cs6 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkEventBit& Bit = g_VdkData->GetEventBit( i );

		csName = Bit.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Eventbit with no name found");
		VERIFY( Bit.GetProperty("Initial Value", cs1) );

		cs2.Format( "        %s << k%s    |    // Initial value of %s == %s",
					cs1, csName, csName, cs1 );
		cs3 += cs2;
		if( i < nElements-1 ) { cs3 += "\r\n"; }
	}

	SourceFile.ReplaceToken( "%InitialEventBitStates%", cs3 );
	SourceFile.ReplaceToken( "%NumEventBits%", cs4 );

	/////////////////////////////////////////////////////////////////////////
	//  %EventConstructors%
	/////////////////////////////////////////////////////////////////////////

	CStdString csBitName, csBitValue;
	nElements = g_VdkData->GetNumEvents();
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkEvent& Event = g_VdkData->GetEvent( i );

		csName = Event.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Event with no name found");
		VERIFY( Event.GetProperty("Type", cs1) );
		cs1 = (cs1 == "All") ? "true" : "false";
		cs2 = cs3 = cs7 = "";

		for( unsigned int j=0; j<Event.GetNumBits(); j++ )
		{
			Event.GetBit( j, csBitName, csBitValue );

			// value
			cs2.Format( "%s << k%s |\r\n            ", csBitValue, csBitName );
			cs3 += cs2;

			// mask
			cs6.Format( "1 << k%s |\r\n            ", csBitName );
			cs7 += cs6;
		}

		cs4.Format( "        Event( k%s, %s, \r\n            ( // event mask\r\n            %s0 ), "
					"\r\n            ( // event bits \r\n            %s0 ) ),\r\n \r\n",
					csName, cs1, cs7, cs3 );

		cs5 += cs4;
	}

	SourceFile.ReplaceToken( "%EventConstructors%", cs5 );

	/////////////////////////////////////////////////////////////////////////
	//  %NumDevices%, and %IODeviceTemplates%
	/////////////////////////////////////////////////////////////////////////

	CStdString csFlagName, csFlagValue;
	UINT nFlags = 0;
	nElements = g_VdkData->GetNumDeviceDrivers();
	cs1 = cs2 = cs3 = cs4 = "";
	CStdString csFunctionName ="";

	for( i=0; i<nElements; i++ )
	{
		CVdkDeviceDriver& Device = g_VdkData->GetDeviceDriver( i );

		csName = Device.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("I/O device with no name found");
		VERIFY( Device.GetProperty("Source File", cs4) );

		if( cs4.Find(".cpp") == -1 && cs4.Find(".cxx") == -1 )
			csFunctionName = csName + "_DriverWrapper";
		else
			csFunctionName = csName;

		cs1.Format( "        {%s::Create}", csFunctionName);
		cs2 += cs1;
		if( i < nElements-1 ) { cs2 += ",\r\n"; }
	}
	if( cs2.IsEmpty() ) cs2 = "        0 // no I/O objects have been defined";

	cs3.Format( "%d", nElements );

	SourceFile.ReplaceToken( "%NumDevices%", cs3 );
	SourceFile.ReplaceToken( "%MaxNumIOTemplates%", cs3 );
	SourceFile.ReplaceToken( "%NumDeviceFlags%", cs4 );
	SourceFile.ReplaceToken( "%IODeviceTemplates%", cs2 );
  
	/////////////////////////////////////////////////////////////////////////
	//  Set Boot IO objects
	/////////////////////////////////////////////////////////////////////////

	CStdString csIOTemplate;
	nElements = g_VdkData->GetNumBootIOObjects();
	cs1 = cs2 = cs3 = cs4 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkBootIOObject& BootIOObject = g_VdkData->GetBootIOObject( i );

		csName = BootIOObject.GetName();
		VERIFY( BootIOObject.GetProperty("Template",csIOTemplate ) );
		VERIFY( BootIOObject.GetProperty("Initializer",cs3) );

		cs1.Format( "        {k%s, k%s, %s }",csIOTemplate,csName, cs3);
		cs2 += cs1;
		if( i < nElements-1 ) { cs2 += ",\r\n"; }
	}
	
	SourceFile.ReplaceToken( "%BootIOTemplates%", cs2 );


	/////////////////////////////////////////////////////////////////////////
	//  %HeapInit%
	/////////////////////////////////////////////////////////////////////////

	nElements = g_VdkData->GetNumHeaps();
	cs1 = cs2 = cs3 = "";

	for( i=0; i<nElements; i++ )
	{
		CVdkHeap& Heap = g_VdkData->GetHeap( i );

		csName = Heap.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Heap with no name found");
		VERIFY( Heap.GetProperty("ID", cs1) );

		// the 0 are the heap indexes which will be filled in main

		if (!strcmp(g_VdkPlatformInfo.GetFamily(),"SHARC")) // in SHARC we have strings so we need the quotes
			cs2.Format( "        { \"%s\", 0 }", cs1 );
		else
			cs2.Format( "        { %s, 0 }", cs1 );
		cs3 += cs2;
		if( i < nElements-1 ) { cs3 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%HeapInit%", cs3 );

  	/////////////////////////////////////////////////////////////////////////
  	//  %LocalProcessorID%, 
  	/////////////////////////////////////////////////////////////////////////

	VERIFY( g_VdkData->GetMultiprocessorInfo().GetProperty("Local Processor ID", cs1) );
	SourceFile.ReplaceToken( "%LocalProcessorID%", cs1 );

  	/////////////////////////////////////////////////////////////////////////
  	//  %MaxNumActiveMessages%
  	////////////////////////////////////////////////////////////////////////
	VERIFY( g_VdkData->GetSystemData().GetProperty("Maximum Messages", cs1) );
	SourceFile.ReplaceToken( "%MaxNumActiveMessages%", cs1 );



  	/////////////////////////////////////////////////////////////////////////
  	//  %BootFunctionPointers%, 
  	/////////////////////////////////////////////////////////////////////////
  	//
	cs1 = cs2= cs3 = cs4 = cs5 = cs6 = cs7 = "";
	CStdString TSTimerPtr= "VDK::SetTimer";
  	bool isEvent,isMemoryPool,isSemaphore, isDeviceFlag,isBootIOObjects,isMessages,isRoutingThreads,initialiseTimer,isRoundRobin;

	(g_VdkData->GetNumMessages() > 0)      ? isMessages = true      : isMessages = false;

	(g_VdkData->GetNumEvents() > 0)      ? isEvent = true      : isEvent = false;
  	(g_VdkData->GetNumSemaphores() > 0)  ? isSemaphore = true  : isSemaphore = false;
  	(g_VdkData->GetNumMemoryPools() > 0) ? isMemoryPool = true : isMemoryPool = false;
  	(g_VdkData->GetNumDevFlags() > 0) ? isDeviceFlag = true : isDeviceFlag = false;
  	(g_VdkData->GetNumBootIOObjects() > 0) ? isBootIOObjects = true : isBootIOObjects = false;
	(g_VdkData->GetNumPriorities() > 0) ? isRoundRobin = true : isRoundRobin = false;
	// if there are no more than one routing node, then there shouldn't be any routing threads
  	(g_VdkData->GetNumRoutingThreads() > 0 && g_VdkData->GetNumRoutingNodes() > 1) ? isRoutingThreads = true : isRoutingThreads = false;
	// Initialise IMASK. Initialise also the timerunless users set the timer interrupt as none.
	g_VdkData->GetSystemData().GetProperty("Timer Interrupt",cs6);

	// if the timer interrupt chosen is the default one (or one that uses the same timer)
	// we need to set it up
	if (cs6 == g_VdkPlatformInfo.GetDefaultTimer() || 	
		((csFamily.CompareNoCase("tigersharc") == 0) && cs6.Find("TIMER",0) != -1) ||
		((csFamily.CompareNoCase("sharc") == 0) && cs6.Find("TMZ",0) != -1))  
	{
		initialiseTimer =true;
	}
	else {
		initialiseTimer = false;
	}

	if(isMemoryPool || isMessages ) 
  		cs1 += "            VDK::InitBootMemoryPools,\r\n";
	
	if (isSemaphore) 
  		cs1 += "            VDK::InitBootSemaphores,\r\n";

	if (isDeviceFlag)
  		cs1 += "            VDK::InitBootDeviceFlags,\r\n";

	if (isRoundRobin)
		cs1 += "            VDK::InitRoundRobin,\r\n";

	if (isBootIOObjects)
  		cs1 += "            VDK::InitBootIOObjects,\r\n";

  	// TODO: thread templates 
	cs1 += "            VDK::InitBootThreads,\r\n";

	if (isRoutingThreads)
  		cs1 += "            VDK::InitRoutingThreads,\r\n";		

	if (csFamily.CompareNoCase("SHARC") == 0 || csProcessor == "ADSP-TS101")
		cs1 += "            VDK::InitISRMask,\r\n";

	if (initialiseTimer)
	{
		if ((csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || 
			csProcessor == "ADSP-TS203") && (cs6 == "INT_TIMER0H_64" || cs6 == "INT_TIMER0L_64")) 
		{
			cs1 += "            VDK::SetTimer0\r\n";
			TSTimerPtr = "VDK::SetTimer0";
		}
		else
		{
			cs1 += "            VDK::SetTimer\r\n";
		}

	}

	// fill in the function pointer to the right SetTimer function for TS20x
	if (csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || 
		csProcessor == "ADSP-TS203") 
	{
		SourceFile.ReplaceToken( "%TimerFuncPtr%",
			"    VDK::BootFuncPointers g_pfTS20xSetTimer = " 
			+ TSTimerPtr + ";\r\n");
	} 
	else 
	{
		SourceFile.ReplaceToken( "%TimerFuncPtr%","");

	}
  	SourceFile.ReplaceToken( "%BootFunctionPointers%", cs1 );

  

	/////////////////////////////////////////////////////////////////////////
	//  %DefinedInterrupts% and %InstantiatedInterrupts%
	/////////////////////////////////////////////////////////////////////////
	vector <CStdString> csa;
	vector <CStdString> csa2;
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = cs8 = cs9 = cs10 = "";

    // If the stack overflow interrupt has been configured by the user we 
    // let him decide if it is set up at boot time. This is used in 
    // %StackOverflowInterrupt%

    CStdString  csSOVInterrupt = g_VdkPlatformInfo.GetStackOverflowInterrupt();
    bool bEnableSOVISR = true;

	for ( CVdkConfiguration* conf = g_first_config_file; conf; conf=conf->GetNextLibrary())
	{
		nElements = conf->GetNumISRs();

		for( i=0; i<nElements; i++ )
		{
			CVdkISR& ISR = conf->GetISR( i );

			csName = ISR.GetName();
			if (csProcessor == "ADSP-TS201" || csProcessor == "ADSP-TS202" || csProcessor == "ADSP-TS203") 
			{
				if (csName == "INT_TIMER0H_64" ||csName == "INT_TIMER0L_64") 
				{
					CStdString  SiRev = g_VdkPlatformInfo.GetSiliconRevision();
					if ( SiRev.CompareNoCase("automatic") != 0 && ((atof((LPCSTR) SiRev) < 1) || SiRev.CompareNoCase("any") == 0)) 
					{
						VDKGEN_ERROR_NO_FILE((LPCSTR)(csName + " can only be used in si-revision 1.0 and higher" ));
						return (false);
					}
				}
			}

			if (csName == "") VDKGEN_ERROR_NO_FILE("ISR with no name found");
			VERIFY( ISR.GetProperty("Entry Point", cs1) );
			VERIFY( ISR.GetProperty("Enabled at Boot", cs2) );
			if( cs2 == "true" && ISR.GetInterruptListNum() == 1) csa.push_back( csName );
			else if( cs2 == "true" && ISR.GetInterruptListNum() == 2) csa2.push_back( csName );
			VERIFY( ISR.GetProperty("Source File", cs8));
			if (cs8.Find(".s") != -1 || cs8.Find(".asm") != -1 ) {
				cs9.Format("INT_ASM_ROUT(%s)\r\n", cs1);
				cs3.Format( "    INTVECTOR( seg_%s, %s )", csName, cs1 );

			}
			else 
			{
				if (cs8.Find(".cpp") != -1 || cs8.Find("cxx")!= -1) {
					cs9.Format("INT_CPP_ROUT(%s)\r\n",cs1);	
					cs3.Format( "    INTVECTOR_CPP( seg_%s, %s )", csName, cs1 );

				}
				else {
					cs9.Format("INT_C_ROUT(%s)\r\n",cs1);
					cs3.Format( "    INTVECTOR_C( seg_%s, %s )", csName, cs1 );
				}
			}
			cs10 += cs9;
			cs5 += cs3;
			if( i < nElements-1 || (conf->GetNextLibrary() && 
				conf->GetNextLibrary()->GetNumISRs() >0)) 
			{ cs5 += "\r\n"; }

            // check if this was the stack overflow interrupt and whether 
            // we need to set it
            if (bEnableSOVISR == true && csName == csSOVInterrupt) 
            {
                bEnableSOVISR = false;
            }
		}
	}

	for( i=0; i<(unsigned int) csa.size(); i++ )
	{
		cs3.Format( "        %s |", csa[i] );
		cs4 += cs3;
		if( i < (unsigned int) (csa.size()-1) ) { cs4 += "\r\n"; }
	}

	for( i=0; i<(unsigned int) csa2.size(); i++ )
	{
		cs6.Format( "        %s |", csa2[i] );
		cs7 += cs6;
		if( i < (unsigned int) (csa2.size()-1) ) { cs7 += "\r\n"; }
	}

	// We need the prototypes of the ISRs indicating if they are C, asm or C++
	// cs10 has a list of all the user defined functions with the language specified
	CStdString csUserInterrupts = "";
	csUserInterrupts = cs10 ;
	csUserInterrupts += "\r\nINT_ASM_ROUT(_tmk_TimerISR)";

	SourceFile.ReplaceToken( "%UserDefinedISRPrototypes%", csUserInterrupts );
	SourceFile.ReplaceToken( "%DefinedInterrupts%", cs5 );
	SourceFile.ReplaceToken( "%InstantiatedInterrupts%", cs4 );
	SourceFile.ReplaceToken( "%InstantiatedInterrupts2%", cs7);

        /////////////////////////////////////////////////////////////////////////
        //  %StackOverflowInterrupt%
        /////////////////////////////////////////////////////////////////////////
        // in SHARC we need the stack overflow interrupt (CB7I) always on so we need the IMASK 
        // for it. bEnableSOVISR comes from looking at the enabled interrupts and seeing if a customer asked
        // for this one to be false
        if (g_VdkPlatformInfo.GetStackOverflowInterrupt() != "" && bEnableSOVISR == true)
            SourceFile.ReplaceToken( "%StackOverflowInterrupt%",g_VdkPlatformInfo.GetStackOverflowInterrupt() + " |");
        else
            SourceFile.ReplaceToken( "%StackOverflowInterrupt%","");

	/////////////////////////////////////////////////////////////////////////
	//  %RescheduleInterrupt%
	/////////////////////////////////////////////////////////////////////////
	// in SHARC we need two reschedule interrupts so we need the IMASK for both
	if (g_VdkPlatformInfo.GetRescheduleInterrupt2() != "")
		SourceFile.ReplaceToken( "%RescheduleInterrupt%",g_VdkPlatformInfo.GetRescheduleInterrupt() + 
		"|\r\n        " + g_VdkPlatformInfo.GetRescheduleInterrupt2());
	else
		SourceFile.ReplaceToken( "%RescheduleInterrupt%",g_VdkPlatformInfo.GetRescheduleInterrupt());

	/////////////////////////////////////////////////////////////////////////
	//  %ChannelRoutingMask%
	/////////////////////////////////////////////////////////////////////////
	SourceFile.ReplaceToken( "%ChannelRoutingMask%", "0");

	/////////////////////////////////////////////////////////////////////////
	//  %MarshalledMessageTypes%
	/////////////////////////////////////////////////////////////////////////


	// create a list with all the marshalled types from all the configurations

	for (CVdkConfiguration* config=g_first_config_file;config;config=config->GetNextImport()) {
		for (unsigned int num=0;num<config->GetNumMarshalledMessages();num++) {
			g_all_messages_list.push_back(config->GetMarshalledMessage(num).GetName());

		}
	}

	// sort list i alphabetical order and eliminate duplicates
	g_all_messages_list.sort();
	g_all_messages_list.unique();

	// now dump the constructors
	cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 ="";
	CStdString csSource="";

	bool custom_messages=false;

	// if there are no messages we need to fake one as the array is still there
	if (g_all_messages_list.size() == 0) {
		cs4.Format("        {0,0}");
	}
	
	nElements=0;

	for ( list<CStdString>::iterator theIterator = g_all_messages_list.begin(); 
			theIterator != g_all_messages_list.end(); theIterator++,nElements++)
	{
		if (CVdkMarshalledMessage* Message = g_first_config_file->FindMarshalledMessage((LPCSTR) *theIterator)) {
			Message->GetProperty("Alloc Type",cs3);
			VERIFY( Message->GetProperty("Marshalling Function", cs1) );
			
			if (cs3.CompareNoCase("custom") == NULL) {
				cs5.Format("extern \"C\" int %s (VDK::MarshallingCode,"
                         "VDK::MsgWireFormat *,VDK::DeviceInfoBlock *,"
						 "unsigned int, VDK::Ticks);\r\n",cs1);
				cs7 += cs5;
			}


			VERIFY( Message->GetProperty("Alloc Area", cs2) );
			if (cs3.CompareNoCase("pool") == NULL || cs3.CompareNoCase("heap") == NULL ) 
				cs2 = "k" + cs2;
			if (cs3.CompareNoCase("clusterbus")== NULL) 
				cs2 = "0";
			cs3.Format( "        {%s, %s}", cs1, cs2 );

		}
		else {
			// the message type is defined in an import so we don't give any parameters to its constructor
			cs3.Format( "        {0, 0}");
		}

		cs4 += cs3;
	
		if( nElements < g_all_messages_list.size() -1 ) { cs4 += ",\r\n";}
	}

	SourceFile.ReplaceToken( "%CustomMarshalledIncludes%",cs7);

	SourceFile.ReplaceToken( "%MarshalledMessagesConstructor%", cs4 );
	{
		char num_marsh[10];
		SourceFile.ReplaceToken( "%NumMarshalledTypes%", itoa(g_all_messages_list.size(),num_marsh,10));
	}
	/////////////////////////////////////////////////////////////////////////
	//  %NumRoutingThreads% 
	/////////////////////////////////////////////////////////////////////////
	csName = cs1 = cs2 = cs3 = cs4 = cs5 = cs6 = cs7 = cs8 = cs9 = cs10 = cs11 = cs12 = cs13 = cs14 = "";

	nElements = g_VdkData->GetNumRoutingThreads();

	// if the number of routing nodes is 0, the IDDE does not allow to have routing 
	// threads but it doesn't remove them from the .vdk file. To work around this,
	// we just don't dump them out.

	if (g_VdkData->GetNumRoutingNodes() == 1) nElements=0;

	cs1.Format( "%d", nElements );
	SourceFile.ReplaceToken( "%NumRoutingThreads%", cs1 );

	if (nElements == 0) {
		// we dump a fake entry so the compiler does not complain. we still need
		// to know how many elements we need

		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) 
		{// 219x does not have multiple heaps

			if ( g_VdkPlatformInfo.GetNumContextAreas() == 1) {
				cs6 = "system_heap";
			}
			else if ( g_VdkPlatformInfo.GetNumContextAreas()> 1) {
					cs6= "system_heap0"; cs7 = "system_heap1";
			}

			if ( g_VdkPlatformInfo.GetNumThreadStacks() == 1) {
				cs8 = "system_heap";
			}
			else if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1) {
		 		cs8 = "system_heap0";	cs9 = "system_heap1";
			}

			if ( g_VdkPlatformInfo.GetNumContextAreas() > 1 || g_VdkPlatformInfo.GetNumThreadStacks() > 1)
				cs13 = "system_heap0"; // for the thread structure
			else
				cs13 = "system_heap";

			cs12.Format( "        INIT_RTHREAD_(NULL, (VDK::Priority) 0, 0,%s  NULL %s %s%s%s%s, false, (VDK::ThreadID) 0, (VDK::IOID) 0,\"0\", (VDK::RoutingDirection) 0)",
				(g_VdkPlatformInfo.GetNumThreadStacks() > 1)? "0,":"",choose_heap_string(cs6,true) , choose_heap_string(cs7,true), 
				choose_heap_string(cs8,true), choose_heap_string(cs9,true), choose_heap_string(cs13,true));
		}
		else
		{

			cs12.Format( "        INIT_RTHREAD_(NULL, (VDK::Priority) 0, 0,%s  NULL , false, (VDK::ThreadID) 0, (VDK::IOID) 0,\"0\", (VDK::RoutingDirection) 0)",
				(g_VdkPlatformInfo.GetNumThreadStacks() > 1)? "0,":"");

		}
	}

	for( i=0; i<nElements; i++ )
	{
		CVdkRoutingThread& RoutThread = g_VdkData->GetRoutingThread( i );

		csName = RoutThread.GetName();
		if (csName == "") VDKGEN_ERROR_NO_FILE("Routing Thread with no name found");
		VERIFY( RoutThread.GetProperty("I/O Object", cs1) );
		VERIFY( RoutThread.GetProperty("OpenDevice String", cs2) );
		VERIFY( RoutThread.GetProperty("Message Flow", cs3) );
		if (cs3.CompareNoCase("outgoing") == 0 ) {
			cs3="kOUTGOING";
		}
		else {
			cs3="kINCOMING";
		}
		VERIFY( RoutThread.GetProperty("Priority", cs4) );
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			if ( g_VdkPlatformInfo.GetNumContextAreas() == 1) {
				VERIFY( RoutThread.GetProperty("Context Heap",cs6));
			}
			else if ( g_VdkPlatformInfo.GetNumContextAreas()> 1) {
				VERIFY( RoutThread.GetProperty("Context0 Heap",cs6));
				VERIFY( RoutThread.GetProperty("Context1 Heap",cs7));
			}

			if ( g_VdkPlatformInfo.GetNumThreadStacks() == 1) {
				VERIFY( RoutThread.GetProperty("Stack Heap",cs8));
			}
			else if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1) {
				VERIFY( RoutThread.GetProperty("Stack0 Heap",cs8));
			 	VERIFY( RoutThread.GetProperty("Stack1 Heap",cs9));
				//VERIFY( RoutThread.GetProperty("Stack1 Size",cs14));
				//cs14 += ", ";
			}
			if ( g_VdkPlatformInfo.GetNumStackSizes() == 1) {
				VERIFY( RoutThread.GetProperty("Stack Size", cs5) );

			} else 	if ( g_VdkPlatformInfo.GetNumStackSizes() > 1) {
				VERIFY( RoutThread.GetProperty("Stack0 Size", cs5) );
				VERIFY( RoutThread.GetProperty("Stack1 Size", cs14) );
				cs14 += ", ";
			}
			VERIFY( RoutThread.GetProperty("Thread Structure Heap",cs10));
		}


		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) // 219x does not have multiple heaps
			cs11.Format( "        INIT_RTHREAD_(\"%s RThread\", kPriority%s, %s, %sNULL %s%s%s%s%s, %s,k%s, k%s,\"%s\", %s )",
			(cs3.CompareNoCase("kINCOMING") == 0)? "Incoming":"Outgoing",cs4,cs5,cs14,
				choose_heap_string(cs6,true) , choose_heap_string(cs7,true), 
				choose_heap_string(cs8,true), choose_heap_string(cs9,true), choose_heap_string(cs10,true), 
				(cs3 == "kOUTGOING")?"true":"false",csName, cs1 /*device*/, cs2 /*open arg*/,cs3);
		else
			cs11.Format( "        INIT_RTHREAD_(\"%s RThread\", kPriority%s, %s,NULL, %s,k%s, k%s,\"%s\", %s )",
			(cs3.CompareNoCase("kINCOMING") == 0)? "Incoming":"Outgoing",cs4,cs5,
				(cs3 == "kOUTGOING")?"true":"false",csName, cs1 /*device*/, cs2 /*open arg*/,cs3);
		cs12 += cs11;
		if( i < nElements-1 ) { cs12 += ",\r\n"; }
	}

	SourceFile.ReplaceToken( "%RoutingThreadConstructor%", cs12 );

	/////////////////////////////////////////////////////////////////////////
	//  %NumRoutingNodes% 
	/////////////////////////////////////////////////////////////////////////
	csName = cs1 = cs2 = cs3 = cs4 = cs5 = "";
	nElements = g_VdkData->GetNumRoutingNodes();
	cs1.Format( "%d", nElements );
	SourceFile.ReplaceToken( "%NumRoutingNodes%", cs1 );

	for( i=0; i<nElements; i++ )
	{
		CStdString nodeName;
		csName.Format("RoutingNode%d",i);
		CVdkRoutingNode* RoutNode = g_VdkData->FindRoutingNode(csName);

		if (RoutNode) VERIFY( RoutNode->GetProperty("Routing Thread", cs1) );
				
		if (cs1 == "0") 
		// the local processor will have routing thread -1 and won't need the k
			cs2.Format( "        (VDK::ThreadID) %s ", cs1 );
		else
			cs2.Format( "         k%s ", cs1 );
		// for now the second routing table will be populated with 0-s
		cs4 += "        (VDK::ThreadID) 0";
		cs3 += cs2;
		if( i < nElements-1 ) { cs4+=",\r\n"; cs3 += ",\r\n"; }
	}

	if (nElements == 0) {
		cs3.Format("        (ThreadID) 0");
		cs4.Format("        (ThreadID) 0");
	}
	SourceFile.ReplaceToken( "%RoutingTableConstructor%", cs3 );
	SourceFile.ReplaceToken( "%RoutingTable2Constructor%", cs4 );

	SourceFile.Close();
	return( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// BOOL GenerateConfigHeader()
/////////////////////////////////////////////////////////////////////////////

BOOL GenerateConfigHeader( LPCTSTR szProjectPath )
{
	CVdkTemplateFile HeaderFile;

	CStdString csProjectPath( szProjectPath );
	csProjectPath.ReleaseBuffer();

	UINT nElements = 0;
	CStdString cs1, cs2, cs3, cs4, csName,processorID;
	CStdString csTemplateBase = g_VdkPlatformInfo.GetTemplatePath();

    // variable that will hold the first configuration when we need to walk 
	// through libraries
	CVdkConfiguration *libConfig;

	// variable to calculate the max num of threads, semaphores, mempools,...
	int total_number = 0; 

	// string to hold the maximum number of threads, semaphores, mempools,...
	char max_active[10]; 


	/////////////////////////////////////////////////////////////////////////
	//  Generate the Header File
	/////////////////////////////////////////////////////////////////////////

	if( !HeaderFile.Create( csProjectPath + "VDK.h", csTemplateBase + "VDK.h.tf") )
	{
		CStdString csMsg;
		csMsg.Format( "An error has occured while trying to create the header file\r\n\r\n"
					  "'%s'"
					  "\r\n\r\nPlease make sure that the path is valid and that\r\n"
					  "the directory is writable.", csProjectPath + "VDK.h" );
 		return( FALSE );
	}

	HeaderFile.EnableBuffering( TRUE );

	/////////////////////////////////////////////////////////////////////////
	//  %VDKGenVersion
	////////////////////////////////////////////////////////////////////////

	HeaderFile.ReplaceToken( "%VDKGenVersion%", VdkGenVersion );

	/////////////////////////////////////////////////////////////////////////
	//  %CreationTime% and %Year%
	/////////////////////////////////////////////////////////////////////////

	time_t sTime;
	time(&sTime);

	struct tm *pTime = localtime( &sTime );
	char strTime[MAX_PATH], strTime2[MAX_PATH];

	strftime(strTime,MAX_PATH,"%A, %B %d, %Y - %I:%M:%S %p", pTime);
	strftime(strTime2,MAX_PATH,"%Y", pTime);

	HeaderFile.ReplaceToken( "%CreationTime%", strTime );
	HeaderFile.ReplaceToken( "%Year%", strTime2 );

	/////////////////////////////////////////////////////////////////////////
	//  %TargetProcessor%
	/////////////////////////////////////////////////////////////////////////

	cs1 = g_VdkPlatformInfo.GetPlatform();
	// if the name of the processor starts with ADSP- ignore it. This is not
	// the case for AD6532 for example

    if (!strncmp(cs1,"ADSP-",5))
		HeaderFile.ReplaceToken( "%TargetProcessor%", cs1.Right(cs1.GetLength()-5) );
	else
		HeaderFile.ReplaceToken( "%TargetProcessor%", cs1.Right(cs1.GetLength()) );

	/////////////////////////////////////////////////////////////////////////
	//  %EventsUsed%
	/////////////////////////////////////////////////////////////////////////

	LPCTSTR szValue, szEndValue;
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumEvents() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}

	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%EventsUsed%", szValue );
	HeaderFile.ReplaceToken( "%EventsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %BootSemaphoresUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumSemaphores() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}

	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%BootSemaphoresUsed%", szValue );
	HeaderFile.ReplaceToken( "%BootSemaphoresUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %SemaphoresUsed%
	/////////////////////////////////////////////////////////////////////////

	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ((libConfig->GetMaxActiveSemaphores() | libConfig->GetNumSemaphores()) > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}

	if (libConfig == 0) {
	// we have not found semaphores so far. But we still need them if 
	// there is multiprocessor messaging
		for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
		{
			libConfig->GetMultiprocessorInfo().GetProperty("Multiprocessor Messages Allowed", cs1);
			if (cs1.CompareNoCase("true") == 0) 
			{
                                szValue = "";
								szEndValue = "";
                                break;
            }
        }
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}

	HeaderFile.ReplaceToken( "%SemaphoresUsed%", szValue );
	HeaderFile.ReplaceToken( "%SemaphoresUsedEnd%", szEndValue );


	/////////////////////////////////////////////////////////////////////////
	//  %BootMemoryPoolsUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ((libConfig->GetNumMemoryPools() | libConfig->GetNumMessages()) > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%BootMemoryPoolsUsed%", szValue );
	HeaderFile.ReplaceToken( "%BootMemoryPoolsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %MemoryPoolsUsed%
	/////////////////////////////////////////////////////////////////////////

	bool memPoolUsed = false;
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ((libConfig->GetMaxActiveMemoryPools() | libConfig->GetNumMemoryPools() | libConfig->GetNumMessages()) > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) {
		szValue = "/*";
		szEndValue = "*/";
		memPoolUsed = false;
	}
	else {
		memPoolUsed = true;
	}
	HeaderFile.ReplaceToken( "%MemoryPoolsUsed%", szValue );
	HeaderFile.ReplaceToken( "%MemoryPoolsUsedEnd%", szEndValue );


	/////////////////////////////////////////////////////////////////////////
	//  %BootIOObjectsUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumBootIOObjects() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%BootIOObjectsUsed%", szValue );
	HeaderFile.ReplaceToken( "%BootIOObjectsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %IOTemplatesUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetMaxActiveDrivers() > 0 && libConfig->GetNumDeviceDrivers()  > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%IOTemplatesUsed%", szValue );
	HeaderFile.ReplaceToken( "%IOTemplatesUsedEnd%", szEndValue );


	/////////////////////////////////////////////////////////////////////////
	//  %MPMessagedUsed%
	/////////////////////////////////////////////////////////////////////////
	cs1.Empty();
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		libConfig->GetMultiprocessorInfo().GetProperty("Multiprocessor Messages Allowed", cs1);
			if (cs1.CompareNoCase("true") == 0) {
				szValue = "";
				szEndValue = "";
				break;
			}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%MPMessagesUsed%", szValue );
	HeaderFile.ReplaceToken( "%MPMessagesUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %BootDevFlagsUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumDevFlags() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%BootDevFlagsUsed%", szValue );
	HeaderFile.ReplaceToken( "%BootDevFlagsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %DevFlagsUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ((libConfig->GetMaxActiveDevFlags() | libConfig->GetNumDevFlags()) > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%DevFlagsUsed%", szValue );
	HeaderFile.ReplaceToken( "%DevFlagsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %MessagesUsed%
	/////////////////////////////////////////////////////////////////////////
	bool messagesUsed = false;
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumMessages() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) { 
		szValue = "/*";
		szEndValue = "*/";
		messagesUsed = false;
	}
	else {
		messagesUsed = true; // used to see if we need a pool for the messages
	}
	HeaderFile.ReplaceToken( "%MessagesUsed%", szValue );
	HeaderFile.ReplaceToken( "%MessagesUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %MultipleHeapsUsed%
	/////////////////////////////////////////////////////////////////////////
	szValue = (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) ? "" : "/*";
	szEndValue = (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) ? "" : "*/";
	HeaderFile.ReplaceToken( "%MultipleHeapsUsed%", szValue );
	HeaderFile.ReplaceToken( "%MultipleHeapsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %MultiprocesorMessagingUsed%
	/////////////////////////////////////////////////////////////////////////
	cs1.Empty();
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		libConfig->GetMultiprocessorInfo().GetProperty("Multiprocessor Messages Allowed", cs1);
			if (cs1.CompareNoCase("true") == 0) {
				szValue = "";
				szEndValue = "";
				break;
			}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%MultiprocessorMessagingUsed%", szValue );
	HeaderFile.ReplaceToken( "%MultiprocessorMessagingUsedEnd%", szEndValue );
		
	/////////////////////////////////////////////////////////////////////////
	//  %RoutingThreadsUsed%
	/////////////////////////////////////////////////////////////////////////

	// if there are no more than one routing node, then there shouldn't be any routing threads
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumRoutingThreads () > 0 && libConfig->GetNumRoutingNodes() > 1) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%RoutingThreadsUsed%", szValue );
	HeaderFile.ReplaceToken( "%RoutingThreadsUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %RoutingNodesUsed%
	/////////////////////////////////////////////////////////////////////////

	// there is always one node (current node) so we check that there is more than one
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumRoutingNodes() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%RoutingNodesUsed%", szValue );
	HeaderFile.ReplaceToken( "%RoutingNodesUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %MarshalledMessagesUsed%
	/////////////////////////////////////////////////////////////////////////
	for (libConfig = g_first_config_file; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if (libConfig->GetNumMarshalledMessages() > 0) {
			szValue = "";
			szEndValue = "";
			break;
		}
	}
	if (libConfig == 0) 
	{ 
		szValue = "/*";
		szEndValue = "*/";
	}
	HeaderFile.ReplaceToken( "%MarshalledMessagesUsed%", szValue );
	HeaderFile.ReplaceToken( "%MarshalledMessagesUsedEnd%", szEndValue );

	/////////////////////////////////////////////////////////////////////////
	//  %InstrumentationLevel%
	/////////////////////////////////////////////////////////////////////////

	VERIFY( g_VdkData->GetSystemData().GetProperty("Instrumentation Level", cs1) );
	if (cs1 == "Full Instrumentation") szValue = "2";
	else if (cs1== "Error Checking") szValue = "1";
	else  if (cs1== "None") szValue = "0";

	HeaderFile.ReplaceToken( "%InstrumentationLevel%", szValue );

	/////////////////////////////////////////////////////////////////////////
	//  %TimerInterrupt%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty();
	VERIFY( g_VdkData->GetSystemData().GetProperty("Timer Interrupt",cs1) );
	if (cs1 == "None") 
		HeaderFile.ReplaceToken( "%TimerInterrupt%", "" );
	else
		HeaderFile.ReplaceToken( "%TimerInterrupt%", cs1 );


	/////////////////////////////////////////////////////////////////////////
	//	%C_ThreadTypeEnumList% and %ASM_ThreadTypeEnumList%
	/////////////////////////////////////////////////////////////////////////
	unsigned int i=0;
	unsigned int num_of_thread_types =0;
	cs1.Empty(); cs2.Empty();
	libConfig = g_first_config_file; 
	while (libConfig->GetNextLibrary()) libConfig = libConfig->GetNextLibrary();
	for ( ; libConfig; libConfig=libConfig->GetPrevLibrary()) // the library types come first
	{
		nElements = libConfig->GetNumThreadTypes();

		for(i=0; i<nElements; i++ )
		{
			csName = libConfig->GetThreadType(i).GetName();
			cs4.Format("    k%s = %d", csName, num_of_thread_types);
			cs1 += cs4;
			cs3.Format( "#define k%s %d    /* enum */", csName, num_of_thread_types++);
			cs2 += cs3;

			if( i < nElements-1 || (libConfig->GetPrevLibrary() &&  
				//what if one doesn't have any but the next one does?
				libConfig->GetPrevLibrary()->GetNumThreadTypes() >0) ) 
			{ cs1 += ",\r\n"; cs2 += "\r\n"; }
		}
	}

	HeaderFile.ReplaceToken( "%C_ThreadTypeEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_ThreadTypeEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumThreads%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of threads we need to go through all the 
	// libraries we are including and see how many threads they needed so we
	// can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Running Threads", cs1) );
		total_number += libConfig->GetMaxRunningThreads();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	HeaderFile.ReplaceToken( "%MaxNumThreads%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumSemaphores%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of semaphores we need to go through all the 
	// libraries we are including and see how many semaphores they needed so we
	// can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Active Semaphores", cs1) );
		total_number += libConfig->GetMaxActiveSemaphores();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	HeaderFile.ReplaceToken( "%MaxNumActiveSemaphores%", cs1 );


	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumMemoryPools%
	/////////////////////////////////////////////////////////////////////////
	// To know the maximum number of mempools we need to go through all the 
	// libraries we are including and see how many mempools they needed so we
	// can add them to the total

	bool messages_pool_required = false;
	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		if ( messages_pool_required== false && libConfig->GetNumMessages() >0) {
			messages_pool_required = true;
			total_number +=1;
		}
		total_number += libConfig->GetMaxActiveMemoryPools();
	}

	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	HeaderFile.ReplaceToken( "%MaxNumActiveMemoryPools%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumDeviceFlags%
	/////////////////////////////////////////////////////////////////////////

	// To know the maximum number of device flags we need to go through all the 
	// libraries we are including and see how many device flags they needed so 
	// we can add them to the total

	for (libConfig = g_first_config_file, total_number = 0; libConfig!=0; libConfig = libConfig->GetNextLibrary())
	{
		VERIFY( libConfig->GetSystemData().GetProperty("Maximum Active Device Flags", cs1) );
		total_number += libConfig->GetMaxActiveDevFlags();
	}
	cs1 = "";
	cs1 = itoa(total_number, max_active, 10);
	HeaderFile.ReplaceToken( "%MaxNumActiveDevFlags%", cs1 );


	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumIOObjects%  
	/////////////////////////////////////////////////////////////////////////
	VERIFY( g_VdkData->GetSystemData().GetProperty("Maximum I/O Objects", cs1) );
	HeaderFile.ReplaceToken( "%MaxNumIOObjects%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %MaxNumActiveMessages%  
	/////////////////////////////////////////////////////////////////////////
	VERIFY( g_VdkData->GetSystemData().GetProperty("Maximum Messages", cs1) );
	HeaderFile.ReplaceToken( "%MaxNumActiveMessages%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//  %HistoryBufferSize%  
	/////////////////////////////////////////////////////////////////////////
	VERIFY( g_VdkData->GetSystemData().GetProperty("History Buffer Size", cs1) );
	HeaderFile.ReplaceToken( "%HistoryBufferSize%", cs1 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_ThreadIDEnumList% and %ASM_ThreadIDEnumList%
	/////////////////////////////////////////////////////////////////////////
	cs1.Empty(); cs2.Empty();cs3.Empty();cs4.Empty(),processorID.Empty();

	// we keep a list of all the boot thread names to see if they are repeated

	vector<CStdString> BootThreadNames;
	cs4="IdleThread";
	BootThreadNames.push_back(cs4);

	// all the thread IDs will have 4 bits for the processor ID, bits 10 to 14
	// just now

	libConfig = g_first_config_file;
	int threadID=0, processorIDMask=0;
	libConfig->GetMultiprocessorInfo().GetProperty("Local Processor ID", processorID);
	processorIDMask = atoi(processorID) << 9;


	// we do the idle thread separately because it is not in the list of boot threads
	// as we don't want it to have constructors like the others (for example it does not
	// have a type 
	cs4.Format("    kIdleThread=%d,\r\n",processorIDMask);
	cs1 += cs4;
	cs4.Format("#define kIdleThread %d\r\n",processorIDMask);
	cs2 += cs4;

	// we want to output the name of the thread in the enumeration and its value.
	// If the configuration is an import, we also want the threadname_procnum to
	// go in the enumeration. If there is a clash of names with two configurations, 
	// we will print out a warning and only the name of the thread with its processor 
	// name will be printed.

	CVdkConfiguration* importConfig;
	for ( importConfig = g_first_config_file;importConfig;importConfig = importConfig->GetNextImport()) {
		importConfig->GetMultiprocessorInfo().GetProperty("Local Processor ID", processorID);
		processorIDMask = atoi(processorID) << 9;
		nElements = importConfig->GetNumBootThreads();
		for( i=0; i<nElements; i++ )
		{
			csName = importConfig->GetBootThread(i).GetName();

			threadID = processorIDMask | (i + 1) ;
			bool thread_found = FALSE;
			for (unsigned int total_threads=0; total_threads < BootThreadNames.size();total_threads++) {
				if (BootThreadNames[total_threads] == csName) {
					cerr << "VdkGen warning: multiple definition of "<< (LPCSTR) csName 
						<< " in different processors."<< endl;
					thread_found = TRUE;
					break;
					
				}
			}

			BootThreadNames.push_back(csName);
		
			// if the thread was in a different processor we don't need to output 
			// it anymore
			if (!thread_found) {
				cs4.Format("    k%s=%d",csName,threadID);
				cs1 += cs4;
				cs3.Format( "#define k%s %d    /* enum */", csName, threadID );
				cs2 += cs3;
				if (importConfig->m_ImportedConfiguration) {
					cs1 += ",\r\n"; cs2 += "\r\n";
				}
			}

			// the imported threads have the name bootname_procprocessorID
			if (importConfig->m_ImportedConfiguration)
			{
				cs4.Format("    k%s=%d",csName + "_proc" + processorID,threadID);

				cs1 += cs4;
				cs3.Format( "#define k%s %d    /* enum */", csName + "_proc" + processorID, threadID );
				cs2 += cs3;
			}


			if( i < nElements-1 || importConfig->GetNextImport()) 
				{ cs1 += ",\r\n"; cs2 += "\r\n"; }

	// if the any config has routing threads we will need the , in the end
	// but not the return as they come out separately
	// it might only be necessary to check one config (as they will all have the rout threads)
	// but doesn't harm us.
			if ( i == nElements -1 && importConfig->GetNextImport() == 0)
				for (; importConfig!= g_first_config_file ; importConfig = importConfig->GetPrevImport())
					if (importConfig->GetNumRoutingThreads() > 0)
					{ 
						cs1 += ","; 
						break;
					}
		}

	}

	HeaderFile.ReplaceToken( "%C_ThreadIDEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_ThreadIDEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_SemaphoreEnumList% and %ASM_SemaphoreEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	unsigned int num_semaphores=0;
	for ( libConfig = g_first_config_file; libConfig; libConfig=libConfig->GetNextLibrary())
	{
		nElements = libConfig->GetNumSemaphores();

		for( i=0; i<nElements; i++ )
		{
			csName = libConfig->GetSemaphore(i).GetName();
			cs4.Format("    k%s = %d,", csName,num_semaphores);
			cs1 += cs4;
			cs3.Format( "#define k%s %d    /* enum */", csName, num_semaphores++ );
			cs2 += cs3;

			if( i < nElements-1 || (libConfig->GetNextLibrary() && 
				libConfig->GetNextLibrary()->GetNumSemaphores() >0))
			{ cs1 += "\r\n"; cs2 += "\r\n"; }
		}
	}

	HeaderFile.ReplaceToken( "%C_SemaphoreEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_SemaphoreEnumList%", cs2 );



	/////////////////////////////////////////////////////////////////////////
	//	%C_MemoryPoolEnumList% and %ASM_MemoryPoolEnumList%
	/////////////////////////////////////////////////////////////////////////

	int enum_value=0; // for the #define part we need to keep the value in case there are
					// messages and we need to reserve a pool
	cs1.Empty(); cs2.Empty();
	// first find if we need a message pool because that one would be the first one 
	// even if it comes from a library. We calculated the variable messagesUsed earlier so we use that.


	if (messagesUsed) {
		cs1 += "    kReservedPool = 0 ,";
		cs2 += "#define kReservedPool 0    /* enum */";
		if (memPoolUsed) {cs1+= "\r\n"; cs2+= "\r\n"; }
		enum_value=1; // the first pool was the one reserved for messages
	}

	for ( libConfig = g_first_config_file; libConfig; libConfig=libConfig->GetNextLibrary())
	{
		nElements = libConfig->GetNumMemoryPools();
		for( i=0; i<nElements; i++ )
		{
			csName = libConfig->GetMemoryPool(i).GetName();
			cs4.Format("    k%s = %d,", csName, enum_value);
			cs1 += cs4;
			cs3.Format( "#define k%s %d    /* enum */", csName, enum_value++ );
			cs2 += cs3;

			if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
		}
	}

	HeaderFile.ReplaceToken( "%C_MemoryPoolEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_MemoryPoolEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_MessagePortEnumList% and %ASM_MessagePortEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumMessagePorts();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetMessagePort(i).GetName();
		cs4.Format("    k%s = %d,", csName, i);
		cs1 += cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_MessagePortEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_MessagePortEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_EventEnumList% and %ASM_EventEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumEvents();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetEvent(i).GetName();
		cs4.Format("    k%s = %d,",csName,i);
		cs1 += cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_EventEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_EventEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_EventBitEnumList% and %ASM_EventBitEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumEventBits();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetEventBit(i).GetName();
		cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_EventBitEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_EventBitEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_DeviceEnumList% and %ASM_DeviceEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumDeviceDrivers();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetDeviceDriver(i).GetName();
		cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_IOTemplateEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_IOTemplateEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_BootIODeviceList% and %ASM_BootIODeviceList%
	//	Boot IO Devices
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumBootIOObjects();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetBootIOObject(i).GetName();
		cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_BootIODeviceList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_BootIODeviceList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_DeviceFlagEnumList% and %ASM_DeviceFlagEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumDevFlags();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetDeviceFlag(i).GetName();
		cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_DeviceFlagEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_DeviceFlagEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_MultipleHeapsEnumList% and %ASM_MultipleHeapsEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumHeaps();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetHeap(i).GetName();
		if (i == nElements -1) //the last one does not need a comma
			cs4.Format("    k%s = %d",csName,i);
		else
			cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_HeapEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_HeapEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_RoutThreadEnumList% and %ASM_RoutThreadEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();cs3.Empty();cs4.Empty();
	nElements = g_VdkData->GetNumRoutingThreads();
	
	// if the number of routing nodes is 0, the IDDE does not allow to have routing 
	// threads but it doesn't remove them from the .vdk file. To work around this,
	// we just don't dump them out.

	if (g_VdkData->GetNumRoutingNodes() == 1) nElements=0;

	g_first_config_file->GetMultiprocessorInfo().GetProperty("Local Processor ID", processorID);
	processorIDMask = atoi(processorID) << 9;

	for( i=0; i<nElements; i++ )
	{
		threadID = processorIDMask | (i + g_first_config_file->GetNumBootThreads() + 1) ;
		csName = g_first_config_file->GetRoutingThread(i).GetName();
		if (i == nElements -1) //the last one does not need a comma
			cs1.Format( "    k%s = %d", csName, threadID);
		else
			cs1.Format( "    k%s = %d,", csName, threadID);
		cs4 += cs1;
		// we want the numbers to go on from the boot threads
		cs3.Format( "#define k%s %d    /* enum */", csName, threadID );
		cs2 += cs3;

		if( i < nElements-1 ) { cs4 += "\r\n"; cs2 += "\r\n"; }
	}

	HeaderFile.ReplaceToken( "%C_RoutThreadEnumList%", cs4 );
	HeaderFile.ReplaceToken( "%ASM_RoutThreadEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_RoutTableEnumList% and %ASM_RoutTableEnumList%
	/////////////////////////////////////////////////////////////////////////

	cs1.Empty(); cs2.Empty();
	nElements = g_VdkData->GetNumRoutingNodes();

	for( i=0; i<nElements; i++ )
	{
		csName = g_VdkData->GetRoutingNode(i).GetName();
		if (i == nElements -1) //the last one does not need a comma
			cs4.Format("    k%s = %d",csName,i);
		else
			cs4.Format("    k%s = %d,",csName,i);
		cs1 +=cs4;
		cs3.Format( "#define k%s %d    /* enum */", csName, i );
		cs2 += cs3;

		if( i < nElements-1 ) { cs1 += "\r\n"; cs2 += "\r\n"; }
	}

	// we want to be able to define this even when there are no marshalled messages
	// so we define cs1 as non-empty 

	if (nElements == 0 )  // there were no multiple nodes
	{
		cs1 = "        kNoRoutingNodes\r\n";
	}
	HeaderFile.ReplaceToken( "%C_RoutTableEnumList%", cs1 );
	HeaderFile.ReplaceToken( "%ASM_RoutTableEnumList%", cs2 );

	/////////////////////////////////////////////////////////////////////////
	//	%C_MarshalledMessagesEnumList% and %ASM_MarshalledMessagesEnumList%
	/////////////////////////////////////////////////////////////////////////


	cs1 .Empty(); cs2.Empty();
	nElements=0;
	int ElementIndex=-1;

	bool type_dumped=false;
	for ( list<CStdString>::iterator theIterator = g_all_messages_list.begin(); 
			theIterator != g_all_messages_list.end(); theIterator++,nElements++)
	{
		char str[15];
		if (g_VdkData->FindMarshalledMessage(*theIterator))
		{
		
			CStdString csIndex(itoa(ElementIndex,str,10));
			if (type_dumped) {
				cs1 += ",\r\n    k" + *theIterator + "=" + csIndex ;
			}
			else {
				cs1 += "    k" + *theIterator + "=" + csIndex ;
			}
			cs3.Format( "#define k%s %d    /* enum */", *theIterator, ElementIndex-- );
			cs2 += cs3;
	
			if( nElements < g_all_messages_list.size() -1 ) {
				//cs1 += "\r\n"; 
				cs2 += "\r\n";
			}
			type_dumped = true;	
		}
		else ElementIndex--;
	}

	// we want to be able to define this even when there are no marshalled messages
	// so we define cs1 as non-empty 

	if (ElementIndex == -1) // there were no marshalled types
		cs1 = "        kNoMarshalledMessages\r\n"; 
	HeaderFile.ReplaceToken( "%C_MarshalledTypeEnumList%", cs1 );

	HeaderFile.ReplaceToken( "%ASM_MarshalledTypeEnumList%", cs2 );
	HeaderFile.Close();

	return( TRUE );
}


BOOL GenerateRbldFile( LPCSTR  szRbldFile )
{
	CVdkTemplateFile RbldFile;
	CStdString csRbldFile(szRbldFile);
	csRbldFile.ReleaseBuffer();
	CStdString csFileContents("Delete this file to trigger rebuild. \r\n" 
		"Will be automatically deleted on selection of 'Rebuild All' from the \r\n"
		"'Project' Menu within VisualDSP");

	if( !RbldFile.Create( csRbldFile ,""))
	{
		CStdString csMsg;
		VDKGEN_ERROR_NO_FILE("An error has occured while trying to create the rebuild file ");
		cerr<<"               " << szRbldFile <<endl;
		cerr<<"              Please make sure that the path is valid and that the directory is writable."<<endl;
		return( FALSE );
	}

	RbldFile.EnableBuffering( TRUE );
	RbldFile.Write((LPCSTR)csFileContents,csFileContents.GetLength());
	RbldFile.Close();
	return (TRUE);

}

BOOL GenerateLibraryImportFile( LPCSTR szImportFile)
{
	CVdkTemplateFile ImportFile;
	CStdString csImportFile(szImportFile);
	csImportFile.ReleaseBuffer();
	if( !ImportFile.Create( csImportFile ,""))
	{
		CStdString csMsg;
		VDKGEN_ERROR_NO_FILE("An error has occured while trying to create the rebuild file ");
		cerr<<"               " << szImportFile <<endl;
		cerr<<"              Please make sure that the path is valid and that the directory is writable."<<endl;
		return( FALSE );
	}

	ImportFile.EnableBuffering( TRUE );
	ImportFile.Close();
	g_first_config_file->Save(szImportFile,true);
	return (TRUE);
	
}
	
