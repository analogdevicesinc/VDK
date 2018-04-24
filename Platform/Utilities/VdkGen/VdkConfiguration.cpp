/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

////////////////////////////////////////////////////////////////////////////
//
//  VdkConfiguration.cpp
//  CVdkConfiguration Object Class Implementation File
//
//  The CVdkConfiguration class implements the list of VDK elements 
//	from the .vdk file.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VdkConfiguration.h"
#include "VdkApplication.h"
#include "StdString.h"
#include <algorithm>
#include <list>
#include <errno.h>


/////////////////////////////////////////////////////////////////////////////
// Constants, Globals and Enumerations
/////////////////////////////////////////////////////////////////////////////

extern CVdkConfiguration* g_VdkData;
extern CVdkConfiguration* g_first_config_file;
extern CVdkPlatformInfo g_VdkPlatformInfo;
vector <int>					   g_NumberOfNodes;

extern bool vdkgen_for_rt;
bool error_found= false;
extern VdkApplication g_Project;

//////////////////////////////////////////////////////////////////////
// class CVdkConfiguration
//////////////////////////////////////////////////////////////////////

void CVdkConfiguration::SetModified( BOOL bModified )
{
	m_bIsModified = bModified;
}

 void CVdkConfiguration::SetHasMessageQueues( BOOL bHasMessageQueues )
 {
       m_bHasMessageQueues = bHasMessageQueues;
 }

CVdkConfiguration*  CVdkConfiguration::Get(CVdkConfigType inType, CVdkDirection inDirection )
{
	if (inType == kImport) 
	{
		if (inDirection == kNext)
			return (m_next_Import);
		else 
			return (m_prev_Import);
	}
	else
	{
		if (inDirection == kNext)			
			return(m_next_Library);
		else
			return (m_prev_Library);
	}
}

void CVdkConfiguration::Set( CVdkConfiguration* configuration, CVdkConfigType inType , CVdkDirection inDirection )
{
	if (inType == kImport) {
		if (inDirection == kNext)
			m_next_Import = configuration;
		else
			m_prev_Import = configuration;
	}
	else {
		if (inDirection == kNext)
			m_next_Library = configuration;
		else
			m_prev_Library = configuration;
	}
}


BOOL CVdkConfiguration::Load( LPCTSTR szFileName )
{
	// Read the configuration data from a .vdk file.  This data
	// is stored in a standard human-readable ini file format.

	// empty the current configuration data
	//Clear();
	UINT i =0;
	m_bErrorDumped=FALSE;
    char ext[10], fname[100],drive[10],dir[MAX_PATH],path[MAX_PATH];
	if( !PathFileExists(szFileName) )
	{
		m_csFileName = "";
		SetModified( FALSE );
		return( FALSE );
	}

	_splitpath(szFileName,drive, dir, fname, ext );

	_makepath(path,drive,dir,"","");

	m_csFileDir = path;

	unsigned int nCount = 0;
	m_csFileName = szFileName;
	SetModified();

	// read the heaps. we need to do this first so we can see if the heaps 
	// specified in the data are correct

	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Heaps", "Count", nCount ) );
	// Add system heap 
	
	if (m_ConfigType!= kLibrary) { // for libraries we cannot have heaps specified
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			if (!strcmp(g_VdkPlatformInfo.GetFamily(),"SHARC"))
			{
				CVdkHeap system_heap("system_heap","seg_heap");
				m_vHeap.AddElement(system_heap);
			}
			else
			{
				if (!strcmp(g_VdkPlatformInfo.GetFamily(),"TigerSHARC")) {
					CVdkHeap system_heap0("system_heap0","0");
					m_vHeap.AddElement(system_heap0);
					CVdkHeap system_heap1("system_heap1","1");
					m_vHeap.AddElement(system_heap1);
				}
				else {
					CVdkHeap system_heap("system_heap","0");
					m_vHeap.AddElement(system_heap);
				}

			}
		}
	

		for( i=0; i<nCount; i++ )
		{
			CVdkHeap e;
			ReadElement( e, i, "Heaps", "Heap" );
			m_vHeap.AddElement( e );
		}
	}

	if (!m_ImportedConfiguration) {
	// read the system data
		ReadElement( m_SystemData, -1, "System Data", "System" );
	}

	nCount = 0;
	// read the thread types
	VERIFY( ReadInt( "Thread Types", "Count", nCount ) );

	// Libraries might not have any thread types defined. 
	if (m_ConfigType!= kLibrary) { 
		if (nCount <1 && !g_VdkPlatformInfo.IsInitConfig())
			VDKGEN_ERROR("Number of thread types must be greater than 0");
	}

	for(i=0; i<nCount; i++ )
	{
		CVdkThreadType e;
		ReadElement( e, i, "Thread Types", "ThreadType" );
		if (!m_ImportedConfiguration) {
			CStdString mess_enabled= "";
			e.GetProperty("Message Enabled", mess_enabled);
			if (mess_enabled == "true")
			{
				SetHasMessageQueues();
				if (GetNumMessages() == 0)
					VDKGEN_ERROR((LPCSTR)("Thread Type " + e.GetName() + " enables messages but messages are not allowed in the system"));
			}
		}
		m_vThreadType.AddElement( e );
	}
	nCount = 0;
	// read the boot threads
	VERIFY( ReadInt( "Boot Threads", "Count", nCount ) );


	if (m_ConfigType == kLibrary) 
	{ 
		// libraries should not have any boot threads 
		if (nCount !=0) 
			VDKGEN_ERROR("A library must not have any boot threads");

	} else 
	{
		if (nCount <1 && !g_VdkPlatformInfo.IsInitConfig()) {
		VDKGEN_ERROR("Number of boot threads must be greater than 0");
		}
	}

	for( i=0; i<nCount; i++ )
	{
		CVdkBootThread e;
		ReadElement( e, i, "Boot Threads", "BootThread" );
		m_vBootThread.AddElement( e );
	}


	if (!m_ImportedConfiguration)	
	{
		unsigned int max_threads=0;
		CStdString str;
		g_VdkData->GetSystemData().GetProperty("Maximum Running Threads", str);
		max_threads = atoi(str);
		if (nCount> max_threads) {
			VDKGEN_ERROR("The maximum number of threads must be greater or equal");
			cerr << "       to the number of boot threads"<<endl;
		}
	}

	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Idle Thread", "Count", nCount ) );

	if ( nCount <1 && !g_VdkPlatformInfo.IsInitConfig()) {
		if (m_ConfigType == kMain)
			VDKGEN_ERROR("No idle thread specified");
	}
	else {
	// read the idle thread
		CVdkIdleThread e;
		ReadElement( e, 0, "Idle Thread", "IdleThread" );
		m_vIdleThread.AddElement( e );
	
	}

	// read the priorities
	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Priorities", "Count", nCount ) );
	for( i=0; i<nCount; i++ )
	{
		CVdkPriority e;
		ReadElement( e, i, "Priorities", "Priority" );
		m_vPriority.AddElement( e );
	}

	// read the semaphores
