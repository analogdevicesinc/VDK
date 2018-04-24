/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

////////////////////////////////////////////////////////////////////////////
//                                     
//  VdkProjectWnd.h
//  CVdkProjectWnd Object Class Definition File
//   
//  The CVdkProjectWnd class implements the VDK element window in the
//	IDDE.
// 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VDKCONFIGURATION_H_
#define __VDKCONFIGURATION_H_

#include "VdkProjectWnd.h"
#include "StdString.h"


///////////////////////////////////////////////////////////////////////////////
// class CVdkConfiguration
//
// This is the container for the entire VDK configuration.  It allows the
// addition, removal and iteration of any specific type of element as well
// as the ability to search for generic elements across all types.
//
///////////////////////////////////////////////////////////////////////////////

enum CVdkConfigType
{
	kMain,
	kImport,
	kLibrary,
	kEnd
};

enum CVdkDirection
{
	kPrev,
	kNext
};

class CVdkConfiguration;
extern CVdkConfiguration* g_first_config_file;
class CVdkConfiguration
{

public:
	BOOL								   m_ImportedConfiguration;
	CVdkConfigType						   m_ConfigType;

	CVdkConfiguration() :
		m_bIsModified(FALSE),m_ConfigType(kMain), m_bHasMessageQueues(FALSE), m_next_Import(NULL),m_next_Library(NULL), m_prev_Library(NULL),m_ImportedConfiguration(FALSE),m_bErrorDumped(FALSE)
	{}

	///////////////////////////////////////////////////////////////////////////
	// System Data Management
	///////////////////////////////////////////////////////////////////////////

	CVdkSystemData& GetSystemData()
	{ 
		return( m_SystemData );
	}

	// Messages management
	UINT GetNumMessages() const 
	{ 
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum Messages", cs1) ;
		return(  atoi(cs1) ); 
	}
	
	CVdkHeap GetMessagesHeap() const 
	{ 
		CStdString cs1("20");
		m_SystemData.GetProperty("Messages Heap", cs1) ;
		return( CVdkHeap(cs1) ); 
	}