//	VERIFY( ReadInt( "Semaphores", "Max Number Active", nCount ) );
//	m_vSemaphore.SetMaxActive(nCount);

	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Semaphores", "Count", nCount ) );

	if (m_ConfigType == kLibrary) 
	{ 
		// libraries should not have any boot elements 
		if (nCount !=0) 
			VDKGEN_ERROR("A library must not have any boot semaphores");
	}

	for( i=0; i<nCount; i++ )
	{
		CVdkSemaphore e;
		ReadElement( e, i, "Semaphores", "Semaphore" );
		m_vSemaphore.AddElement( e );
	}

	if (!m_ImportedConfiguration && nCount> g_VdkData->GetMaxActiveSemaphores()) {
		VDKGEN_ERROR("The maximum number of active semaphores must be greater or equal");
	    cerr << "       to the number of boot semaphores"<<endl;
	}

	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Memory Pools", "Count", nCount ) );

	if (m_ConfigType == kLibrary) 
	{ 
		// libraries should not have any boot elements 
		if (nCount !=0) 
			VDKGEN_ERROR("A library must not have any boot memory pools");
	}

	for( i=0; i<nCount; i++ )
	{
		CVdkMemoryPool e;
		ReadElement( e, i, "Memory Pools", "MemoryPool" );
		m_vMemoryPool.AddElement( e );
	}

	if (!m_ImportedConfiguration && nCount> g_VdkData->GetMaxActiveMemoryPools()) {
		VDKGEN_ERROR("The maximum number of active memory pools must be greater or equal");
	    cerr << "       to the number of boot memory pools"<<endl;
	}

	// read the events
	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Events", "Count", nCount ) );
	if (nCount > 31) 
		VDKGEN_ERROR("Number of events must be lower than 31");

	for( i=0; i<nCount; i++ )
	{
		CVdkEvent e;
		ReadElement( e, i, "Events", "Event" );

		// read the event bits associated with this particular event
		CStdString csName;
		UINT nBits = 0;
		csName.Format( "Event%d.", i );
		ReadInt( "Events", csName + "NumBits", nBits );

		for( unsigned int j=0; j<nBits; j++ )
		{
			CStdString csBitName, csBitValue, csKey;
			csKey.Format( "Bit%d.", j );
			ReadString( "Events", csName + csKey + "Name", csBitName );
			ReadString( "Events", csName + csKey + "Value", csBitValue );
			e.AddBit( csBitName, csBitValue );
		}

		m_vEvent.AddElement( e );
	}

	// read the event bits
	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Event Bits", "Count", nCount ) );
	if (nCount > 31) 
		VDKGEN_ERROR("Number of event bits must be lower than 31");
	for( i=0; i<nCount; i++ )
	{
		CVdkEventBit e;
		ReadElement( e, i, "Event Bits", "EventBit" );
		m_vEventBit.AddElement( e );
	}

	// read the interrupts
	if (!m_ImportedConfiguration) {
		nCount = 0;
		VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Interrupts", "Count", nCount ) );
		for( i=0; i<nCount; i++ )
		{
			CVdkISR e;
			bool found = false;
			ReadElement( e, i, "Interrupts", "ISR" );
			for ( unsigned int j = 0 ; j<g_VdkPlatformInfo.GetNumInterrupts();j++) {
				if (g_VdkPlatformInfo.GetInterrupt(j) == e.GetName()) { 
					g_VdkPlatformInfo.RemoveInterrupt(j); // an interrupt can only be used once
					found = true;
					break;
				}
			}
			if (!found) VDKGEN_ERROR ((LPCTSTR) (e.GetName() + " is not recognised or already in use"));
			if (g_VdkPlatformInfo.FindInInterrupt2(e.GetName())) {
				e.SetInterruptListNum(2);
				m_vISR2.AddElement( e );
			}
			else  {
				e.SetInterruptListNum(1);
				m_vISR.AddElement( e );
			}

		}
		nCount = 0;
		// nobody is forced to have an entry for imported libraries
		VDKGEN_VERIFY(false,ReadInt( "VDK Imported Libraries", "Count", nCount ));
		for(  i=0; i<nCount; i++ )
		{
			ReadFile("VDK Imported Libraries","VDK Library",i);
		}
	}

	// read the device drivers
	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Device Drivers", "Count", nCount ) );
	for( i=0; i<nCount; i++ )
	{
		CVdkDeviceDriver e;
		ReadElement( e, i, "Device Drivers", "DeviceDriver" );
		m_vDeviceDriver.AddElement( e );
	}

	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "I/O Objects", "Count", nCount ) );

	if (m_ConfigType == kLibrary) 
	{ 
		// libraries should not have any boot elements 
		if (nCount !=0) 
			VDKGEN_ERROR("A library must not have any boot I/O objects");
	}

	if (!m_ImportedConfiguration && nCount> g_VdkData->GetMaxActiveDrivers()) {
		VDKGEN_ERROR("The maximum number of active I/O Objects must be greater or equal");
	    cerr << "       to the number of boot I/O Objects"<<endl;
	}

	for( i=0; i<nCount; i++ )
	{
		CVdkBootIOObject e;
		ReadElement( e, i, "I/O Objects", "IOObject" );
		m_vBootIOObject.AddElement(e);
	}

	nCount = 0;
	VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Device Flags", "Count", nCount ) );
	for( i=0; i<nCount; i++ )
	{
		CVdkDeviceFlag e;
		ReadElement( e, i, "Device Flags", "DeviceFlag" );
		m_vDeviceFlag.AddElement( e );
	}

	if (m_ConfigType == kLibrary) 
	{ 
		// libraries should not have any boot elements 
		if (nCount !=0) 
			VDKGEN_ERROR("A library must not have any boot device flags");
	}

	if (!m_ImportedConfiguration && nCount> g_VdkData->GetMaxActiveDevFlags()) {
		VDKGEN_ERROR("The maximum number of active device flags must be greater or equal");
	    cerr << "       to the number of boot device flags"<<endl;
	}

	if (!g_VdkPlatformInfo.IsInitConfig())
	{
		ReadElement( m_vMultiprocessorInfo, 0, "Multiprocessor Info", "MultiProcInfo" );
	}

	CStdString csMultiprocessorMessages;

	g_VdkData->GetMultiprocessorInfo().GetProperty("Multiprocessor Messages Allowed", csMultiprocessorMessages);

	if (csMultiprocessorMessages.CompareNoCase("false") != NULL) 
	{
		// read the marshalled messages
		bool bErrorFound = false;
		nCount = 0;
		VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Marshalled Message Types", "Count", nCount ) );
		if (nCount >0) {

			// in the imported files we don't know if messaging is allowed as we don't
			// read the system data
			if (GetNumMessages() == 0 && !m_ImportedConfiguration) {
				// if the error was already found we don't need to print this again
				if (!bErrorFound) {
					VDKGEN_ERROR("Marshalled messages are not allowed in project");			
					cerr<<(LPCSTR)("              " + m_csFileName)<<endl;
				}
				cerr<<(LPCSTR)("              Maximum number of messages is 0") <<endl ;
			}
		}
			

	for(  i=0; i<nCount; i++ )
	{
		CVdkMarshalledMessage e;
		ReadElement( e, i, "Marshalled Message Types", "MessageType" );
		m_vMarshalledMessage.AddElement( e );
	}

	// read the routing threads

		nCount = 0;
		VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt("Routing Threads", "Count", nCount ) );
		if (nCount >0 ) {
			if (!m_ImportedConfiguration) {
				// in the imported files we don't know if messaging is allowed as we don't
				// read the system data
				if (GetNumMessages() == 0 && !m_ImportedConfiguration) {
					// if the error was already found we don't need to print this again
					if (!bErrorFound) {
						VDKGEN_ERROR("Routing threads are not allowed in project");			
						cerr<<(LPCSTR)("              " + m_csFileName)<<endl;
					}
					cerr<<(LPCSTR)("              Maximum number of messages is 0")<<endl  ;
				}
			}
		}
	
		bool incoming_rthreads=false;
		for(  i=0; i<nCount; i++ )
		{
			CVdkRoutingThread e;
			CStdString cs1;
			ReadElement( e, i, "Routing Threads", "RoutingThread" );
			m_vRoutingThread.AddElement( e );
			e.GetProperty("Message Flow",cs1);
			if (cs1.CompareNoCase("incoming") == 0) incoming_rthreads=true;
		}
	
		if (incoming_rthreads == false && nCount != 0)
				VDKGEN_ERROR("At least one routing thread must be incoming");

		nCount = 0;
		VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Routing Nodes", "Count", nCount ) );
		if (nCount == 1 && g_VdkData->GetNumMarshalledMessages() > 0) {
			VDKGEN_ERROR("Marshalled message types specified but number of nodes is 1");
		}

		if (nCount >1 && g_VdkData->GetNumRoutingThreads() <1 ) {
			VDKGEN_ERROR("Routing nodes specified but not routing threads");
		}
		else if (nCount > 32) {
			VDKGEN_ERROR("Number of nodes greater than the maximum allowed");
		}
		else
		{
				bErrorFound = false;
				// If we have one node it will be the current one. The tests specify 0 just now but
				// that might be changed
				if (nCount >1) {
				// in the imported files we don't know if messaging is allowed as we don't
				// read the system data
				if (GetNumMessages() == 0 && !m_ImportedConfiguration) {
						// if the error was already found we don't need to print this again
						if (!bErrorFound) {
							VDKGEN_ERROR("Routing nodes are not allowed in project");			
							cerr<<(LPCSTR)("              " + m_csFileName)<<endl;
						}
					cerr<<(LPCSTR)("              Maximum number of messages is 0") <<endl  ;
				}
			}
			for(  i=0; i<nCount; i++ )
			{
				CVdkRoutingNode e;
				ReadElement( e, i, "Routing Nodes", "RoutingNode" );
				m_vRoutingNode.AddElement( e );
			}
		}
			// we do not support nested imports 
		if (!m_ImportedConfiguration) {
			VDKGEN_VERIFY( m_ConfigType!= kLibrary, ReadInt( "Imports", "Count", nCount ) );
			for(  i=0; i<nCount; i++ )
			{
				ReadFile("Imports","Import",i);
			}

		}
	}
	bool MP_MessagingAllowed = false;
	CStdString cs1 = "";
	GetMultiprocessorInfo().GetProperty("Multiprocessor Messages Allowed", cs1);
	if (cs1.CompareNoCase("true") == 0) {
		MP_MessagingAllowed =true;
	}
	/* if Multi-processor messaging is allowed we can have thread types in the other core
	that allow messages so we don't want the warning */

	if ( !MP_MessagingAllowed && !HasMessageQueues() && GetNumMessages() > 0)
		VDKGEN_WARNING("Maximum Messages set to " << GetNumMessages() <<" but no Thread Types enable messages in the system");
	
	SetModified( FALSE );
	return( TRUE );
}

void CVdkConfiguration::CheckConsistencyWithMaster() {

	CStdString csID, csID2,csMasterID;
	int iValue;
	unsigned int i=0;
	bool wrongID=false;

	// Remove ID from the list of all node's IDs.
	g_first_config_file->GetMultiprocessorInfo().GetProperty("Local Processor ID",csMasterID);
	GetMultiprocessorInfo().GetProperty("Local Processor ID",csID);

	iValue = atoi((LPCSTR)csID);
	if (!g_NumberOfNodes.empty()){
		for (i=0;i<g_NumberOfNodes.size();i++) {
			if (g_NumberOfNodes[i] == iValue) 
				break;
		}

		if (i == g_NumberOfNodes.size() ) {
			wrongID=true;
			VDKGEN_ERROR((LPCSTR) ("Node" + csID + " from project "));
			cerr<<(LPCSTR)("              " + m_csFileName)<<endl;
			cerr<<"              has not been recognised"<<endl;
		}
		else {
			g_NumberOfNodes.erase(g_NumberOfNodes.begin() + i);
		}
	}

	// we are not checking a file with itself
	if (this == g_first_config_file) {
		return;
	}

	// check that the number of nodes is the same as the master
	if (GetNumRoutingNodes() != g_first_config_file->GetNumRoutingNodes()) {
		VDKGEN_ERROR("Number of nodes in imported project ");
		cerr<<(LPCSTR)("              " + m_csFileName)<<endl;
		cerr<<(LPCSTR)("              does not match master project")<<endl;
	}


	// check that the Node ID is not repeated in any other configuration
	if (!wrongID) {
	// first compare with the master
		if (csID == csMasterID) {
			VDKGEN_ERROR((LPCSTR)("The Processor ID in import" )); 
			cerr<<(LPCSTR)("              " + m_csFileName + " is ")<<endl;
			cerr<<"              the same as the master" << endl;
		}

		// compare with the rest of the IDs (only with the next configuration onwards so we
		// don't get the error message twice.

		for (CVdkConfiguration* config1= GetNextImport();config1;config1=config1->GetNextImport())
		{
			config1->GetMultiprocessorInfo().GetProperty("Local Processor ID",csID2);
		
			if (csID == csID2) {
				VDKGEN_ERROR((LPCSTR)("The Processor ID in imported projects" )); 
				cerr<<(LPCSTR)("              " + m_csFileName + " and ")<<endl;
				cerr<<(LPCSTR)("              " + config1->m_csFileName)<<endl;
				cerr<<(LPCSTR)("              is the same")<<endl;
			}
		}
	}

}