	UINT GetMaxActiveMemoryPools() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum Active Memory Pools", cs1) ;
		return(  atoi(cs1) ); 
	}

	UINT GetMaxActiveSemaphores() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum Active Semaphores", cs1) ;
		// we used to add the four for the C Runtime Library semaphores
		// 3 for the streams and one for the C runtime but they use 
		// mutex now so this is not required
		return(atoi(cs1)); 
	}

	UINT GetMaxRunningThreads() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum Running Threads", cs1) ;
		return(atoi(cs1)); 
	}
	CVdkHeap GetSemaphoresHeap() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Semaphores Heap", cs1) ;
		return(  CVdkHeap(cs1) ); 
	}

	UINT GetMaxActiveDrivers() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum I/O Objects", cs1) ;
		return(  atoi(cs1) ); 
	}

	CVdkHeap GetDriversHeap() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("I/O Objects Heap", cs1) ;
		return(  CVdkHeap(cs1) ); 
	}

	UINT GetMaxActiveDevFlags() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Maximum Active Device Flags", cs1) ;
		return(  atoi(cs1) ); 
	}

	CVdkHeap GetDevFlagsHeap() const
	{
		CStdString cs1("20");
		m_SystemData.GetProperty("Device Flags Heap", cs1) ;
		return(  CVdkHeap(cs1) ); 
	}
	///////////////////////////////////////////////////////////////////////////
	// Thread Type Management
	///////////////////////////////////////////////////////////////////////////

	void AddThreadType( const CVdkThreadType& Thread )
	{ 
		m_vThreadType.AddElement(Thread);
		SetModified( TRUE );
	}

	void DeleteThreadType( CVdkThreadType* pThread )	
	{ 
		m_vThreadType.DeleteElement( pThread );
		SetModified( TRUE );
	}

	UINT GetNumThreadTypes() const 
	{ 
		return( m_vThreadType.GetNumElements() ); 
	}

	CVdkThreadType& GetThreadType( UINT n )	
	{ 
		return( m_vThreadType.GetElement(n) ); 
	}
	
	CVdkThreadType* FindThreadType( LPCTSTR szName )
	{ 
		return( m_vThreadType.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Boot Thread Management
	///////////////////////////////////////////////////////////////////////////

	void AddBootThread( const CVdkBootThread& Thread )
	{ 
		m_vBootThread.AddElement(Thread); 
		SetModified( TRUE );
	}

	void DeleteBootThread( CVdkBootThread* pThread )	
	{ 
		m_vBootThread.DeleteElement( pThread );
		SetModified( TRUE );
	}

	UINT GetNumBootThreads() const 
	{ 
		return( m_vBootThread.GetNumElements() ); 
	}

	CVdkBootThread& GetBootThread( UINT n )	
	{ 
		return( m_vBootThread.GetElement(n) ); 
	}
	
	CVdkBootThread* FindBootThread( LPCTSTR szName )
	{ 
		return( m_vBootThread.FindElement(szName) );
	}
	void RelocateBootThread( CVdkBootThread* pSrc, CVdkBootThread* pDest )
	{
		m_vBootThread.RelocateElement( pSrc, pDest );
	}
	///////////////////////////////////////////////////////////////////////////
	// Idle Thread Management
	///////////////////////////////////////////////////////////////////////////

	void AddIdleThread( const CVdkIdleThread& Thread )
	{ 
		m_vIdleThread.AddElement(Thread);
		SetModified( TRUE );
	}

	void DeleteIdleThread( CVdkIdleThread* pThread )	
	{ 
		m_vIdleThread.DeleteElement( pThread );
		SetModified( TRUE );
	}

	CVdkIdleThread& GetIdleThread( UINT n )	
	{ 
		return( m_vIdleThread.GetElement(n) ); 
	}
	
	CVdkIdleThread* FindIdleThread( LPCTSTR szName )
	{ 
		return( m_vIdleThread.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// Priority Management
	///////////////////////////////////////////////////////////////////////////

	void AddPriority( const CVdkPriority& Thread )
	{ 
		m_vPriority.AddElement(Thread); 
		SetModified( TRUE );
	}

	void DeletePriority( CVdkPriority* pThread )	
	{ 
		m_vPriority.DeleteElement( pThread );
		SetModified( TRUE );
	}

	UINT GetNumPriorities() const 
	{ 
		return( m_vPriority.GetNumElements() ); 
	}

	CVdkPriority& GetPriority( UINT n )	
	{ 
		return( m_vPriority.GetElement(n) ); 
	}
	
	CVdkPriority* FindPriority( LPCTSTR szName )
	{ 
		return( m_vPriority.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Routing Node
	///////////////////////////////////////////////////////////////////////////
	
	void AddRoutingNode( const CVdkRoutingNode& RoutingNode )
	{ 
		m_vRoutingNode.AddElement(RoutingNode);
		SetModified( TRUE );
	}

	void DeleteRoutingNode( CVdkRoutingNode* pRoutingNode )	
	{ 
		m_vRoutingNode.DeleteElement( pRoutingNode );
		SetModified( TRUE );
	}

	UINT GetNumRoutingNodes() const 
	{ 
		return( m_vRoutingNode.GetNumElements() ); 
	}

	CVdkRoutingNode& GetRoutingNode( UINT n )	
	{ 
		return( m_vRoutingNode.GetElement(n) ); 
	}
	
	CVdkRoutingNode* FindRoutingNode( LPCTSTR szName )
	{ 
		return( m_vRoutingNode.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Routing Thread
	///////////////////////////////////////////////////////////////////////////
	
	void AddRoutingThread( const CVdkRoutingThread& RoutingThread )
	{ 
		m_vRoutingThread.AddElement(RoutingThread);
		SetModified( TRUE );
	}

	void DeleteRoutingThread( CVdkRoutingThread* pRoutingThread )	
	{ 
		m_vRoutingThread.DeleteElement( pRoutingThread );
		SetModified( TRUE );
	}

	UINT GetNumRoutingThreads() const 
	{ 
		return( m_vRoutingThread.GetNumElements() ); 
	}

	CVdkRoutingThread& GetRoutingThread( UINT n )	
	{ 
		return( m_vRoutingThread.GetElement(n) ); 
	}
	
	CVdkRoutingThread* FindRoutingThread( LPCTSTR szName )
	{ 
		return( m_vRoutingThread.FindElement(szName) );
	}



	///////////////////////////////////////////////////////////////////////////
	// Semaphore Management
	///////////////////////////////////////////////////////////////////////////

	void AddSemaphore( const CVdkSemaphore& Semaphore )
	{ 
		m_vSemaphore.AddElement(Semaphore); 
		SetModified( TRUE );
	}

	void DeleteSemaphore( CVdkSemaphore* pSemaphore )	
	{ 
		m_vSemaphore.DeleteElement( pSemaphore );
		SetModified( TRUE );
	}

	UINT GetNumSemaphores() const 
	{ 
		return( m_vSemaphore.GetNumElements() ); 
	}

	CVdkSemaphore& GetSemaphore( UINT n )	
	{ 
		return( m_vSemaphore.GetElement(n) ); 
	}
	
	CVdkSemaphore* FindSemaphore( LPCTSTR szName )
	{ 
		return( m_vSemaphore.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Heap Management
	///////////////////////////////////////////////////////////////////////////

	void AddHeap( const CVdkHeap& Heap )
	{ 
		m_vHeap.AddElement(Heap); 
		SetModified( TRUE );
	}

	void DeleteHeap( CVdkHeap* pHeap )	
	{ 
		m_vHeap.DeleteElement( pHeap );
		SetModified( TRUE );
	}

	UINT GetNumHeaps() const 
	{ 
		return( m_vHeap.GetNumElements() ); 
	}

	CVdkHeap& GetHeap( UINT n )	
	{ 
		return( m_vHeap.GetElement(n) ); 
	}
	
	CVdkHeap* FindHeap( LPCTSTR szName )
	{ 
		return( m_vHeap.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// Memory Pool Management
	///////////////////////////////////////////////////////////////////////////

	void AddMemoryPool( const CVdkMemoryPool& MemoryPool )
	{ 
		m_vMemoryPool.AddElement(MemoryPool); 
		SetModified( TRUE );
	}

	void DeleteMemoryPool( CVdkMemoryPool* pMemoryPool )	
	{ 
		m_vMemoryPool.DeleteElement( pMemoryPool );
		SetModified( TRUE );
	}

	UINT GetNumMemoryPools() const 
	{ 
		return( m_vMemoryPool.GetNumElements() ); 
	}

	CVdkMemoryPool& GetMemoryPool( UINT n )	
	{ 
		return( m_vMemoryPool.GetElement(n) ); 
	}
	
	CVdkMemoryPool* FindMemoryPool( LPCTSTR szName )
	{ 
		return( m_vMemoryPool.FindElement(szName) );
	}

	
	///////////////////////////////////////////////////////////////////////////
	// Message Info
	///////////////////////////////////////////////////////////////////////////

	CVdkMultiprocessorInfo& GetMultiprocessorInfo()
	{
		return( m_vMultiprocessorInfo );
	}

	///////////////////////////////////////////////////////////////////////////
	// Marshalled Message
	///////////////////////////////////////////////////////////////////////////
	
	void AddMarshalledMessage( const CVdkMarshalledMessage& MarshalledMessage )
	{ 
		m_vMarshalledMessage.AddElement(MarshalledMessage);
		SetModified( TRUE );
	}

	void DeleteMarshalledMessage( CVdkMarshalledMessage* pMarshalledMessage )	
	{ 
		m_vMarshalledMessage.DeleteElement( pMarshalledMessage );
		SetModified( TRUE );
	}

	UINT GetNumMarshalledMessages() const 
	{ 
		return( m_vMarshalledMessage.GetNumElements() ); 
	}

	CVdkMarshalledMessage& GetMarshalledMessage( UINT n )	
	{ 
		return( m_vMarshalledMessage.GetElement(n) ); 
	}
	
	CVdkMarshalledMessage* FindMarshalledMessage( LPCTSTR szName )
	{ 
		return( m_vMarshalledMessage.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Message Port
	///////////////////////////////////////////////////////////////////////////
	
	void AddMessagePort( const CVdkMessagePort& MessagePort )
	{ 
		m_vMessagePort.AddElement(MessagePort);
		SetModified( TRUE );
	}

	void DeleteMessagePort( CVdkMessagePort* pMessagePort )	
	{ 
		m_vMessagePort.DeleteElement( pMessagePort );
		SetModified( TRUE );
	}

	UINT GetNumMessagePorts() const 
	{ 
		return( m_vMessagePort.GetNumElements() ); 
	}

	CVdkMessagePort& GetMessagePort( UINT n )	
	{ 
		return( m_vMessagePort.GetElement(n) ); 
	}
	
	CVdkMessagePort* FindMessagePort( LPCTSTR szName )
	{ 
		return( m_vMessagePort.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// DeviceFlag Management
	///////////////////////////////////////////////////////////////////////////

	void AddDeviceFlag( const CVdkDeviceFlag& DeviceFlag )
	{ 
		m_vDeviceFlag.AddElement(DeviceFlag); 
		SetModified( TRUE );
	}

	void DeleteDeviceFlag( CVdkDeviceFlag* pDeviceFlag )	
	{ 
		m_vDeviceFlag.DeleteElement( pDeviceFlag );
		SetModified( TRUE );
	}

	UINT GetNumDevFlags() const 
	{ 
		return( m_vDeviceFlag.GetNumElements() ); 
	}


	CVdkDeviceFlag& GetDeviceFlag( UINT n )	
	{ 
		return( m_vDeviceFlag.GetElement(n) ); 
	}
	
	CVdkDeviceFlag* FindDeviceFlag( LPCTSTR szName )
	{ 
		return( m_vDeviceFlag.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Event Management
	///////////////////////////////////////////////////////////////////////////

	void AddEvent( const CVdkEvent& Event )
	{ 
		m_vEvent.AddElement(Event); 
		SetModified( TRUE );
	}

	void DeleteEvent( CVdkEvent* pEvent )	
	{ 
		m_vEvent.DeleteElement( pEvent );
		SetModified( TRUE );
	}

	UINT GetNumEvents() const 
	{ 
		return( m_vEvent.GetNumElements() ); 
	}

	CVdkEvent& GetEvent( UINT n )	
	{ 
		return( m_vEvent.GetElement(n) ); 
	}
	
	CVdkEvent* FindEvent( LPCTSTR szName )
	{ 
		return( m_vEvent.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// EventBit Management
	///////////////////////////////////////////////////////////////////////////

	void AddEventBit( const CVdkEventBit& EventBit )
	{ 
		m_vEventBit.AddElement(EventBit); 
		SetModified( TRUE );
	}

	void DeleteEventBit( CVdkEventBit* pEventBit )	
	{ 
		m_vEventBit.DeleteElement( pEventBit );
		SetModified( TRUE );
	}

	UINT GetNumEventBits() const 
	{ 
		return( m_vEventBit.GetNumElements() ); 
	}

	CVdkEventBit& GetEventBit( UINT n )	
	{ 
		return( m_vEventBit.GetElement(n) ); 
	}
	
	CVdkEventBit* FindEventBit( LPCTSTR szName )
	{ 
		return( m_vEventBit.FindElement(szName) );
	}


	///////////////////////////////////////////////////////////////////////////
	// Import list Management
	///////////////////////////////////////////////////////////////////////////

	void AddImport( const CStdString inImport )
	{ 
		m_vImports.push_back(inImport); 
		SetModified( TRUE );
	}

	UINT GetNumImports() const 
	{ 
		return( m_vImports.size() ); 
	}

	CStdString GetImport( UINT n )	
	{ 
		return( m_vImports[n] );
	}
	///////////////////////////////////////////////////////////////////////////
	// ISR Management
	///////////////////////////////////////////////////////////////////////////

	void AddISR( const CVdkISR& ISR )
	{ 
		if (ISR.m_InterruptListNumber ==1 )
			m_vISR.AddElement(ISR); 
		else if (ISR.m_InterruptListNumber ==2)
			m_vISR2.AddElement(ISR); 
		SetModified( TRUE );
	}

	void DeleteISR( CVdkISR* pISR )	
	{ 
		if (pISR->GetInterruptListNum() ==1 )
			m_vISR.DeleteElement( pISR );
		else if (pISR->GetInterruptListNum() ==2)
			m_vISR2.DeleteElement( pISR );
		SetModified( TRUE );
	}


	UINT GetNumISRs() const 
	{ 
		return( m_vISR.GetNumElements() + m_vISR2.GetNumElements() ); 
	}

	CVdkISR& GetISR( UINT n )	
	{ 
		if (n < m_vISR.GetNumElements()) return( m_vISR.GetElement(n) );
		else return( m_vISR2.GetElement(n - (m_vISR.GetNumElements())));
	}
	
	CVdkISR* FindISR( LPCTSTR szName )
	{ 
		return( m_vISR.FindElement(szName) );
	}

	CVdkISR* FindISR2( LPCTSTR szName )
	{ 
		return( m_vISR2.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// Device Driver Management
	///////////////////////////////////////////////////////////////////////////

	void AddDeviceDriver( const CVdkDeviceDriver& DeviceDriver )
	{ 
		m_vDeviceDriver.AddElement(DeviceDriver); 
		SetModified( TRUE );
	}

	void DeleteDeviceDriver( CVdkDeviceDriver* pDeviceDriver )	
	{ 
		m_vDeviceDriver.DeleteElement( pDeviceDriver );
		SetModified( TRUE );
	}

	UINT GetNumDeviceDrivers() const 
	{ 
		return( m_vDeviceDriver.GetNumElements() ); 
	}

	CVdkDeviceDriver& GetDeviceDriver( UINT n )	
	{ 
		return( m_vDeviceDriver.GetElement(n) ); 
	}
	
	
	CVdkDeviceDriver* FindDeviceDriver( LPCTSTR szName )
	{ 
		return( m_vDeviceDriver.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// Boot IO object Management
	///////////////////////////////////////////////////////////////////////////

	void AddBootIOObject( const CVdkBootIOObject& BootIOObject )
	{ 
		m_vBootIOObject.AddElement(BootIOObject); 
		SetModified( TRUE );
	}

	void DeleteBootIOObject( CVdkBootIOObject* pBootIOObject )	
	{ 
		m_vBootIOObject.DeleteElement( pBootIOObject );
		SetModified( TRUE );
	}

	UINT GetNumBootIOObjects() const 
	{ 
		return( m_vBootIOObject.GetNumElements() ); 
	}

	CVdkBootIOObject& GetBootIOObject( UINT n )	
	{ 
		return( m_vBootIOObject.GetElement(n) ); 
	}
	
	CVdkBootIOObject* FindBootIOObject( LPCTSTR szName )
	{ 
		return( m_vBootIOObject.FindElement(szName) );
	}

	///////////////////////////////////////////////////////////////////////////
	// Generic Element Management
	///////////////////////////////////////////////////////////////////////////

	CVdkElement* FindElement( UINT nType, LPCTSTR szName )
	{ 
		switch( nType )
		{
			case VDK_TYPE_SYSTEM:		return( &m_SystemData );			
			case VDK_TYPE_THREADTYPE:	return( m_vThreadType.FindElement(szName) );
			case VDK_TYPE_BOOTTHREAD:	return( m_vBootThread.FindElement(szName) );
			case VDK_TYPE_IDLETHREAD:	return( m_vIdleThread.FindElement(szName) );
			case VDK_TYPE_PRIORITY:		return( m_vPriority.FindElement(szName) );
			case VDK_TYPE_SEMAPHORE:	return( m_vSemaphore.FindElement(szName) );
			case VDK_TYPE_HEAP:			return( m_vHeap.FindElement(szName) );
			case VDK_TYPE_EVENT:		return( m_vEvent.FindElement(szName) );
			case VDK_TYPE_EVENTBIT:		return( m_vEventBit.FindElement(szName) );
			case VDK_TYPE_ISR:			return( (m_vISR.FindElement(szName) == NULL ? m_vISR.FindElement(szName): m_vISR2.FindElement(szName)) );
			case VDK_TYPE_DEVICEDRIVER: return( m_vDeviceDriver.FindElement(szName) );
			case VDK_TYPE_MEMORYPOOL:   return( m_vMemoryPool.FindElement(szName));
			case VDK_TYPE_DEVICEFLAG:   return( m_vDeviceFlag.FindElement(szName));
			case VDK_TYPE_BOOTIOOBJECTS:return( m_vBootIOObject.FindElement(szName));
			case VDK_TYPE_MESSAGEPORT:	return( m_vMessagePort.FindElement(szName) );
			case VDK_TYPE_MARSHALLEDMESSAGE:	return( m_vMarshalledMessage.FindElement(szName) );
			case VDK_TYPE_ROUTINGNODE:	return( m_vRoutingNode.FindElement(szName) );
			case VDK_TYPE_ROUTINGTHREAD:	return( m_vRoutingThread.FindElement(szName) );
			default: ASSERT(FALSE);		break;
		}

		return( NULL );
	}

	void DeleteElement( UINT nType, LPCTSTR szName )
	{ 
		switch( nType )
		{
			case VDK_TYPE_THREADTYPE:    m_vThreadType.DeleteElement( szName );   break;
			case VDK_TYPE_BOOTTHREAD:    m_vBootThread.DeleteElement( szName );   break;
			case VDK_TYPE_IDLETHREAD:    m_vIdleThread.DeleteElement( szName );   break;
			case VDK_TYPE_PRIORITY:      m_vPriority.DeleteElement( szName );     break;
			case VDK_TYPE_SEMAPHORE:     m_vSemaphore.DeleteElement( szName );    break;
			case VDK_TYPE_HEAP:          m_vHeap.DeleteElement(szName);           break;
			case VDK_TYPE_EVENT:         m_vEvent.DeleteElement( szName );        break;
			case VDK_TYPE_EVENTBIT:      m_vEventBit.DeleteElement( szName );     break;
			case VDK_TYPE_ISR:		     m_vISR.DeleteElement( szName );		  break;
			case VDK_TYPE_DEVICEDRIVER:	 m_vDeviceDriver.DeleteElement( szName ); break;
			case VDK_TYPE_MEMORYPOOL:    m_vMemoryPool.DeleteElement(szName);     break;
			case VDK_TYPE_DEVICEFLAG:    m_vDeviceFlag.DeleteElement(szName);     break;
			case VDK_TYPE_BOOTIOOBJECTS: m_vBootIOObject.FindElement(szName);     break;
			case VDK_TYPE_MESSAGEPORT:	 m_vMessagePort.DeleteElement( szName );  break;
			case VDK_TYPE_MARSHALLEDMESSAGE: m_vMarshalledMessage.DeleteElement( szName );  break;
			case VDK_TYPE_ROUTINGNODE: m_vRoutingNode.DeleteElement( szName );  break;
			case VDK_TYPE_ROUTINGTHREAD: m_vRoutingThread.DeleteElement( szName );  break;

			default:                    ASSERT(FALSE);                           break;
		}
		
		SetModified( TRUE );
	}

	void Clear()
	{ 
		m_SystemData.Reset();
		m_vMultiprocessorInfo.Reset();
		m_vThreadType.DeleteAll();
		m_vBootThread.DeleteAll();
		m_vIdleThread.DeleteAll();
		m_vPriority.DeleteAll();
		m_vSemaphore.DeleteAll();
		m_vHeap.DeleteAll();
		m_vEvent.DeleteAll();
		m_vEventBit.DeleteAll();
		m_vISR.DeleteAll();
		m_vISR2.DeleteAll();
		m_vDeviceDriver.DeleteAll();
		m_vMessagePort.DeleteAll();
		m_vMarshalledMessage.DeleteAll();
		m_vRoutingNode.DeleteAll();
		m_vRoutingThread.DeleteAll();
		m_bIsModified = FALSE;
		m_bHasMessageQueues = FALSE;
		m_csFileName = "";
		m_csFileDir = "";
		m_next_Import=NULL;
		m_prev_Import=NULL;
		m_next_Library=NULL;
		m_prev_Library=NULL;

	}

	CVdkConfiguration& operator=( const CVdkConfiguration& rhs )
	{	
		m_SystemData    = rhs.m_SystemData;
		m_vThreadType   = rhs.m_vThreadType;
		m_vBootThread   = rhs.m_vBootThread;
		m_vIdleThread   = rhs.m_vIdleThread;
		m_vPriority     = rhs.m_vPriority;
		m_vSemaphore    = rhs.m_vSemaphore;
		m_vHeap			= rhs.m_vHeap;
		m_vEvent        = rhs.m_vEvent;
		m_vEventBit     = rhs.m_vEventBit;
		m_vISR          = rhs.m_vISR;
		m_vISR2         = rhs.m_vISR2;
		m_vDeviceDriver = rhs.m_vDeviceDriver;
		m_bIsModified   = rhs.m_bIsModified;
		m_bHasMessageQueues = rhs.m_bHasMessageQueues;
		m_csFileName    = rhs.m_csFileName;
		m_csFileDir		= rhs.m_csFileDir;
		m_vMultiprocessorInfo	= rhs.m_vMultiprocessorInfo;
		m_vMessagePort	= rhs.m_vMessagePort;
		m_vMarshalledMessage = rhs.m_vMarshalledMessage;
		m_vRoutingNode	= rhs.m_vRoutingNode;
		m_vRoutingThread		= rhs.m_vRoutingThread;
		m_ImportedConfiguration = rhs.m_ImportedConfiguration;
		m_next_Import	= rhs.m_next_Import;
		m_prev_Import	= rhs.m_prev_Import;
		m_next_Library	= rhs.m_next_Library;
		m_prev_Library	= rhs.m_prev_Library;
		return( *this );
	}

	BOOL AddName (LPCTSTR arg_string) {
		for (unsigned int i=0;i<m_names_list.size();i++) 
			if (arg_string == m_names_list[i])
				return (FALSE);

		m_names_list.push_back(arg_string);
		return (TRUE);
	}
	CStdString ConsistantName (LPCTSTR arg_string) {
		CVdkConfiguration *config = g_first_config_file;
		while (config != this) {
			for (unsigned int i=0;i<(config->m_names_list.size());i++) 
				if (arg_string == config->m_names_list[i])
					return (config->GetFileName());
			config = config->GetNextLibrary();
		}
		return ("");
	}

	BOOL IsModified() { return( m_bIsModified ); }
	BOOL HasMessageQueues() { return (m_bHasMessageQueues); }
	void SetModified( BOOL = TRUE );
	void SetHasMessageQueues( BOOL = TRUE );
	void SetNextImport(CVdkConfiguration* inconfig ){ Set(inconfig,kImport,kNext);}
	void SetNextLibrary(CVdkConfiguration* inconfig ){ Set(inconfig,kLibrary,kNext);}
	void SetPrevImport(CVdkConfiguration* inconfig ){ Set(inconfig,kImport,kPrev);}
	void SetPrevLibrary(CVdkConfiguration* inconfig ){ Set(inconfig,kLibrary,kPrev);}

	CVdkConfiguration* GetNextImport(void){ return (Get(kImport,kNext));}
	CVdkConfiguration* GetNextLibrary(void){ return (Get(kLibrary,kNext));}
	CVdkConfiguration* GetPrevImport(void) { return (Get(kImport,kPrev));}
	CVdkConfiguration* GetPrevLibrary(void){ return (Get(kLibrary,kPrev));}

	CVdkConfiguration* GetNext(CVdkConfigType inType){return Get(inType,kNext); }
	CVdkConfiguration* GetPrev(CVdkConfigType inType){return Get(inType,kPrev); }
	void SetNext(CVdkConfiguration* inconfig, CVdkConfigType inType ){ Set(inconfig,inType,kNext);}
	void SetPrev(CVdkConfiguration* inconfig, CVdkConfigType inType ){ Set(inconfig,inType,kPrev);}

	CStdString GetFileName() {return m_csFileName;}
	BOOL Load( LPCTSTR );
	BOOL Save( LPCTSTR = NULL,bool = false );
	void CheckConsistencyWithMaster(void);

//	UINT GetNumInterrupts()	    { return( m_vInterrupts.size()); }
//	void RemoveInterrupt(int i) {
//		m_vInterrupts.erase(m_vInterrupts.begin() + i);
//	}
//	CStdString GetInterrupt(int i) { return( m_vInterrupts[i] ); }


//	BOOL FindInInterrupt2(CStdString str) {
//		for (int i=0;i< m_vInterrupts2.size(); i++)
//		{
//			if (m_vInterrupts2[i] == str)
//				return (TRUE);
//		}
//		return (FALSE);
//	}
protected:

	BOOL ReadElement( CVdkElement&, INT, LPCTSTR, LPCTSTR );
	BOOL ReadFile (LPCTSTR,LPCTSTR,INT);
	BOOL WriteElement( CVdkElement&, INT, LPCTSTR, LPCTSTR, LPCSTR );
	BOOL WriteString( LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR );
	BOOL ReadString( LPCTSTR, LPCTSTR, CStdString& );
	BOOL WriteInt( LPCTSTR, LPCTSTR, INT, LPCTSTR );
	BOOL ReadInt( LPCTSTR, LPCTSTR, UINT& );
	CVdkConfiguration* Get(CVdkConfigType,CVdkDirection);
	void Set(CVdkConfiguration*, CVdkConfigType, CVdkDirection);


protected:


	BOOL m_bIsModified;
	BOOL m_bErrorDumped;
	BOOL m_bHasMessageQueues;
	CStdString m_csFileName;
	CStdString m_csFileDir;
	CStdString m_csImportFileName;

	vector<CStdString> m_vImports;
	vector<CStdString> m_vInterrupts;
	vector<CStdString> m_vInterrupts2;
	CVdkSystemData m_SystemData;
	CVdkMultiprocessorInfo					m_vMultiprocessorInfo;

	CVdkElementContainer<CVdkThreadType>   m_vThreadType;
	CVdkElementContainer<CVdkBootThread>   m_vBootThread;
	CVdkElementContainer<CVdkIdleThread>   m_vIdleThread;
	CVdkElementContainer<CVdkPriority>     m_vPriority;
	CVdkElementContainer<CVdkSemaphore>    m_vSemaphore;
	CVdkElementContainer<CVdkHeap>         m_vHeap;
	CVdkElementContainer<CVdkEvent>        m_vEvent;
	CVdkElementContainer<CVdkEventBit>     m_vEventBit;
	CVdkElementContainer<CVdkISR>		   m_vISR;
	CVdkElementContainer<CVdkISR>		   m_vISR2;
	CVdkElementContainer<CVdkDeviceDriver> m_vDeviceDriver;
	CVdkElementContainer<CVdkDeviceFlag>   m_vDeviceFlag;
	CVdkElementContainer<CVdkMemoryPool>   m_vMemoryPool;
	CVdkElementContainer<CVdkBootIOObject> m_vBootIOObject;
	CVdkElementContainer<CVdkMessagePort>  m_vMessagePort;
	CVdkElementContainer<CVdkMarshalledMessage>   m_vMarshalledMessage;
	CVdkElementContainer<CVdkRoutingNode>   m_vRoutingNode;
	CVdkElementContainer<CVdkRoutingThread>   m_vRoutingThread;
	CVdkConfiguration*					   m_next_Import;
	CVdkConfiguration*					   m_prev_Import;
	CVdkConfiguration*					   m_next_Library;
	CVdkConfiguration*					   m_prev_Library;
	vector <CStdString>					   m_names_list;

};

#endif // __VDKCONFIGURATION_H_