BOOL CVdkConfiguration::Save( LPCTSTR szFileName /* = NULL */, bool generate_library )
{
	unsigned int i = 0;
	CStdString csFileName = szFileName;
	if( szFileName ) {
		if (!generate_library)
			m_csFileName = szFileName;
		else
			m_csImportFileName = szFileName;
	}
	SetModified();

	ASSERT( !m_csFileName.IsEmpty() );

	// first clear any existing data in the file
	FILE *pFile;
	if( (pFile = fopen(szFileName, "w+b")) == NULL)
			return( FALSE );

	fclose(pFile);

	// store the system data
	WriteElement(m_SystemData, -1, "System Data", "System", csFileName );

	// store the thread types
	unsigned int iNumThreadTypes = m_vThreadType.GetNumElements();
	if (!generate_library || iNumThreadTypes != 0)
		WriteInt( "Thread Types", "Count", iNumThreadTypes, csFileName );
	for( i=0; i<iNumThreadTypes; i++ )
		WriteElement( m_vThreadType.GetElement(i), i, "Thread Types", "ThreadType", csFileName  );

	// store the boot threads
	unsigned int iNumBootThreads = m_vBootThread.GetNumElements();
	if (!generate_library || iNumBootThreads != 0)
		WriteInt( "Boot Threads", "Count", iNumBootThreads, csFileName );
	for( i=0; i<iNumBootThreads; i++ )
		WriteElement( m_vBootThread.GetElement(i), i, "Boot Threads", "BootThread", csFileName  );

	unsigned int iNumPriorities = m_vPriority.GetNumElements();
	// store the priorities
	if (!generate_library || iNumPriorities != 0)
		WriteInt( "Priorities", "Count", iNumPriorities, csFileName  );
	for( i=0; i<iNumPriorities; i++ )
		WriteElement( m_vPriority.GetElement(i), i, "Priorities", "Priority", csFileName  );

	// store the semaphores
//	WriteInt( "Semaphores", "Max Number Active", m_vSemaphore.GetMaxNumberActive(), csFileName  );
	unsigned int iNumSemaphores = m_vSemaphore.GetNumElements();
	if (!generate_library || iNumSemaphores != 0)
		WriteInt( "Semaphores", "Count", iNumSemaphores, csFileName  );
	for( i=0; i<iNumSemaphores; i++ )
		WriteElement( m_vSemaphore.GetElement(i), i, "Semaphores", "Semaphore", csFileName  );

	// store the events
	unsigned int iNumEvents = m_vEvent.GetNumElements();
	if (!generate_library || iNumEvents != 0)
		WriteInt( "Events", "Count", iNumEvents, csFileName  );
	for( i=0; i<iNumEvents; i++ )
	{
		CVdkEvent& e = m_vEvent.GetElement( i );
		WriteElement( e, i, "Events", "Event", csFileName  );

		// store the event bits associated with this particular event
		CStdString csName;
		INT nBits = e.GetNumBits();
		csName.Format( "Event%d.", i );
		WriteInt( "Events", csName + "NumBits", nBits, csFileName  );

		for( int j=0; j<nBits; j++ )
		{
			CStdString csBitName, csBitValue, csKey;
			csKey.Format( "Bit%d.", j );
			e.GetBit( j, csBitName, csBitValue );
			WriteString( "Events", csName + csKey + "Name", csBitName, csFileName  );
			WriteString( "Events", csName + csKey + "Value", csBitValue, csFileName  );
		}
	}

	// store the event bits
	unsigned int iNumEventBits = m_vEventBit.GetNumElements();
	if (!generate_library || iNumEventBits != 0)
		WriteInt( "Event Bits", "Count", iNumEventBits, csFileName  );
	for( i=0; i<iNumEventBits; i++ )
		WriteElement( m_vEventBit.GetElement(i), i, "Event Bits", "EventBit", csFileName  );

	// store the interrupts
	unsigned int iNumInterrupts = m_vISR.GetNumElements();
	if (!generate_library || iNumInterrupts != 0)
		WriteInt( "Interrupts", "Count", iNumInterrupts, csFileName  );
	for( i=0; i<iNumInterrupts; i++ )
			WriteElement( m_vISR.GetElement(i), i, "Interrupts", "ISR", csFileName  );
	unsigned int iNumInterrupts2 = m_vISR2.GetNumElements();
	for( i=0; i<iNumInterrupts2; i++ )
			WriteElement( m_vISR2.GetElement(i), i + iNumInterrupts, "Interrupts", "ISR" , csFileName );


	// store the device drivers
	unsigned int iNumDevDrivers = m_vDeviceDriver.GetNumElements();
	if (!generate_library || iNumDevDrivers != 0)
		WriteInt( "Device Drivers", "Count", iNumDevDrivers, csFileName  );
	for( i=0; i<iNumDevDrivers; i++ )
	{
		CVdkDeviceDriver& e = m_vDeviceDriver.GetElement( i );
		WriteElement( e, i, "Device Drivers", "Device Driver", csFileName  );
	}
	// Store Boot Io objects
	unsigned int iNumIOObjects = m_vBootIOObject.GetNumElements();
	if (!generate_library || iNumIOObjects != 0)
		WriteInt( "I/O Objects", "Count", iNumIOObjects, csFileName  );
	for( i=0; i<iNumIOObjects; i++ )
	{
		CVdkBootIOObject& e = m_vBootIOObject.GetElement( i );
		WriteElement( e, i, "I/O Objects", "I/O Object", csFileName  );
	}
	// store the device flags
//	WriteInt( "Device Flags", "Max Number Active", m_vDeviceFlag.GetMaxNumberActive(), csFileName  );
	unsigned int iNumDevFlags = m_vDeviceFlag.GetNumElements();
	if (!generate_library || iNumDevFlags != 0)
		WriteInt( "Device Flags", "Count", iNumDevFlags, csFileName  );
	for( i=0; i<iNumDevFlags; i++ )
		WriteElement( m_vDeviceFlag.GetElement(i), i, "Device Flags", "Device Flag", csFileName  );

	// store the memory pools
	unsigned int iNumMempools = m_vMemoryPool.GetNumElements();
	if (!generate_library || iNumMempools != 0)
		WriteInt( "Memory Pools", "Count", iNumMempools, csFileName  );
	for( i=0; i<iNumMempools; i++ )
		WriteElement( m_vMemoryPool.GetElement(i), i, "Memory Pools", "Memory Pool", csFileName  );

	//store the heaps
	unsigned int iNumHeaps = m_vHeap.GetNumElements();
	unsigned int iFirstUserHeap =0;
	if (!strcmp(g_VdkPlatformInfo.GetFamily(),"TigerSHARC")) 
	{
		iNumHeaps-=2; // remove the two system_heaps that we add by default
		iFirstUserHeap +=2; // first user heap is the third
	}
	else 
	{
		iNumHeaps--; // remove the system_heap that we add by default
		iFirstUserHeap ++; // the first heap is the system one 

	}
	if (!generate_library || iNumHeaps != 0)
		WriteInt( "Heaps", "Count", iNumHeaps, csFileName  );
	for( i=iFirstUserHeap; i<m_vHeap.GetNumElements(); i++ )
		WriteElement( m_vHeap.GetElement(i), i, "Heaps", "Heap", csFileName  );

	SetModified( FALSE );
	return( TRUE );
}

BOOL CVdkConfiguration::WriteElement( CVdkElement& Element, INT nId,
									  LPCTSTR szSection,  LPCTSTR szType, LPCSTR szFileName )
{
	CStdString csKey, csName, csValue;

	// if nId is -1 then this is a singleton element like
	// system data so we don't need to append the id to the
	// element name

	if( nId == -1 )
		csKey.Format( "%s.", szType );
	else
		csKey.Format( "%s%d.", szType, nId );

	// write the name
	WriteString( szSection, csKey + "Name" , Element.GetName(), szFileName  );

	// write each of the elements properties
	for( unsigned int i=0; i<Element.GetNumProperties(); i++ )
	{
		Element.GetProperty( i, csName, csValue );
		if (nId == -1 && g_Project.GetOutputType() == kLibrary && csName.Left(7) != "Maximum") continue;
		if (nId != -1 || csValue != "0" || g_Project.GetOutputType() != kLibrary ) // in the system we just want
															// to print out things that are non zero
		WriteString( szSection, csKey + csName, csValue, szFileName  );
	}

	return( TRUE );
}

BOOL CVdkConfiguration::ReadElement( CVdkElement& Element,
									 INT nId,
									 LPCTSTR szSection,
									 LPCTSTR szType )
{
	CStdString csKey, csName, csValue;
	CStdString validNodeName, currProcessorNode,csLocalProcessorID;


	// if nId is -1 then this is a singleton element like
	// system data so we don't need to append the id to the
	// element name

	if( nId == -1 )
		csKey.Format( "%s.", szType );
	else
		csKey.Format( "%s%d.", szType, nId );

	// read the elements name
	if (strcmp(szType,"MultiProcInfo") == 0) {

		// for now we don't want to fail if there is no MultiprocessorInfo as the IDDE still 
		// doesn't dump it. It should be there in the future though.

	 if( !ReadString(szSection, csKey + "Local Processor ID" , csValue) )
			return( FALSE );
	}
	else {
		if( !ReadString(szSection, csKey + "Name" , csValue) ) {
			VDKGEN_ERROR((LPCTSTR) (csKey + "does not have a valid name"));
			return( FALSE );
		}

		// for a routing node its name MUST be the node number 
		if (strcmp(szType,"RoutingNode") == 0) {
			validNodeName.Format("RoutingNode%d",nId);
			if (csValue != validNodeName) {
				VDKGEN_ERROR ((LPCTSTR) (csValue + " is not a valid name for this routing node"));
				cerr<< "              It should be RoutingNode" << nId << endl;
				return (FALSE);
			}
			g_VdkData->GetMultiprocessorInfo().GetProperty("Local Processor ID",csLocalProcessorID);
			if (nId == atoi((LPCSTR)csLocalProcessorID))
				currProcessorNode = validNodeName;
		}
		CStdString csLibName;
		if ((g_VdkData->m_ConfigType == kLibrary) && (csKey != "System.") && ((csLibName = ConsistantName(csValue)) != "") )
				VDKGEN_ERROR( (LPCTSTR) (csValue + " has been used in " + g_VdkData->GetFileName() + " and " + csLibName));
		Element.SetName( csValue );
		if (!g_VdkData->AddName(csValue)) {
			VDKGEN_ERROR ((LPCTSTR) (csValue + " has already been used"));
		}
	}
	// read each property for this element
	for( unsigned int i=0; i<Element.GetNumProperties(); i++ )
	{
		Element.GetProperty( i, csName, csValue );

		if( !ReadString(szSection, csKey + csName, csValue) ) {
			if (g_VdkData->m_ConfigType == kLibrary && csKey == "System.") 
				continue; // in libraries system only has the non-zero fields

			// Stack1 Size is not defined by the IDDE yet
			if (csName == "Stack1 Size") {
				if (!ReadString(szSection, csKey +"Stack Size", csValue) ) {
					VDKGEN_ERROR ((LPCTSTR) (csKey + csName + " not found." ));
					return (FALSE);
				}
			}

			// the open device string can in theory be an empty string
				else {
				if (!(csName == "OpenDevice String" && csValue == "")) {
				// if we are a clusterbus marshalled message it will not
				// have an alloc area
				if (strcmp(szType,"MessageType") == 0  && csName == "Alloc Area") 
				{
					CStdString csAllocType;
					Element.GetProperty("Alloc Type", csAllocType);
					if (csAllocType.CompareNoCase("ClusterBus")) 
					{
						VDKGEN_ERROR ((LPCTSTR) (csKey + csName + " not found." ));
						return (FALSE);
					}
				}
				else 
				{
					VDKGEN_ERROR ((LPCTSTR) (csKey + csName + " not found." ));
					return (FALSE);
				}
				}
		
			}
		}

		// In a library the timer interrupt should not be specified. If it
		// was it would give us an error because the interrupt would have
		// been used already

		if ((g_VdkData->m_ConfigType == kLibrary) && (csKey == "System.") && (csName == "Timer Interrupt"))
			continue;

		if (strcmp(szType,"RoutingNode") == 0) {
			if (validNodeName == currProcessorNode) {
				if (csValue != "0") {
					VDKGEN_ERROR ((LPCTSTR) ("Value for the local processor node (" + validNodeName + ") should be 0"));
					return (FALSE);
				}
			}
			if (csValue == "0") {
				if (validNodeName != currProcessorNode) {
					VDKGEN_ERROR ((LPCTSTR) (csValue + " is an invalid value for " + csName));
					return (FALSE);
				}
			}
		}

		if (strcmp(szType,"IdleThread") == 0 && csValue == "system_heap" && vdkgen_for_rt && 
			g_VdkPlatformInfo.GetFamily() == "TigerSHARC") {
			csValue = "system_heap0";
		}
		
		if ((Element.SetProperty( csName, csValue ) ) != 1 ) {
					VDKGEN_ERROR ((LPCTSTR) (csValue + " is an invalid value for " + csName));
					return (FALSE);
		}
	}

	return( TRUE );
}

BOOL CVdkConfiguration::ReadFile( LPCTSTR szSection, LPCTSTR reason,int import_number )
{
	
	CStdString csKey, csName, csImportFile;
	CVdkConfigType configType;

	csKey.Format( "%s%d.", reason,import_number );
	if( !ReadString(szSection, csKey + "Name" , csImportFile) )
		return (FALSE);

	if (!stricmp ("Import",reason)) {
		configType = kImport;
		AddImport(csImportFile);
	}
	else {
		configType = kLibrary;
	}
	CVdkConfiguration* new_config = new CVdkConfiguration;

	if (new_config) {

			if( PathIsRelative(csImportFile) )
			{
				csImportFile = g_VdkData->m_csFileDir + csImportFile;
			}

		// insert the new configuration in the end of the ones that 
			// are already there

			while (g_VdkData->GetNext(configType)) 
				g_VdkData=g_VdkData->GetNext(configType);
			if (configType == kImport) {
				new_config->m_ImportedConfiguration = TRUE;
				new_config->m_ConfigType = kImport;
			}
			else {
				new_config->m_ConfigType = kLibrary;
			}
			g_VdkData ->SetNext(new_config,configType);
			new_config->SetPrev(g_VdkData,configType);
			g_VdkData = new_config;



			if (!g_VdkData->Load(csImportFile)) {
				VDKGEN_ERROR( (LPCTSTR) ("Failure loading import file " + csImportFile));
				g_VdkData = g_VdkData->GetPrev(configType);
				g_VdkData->SetNext((CVdkConfiguration*)NULL, configType);
				delete (new_config);
				g_VdkData = g_first_config_file;
				return (FALSE);
			}
				g_VdkData = g_first_config_file;
	}
	return( TRUE );
}

BOOL CVdkConfiguration::WriteString( LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue, LPCSTR szFileName )
{
	BOOL bResult;
	ASSERT( !m_csFileName.IsEmpty() );
	VERIFY( bResult = WritePrivateProfileString(szSection, szKey, szValue, szFileName) );
	return( bResult );
}

BOOL CVdkConfiguration::ReadString( LPCTSTR szSection, LPCTSTR szKey, CStdString& csValue )
{
	ASSERT( !m_csFileName.IsEmpty() );
	LPTSTR szValue = csValue.GetBuffer( MAX_PATH );
	DWORD dwResult = GetPrivateProfileString( szSection, szKey, "", szValue, MAX_PATH, m_csFileName );
	csValue.ReleaseBuffer();
	return( dwResult != 0 );
}

BOOL CVdkConfiguration::WriteInt( LPCTSTR szSection, LPCTSTR szKey, INT nValue, LPCSTR szFileName )
{
	ASSERT( !m_csFileName.IsEmpty() );
	CStdString csValue;
	csValue.Format( "%d", nValue );
	return( WriteString(szSection, szKey, csValue, szFileName) );
}

BOOL CVdkConfiguration::ReadInt( LPCTSTR szSection, LPCTSTR szKey, UINT& nValue )
{
	ASSERT( !m_csFileName.IsEmpty() );
	nValue=0; // make sure the old value is lost
	CStdString csValue= "";
	nValue=0; // make sure the old value is lost
	if( !ReadString(szSection, szKey, csValue) )
		return( FALSE );
	nValue = atoi( csValue );
	return( TRUE );
}
