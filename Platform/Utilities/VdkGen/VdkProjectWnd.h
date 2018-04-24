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

#ifndef __VDKPROJECTWND_H__
#define __VDKPROJECTWND_H__

#if _MSC_VER >= 1400
#define stricmp _stricmp
#define itoa _itoa
#endif

#include <vector>
#include "StdString.h"

#ifndef MAINWIN
using namespace std;
#endif

class CVdkConfiguration;
extern CVdkConfiguration* g_VdkData;
extern bool error_found;

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
#define VDKGEN_WARNING(str) \
	{	cerr << "VdkGen Warning: " << str << endl; \
	}
	

#define VDKGEN_VERIFY(docheck, what) {	if (docheck) VERIFY(what); else what;}
///////////////////////////////////////////////////////////////////////////////
// constants and enumerations
///////////////////////////////////////////////////////////////////////////////

typedef vector<CStdString> CStdStringVector;

enum
{
	VDK_TYPE_INVALID,
	VDK_TYPE_SYSTEM,
	VDK_TYPE_THREADTYPE,
	VDK_TYPE_BOOTTHREAD,
	VDK_TYPE_IDLETHREAD,
	VDK_TYPE_SEMAPHORE,
	VDK_TYPE_HEAP,
	VDK_TYPE_EVENT,
	VDK_TYPE_EVENTBIT,
	VDK_TYPE_PRIORITY,
	VDK_TYPE_ISR,
	VDK_TYPE_DEVICEDRIVER, // used for generic IO 
	VDK_TYPE_MEMORYPOOL,
	VDK_TYPE_DEVICEFLAG,
	VDK_TYPE_BOOTIOOBJECTS,
	VDK_TYPE_MESSAGEPORT,
	VDK_TYPE_MARSHALLEDMESSAGE,
	VDK_TYPE_ROUTINGNODE,
	VDK_TYPE_ROUTINGTHREAD
};

BOOL GenerateConfigSource( LPCTSTR );
BOOL GenerateConfigHeader( LPCTSTR );
BOOL GenerateRbldFile(LPCSTR);
BOOL GenerateLibraryImportFile(LPCSTR);

#define VDK_STRINGIZE(L)	#L
#define VDK_MAKESTRING(M,L)	M(L)
#define VDK_LINE			VDK_MAKESTRING(VDK_STRINGIZE,__LINE__)
#define VDK_NOTE(X)			message(__FILE__"("VDK_LINE") : VDK NOTE: " #X)
#define VDK_TODO(X)			message(__FILE__"("VDK_LINE") : VDK TODO: " #X)

class CVdkElement;
typedef vector<CVdkElement*> CVdkElementPtrArray;


///////////////////////////////////////////////////////////////////////////////
// class CVdkElement
//
// This is the generic base class from which all VDK elements are derived.
// It is handles all the common functionality that all elements share such
// as having a name and properties.  All elements are assumed to have a
// number of 'properties', each of which has a value.  These property/value
// pairs (CVdkStringPairs) are stored in a dynamic array (CVdkStringMap).
// Every derived element must implement ValidateProperty() to either accept
// or reject a property value that the user enters and GetPropertyValues() to
// provide a list of possible values for a property.  If only one value is 
// returned then an edit box will be used to edit the property otherwise a 
// combo-box is displayed to allow the user to choose only one value from 
// the list.
//
///////////////////////////////////////////////////////////////////////////////

class CVdkElement
{
public:
	typedef pair<CStdString,CStdString> CVdkStringPair;
	typedef vector<CVdkStringPair> CVdkStringMap;


	CVdkElement() :
		m_bExpanded( TRUE )
	{
	}
	
	CVdkElement( const CVdkElement& rhs ) 
	{
		*this = rhs;
	}

	void SetName( LPCTSTR s )
	{ 
		if (!IsCIdentifier(s)) {
			char* str = (char*) malloc(strlen(s)+ 50);
			sprintf(str,"Name %s is invalid",s);
			VDKGEN_ERROR_NO_FILE(str);
			free (str);
		}
		m_csName = s; 
	}
	
	CStdString GetName() const		
	{ 
		return( m_csName ); 
	}


	BOOL IsExpanded() const
	{
		return( m_bExpanded );
	}

	void SetExpanded( BOOL bExpanded )
	{
		m_bExpanded = bExpanded;
	}


	UINT GetNumProperties()	const	
	{ 
		return( m_PropertyMap.size() );
	}
	
	BOOL GetProperty( UINT nIndex, CStdString& csName, CStdString& csValue ) const
	{ 
		ASSERT( m_PropertyMap.size() >= nIndex );

		csName = m_PropertyMap[nIndex].first;
		csValue = m_PropertyMap[nIndex].second;

		return( TRUE );
	}

	BOOL GetProperty( LPCTSTR szName, CStdString& csValue ) const
	{
		CVdkStringMap::const_iterator p;
		
		for( p = m_PropertyMap.begin(); p != m_PropertyMap.end(); p++ )
		{
			if( p->first == szName )
			{
				csValue = p->second;
				return( TRUE );
			}
		}

		return( FALSE );
	}

	void AddProperty( LPCTSTR szName, LPCTSTR szValue )
	{
		m_PropertyMap.push_back( CVdkStringPair(szName,szValue) );
	}

	void RemoveProperty( LPCTSTR szName, LPCTSTR szValue )
	{
		CVdkStringMap::iterator p;
		
		for( p = m_PropertyMap.begin(); p != m_PropertyMap.end(); p++ )
		{
			if( p->first == szName ) { m_PropertyMap.erase( p ); return; }
		}

		ASSERT( FALSE ); // the property was not found?!?
	}

	BOOL IsValidProperty( LPCTSTR szName ) const
	{
		CVdkStringMap::const_iterator p;
		
		for( p = m_PropertyMap.begin(); p != m_PropertyMap.end(); p++ )
		{
			if( p->first == szName )
				return( TRUE );
		}

		return( FALSE );
	}

	CVdkElement& operator=( const CVdkElement& rhs )
	{
		if( &rhs != this )
		{
//			m_hItem = rhs.m_hItem;
			m_csName = rhs.m_csName;
			m_bExpanded = rhs.m_bExpanded;
			m_PropertyMap = rhs.m_PropertyMap;
		}

		return( *this );
	}

	virtual BOOL SetProperty( LPCTSTR szName, LPCTSTR szValue )
	{
		if( ValidateProperty(szName, szValue) )
		{
			CVdkStringMap::iterator p;
			
			for( p = m_PropertyMap.begin(); p != m_PropertyMap.end(); p++ )
			{
				if( p->first == szName )
				{
					p->second = szValue;
					return( TRUE );
				}
			}
		}
		return( FALSE );
	}

	virtual void GetValidNameList( CStdStringVector& csa ) const
	{
		csa.clear();
		csa.push_back( m_csName );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR ) = 0;
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& ) = 0;

protected:

	BOOL IsCIdentifier (LPCTSTR arg_string) {
		char* ptr;
		char* tmp_str= (char*)malloc (strlen(arg_string)+10);
		ptr=tmp_str;
		strcpy(ptr,arg_string);
		while (*ptr!=NULL) {
			if (!isalnum(*ptr) && *ptr!= '_') {
				free(tmp_str);
				return FALSE;
			}
			ptr++;
		}
		free(tmp_str);
		return TRUE;
	}


	
	BOOL m_bExpanded;
	CStdString m_csName;
	CVdkStringMap m_PropertyMap;
};

///////////////////////////////////////////////////////////////////////////////
// template class CVdkElementContainer
//
// This is a generic container implementation based on std::vector that is 
// used by CVdkConfiguration to store the different element types.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> class CVdkElementContainer
{

public:

	void AddElement( const T& _e )
	{ 
		m_a.push_back( _e );
	}

	void RelocateElement( T* _s, T* _d )
	{
		bool found = false;
		ASSERT( _s && _d );
		T _E( *_s );
		DeleteElement(_s);
		
		vector <T>::iterator Iter=m_a.begin();
		while (found == false && Iter != m_a.end())
		{
			if (Iter->GetName() == _d->GetName()) 
			{
				m_a.insert( Iter, _E );
				found = true;
			}
			else Iter++;
		}		
	}

	void DeleteElement( T* _e )	
	{ 
		bool found = false;
		ASSERT( _e );
		vector <T>::iterator Iter=m_a.begin();
		while (found == false && Iter != m_a.end())
		{
			if (Iter->GetName() == _e->GetName()) 
			{
				m_a.erase( Iter );
				found = true;
			}
			else Iter++;
		}
		
	}

	void DeleteElement( LPCTSTR _s )	
	{ 
		ASSERT( FindElement(_s) );
		DeleteElement( FindElement(_s) );
	}

	void DeleteAll()
	{ 
		m_a.clear();
	}

	UINT GetNumElements() const 
	{ 
		return( m_a.size() );
	}

	T& GetElement( UINT _n )	
	{ 
		ASSERT(m_a.size() >= _n); 
		return( m_a[_n] ); 
	}
	
	T* FindElement( LPCTSTR _s )
	{ 
		for( unsigned int i=0; i<m_a.size(); i++ )
		{
			if( m_a[i].GetName() == _s )
				return( &m_a[i] );
		}

		return( NULL );
	}


	CVdkElementContainer& operator=( const CVdkElementContainer& rhs )
	{
		if( &rhs != this ) 
			m_a = rhs.m_a;
		return( *this );
	}

	UINT GetMaxNumberActive() const
	{
		return m_MaxActive;
	}
	
	void SetMaxActive(UINT inMaxActive)
	{
		m_MaxActive = inMaxActive;
	}

protected:

	vector<T> m_a;
	UINT m_MaxActive;

};

///////////////////////////////////////////////////////////////////////////////
// class CVdkSystemData
//
// This represents the global 'system-wide' data for the project.  There is 
// only one of these instantiated at runtime.
//
///////////////////////////////////////////////////////////////////////////////
#include "VdkPlatformInfo.h"

extern CVdkPlatformInfo g_VdkPlatformInfo;

class CVdkSystemData : public CVdkElement
{

public:

	CVdkSystemData()
	{
		AddProperty( "Clock Frequency (MHz)",  "40"   );
		AddProperty( "Tick Period (ms)",	   "5"    );
		AddProperty( "History Buffer Size",	   "256"  );
		AddProperty( "Maximum Running Threads",	   "10"   );
		AddProperty( "Instrumentation Level", "Full Instrumentation" );
		AddProperty( "Maximum Messages",	   "0"   );
		AddProperty( "Maximum Active Memory Pools",	   "0"   );
		AddProperty( "Maximum Active Semaphores",	   "0"   );
		AddProperty( "Maximum Active Device Flags",	   "0"   );
		AddProperty( "Maximum I/O Objects",	   "0"   );
		AddProperty( "Timer Interrupt", "");
//		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			AddProperty( "Semaphores Heap", "system_heap");
			AddProperty( "Device Flags Heap", "system_heap");
			AddProperty( "Messages Heap", "system_heap");
			AddProperty( "I/O Objects Heap", "system_heap");
//		}

		SetName( "System" );
		
		// don't expand this node by default
		m_bExpanded = FALSE;
	}

	void Reset()
	{
		SetProperty( "Clock Frequency (MHz)",  "40"   );
		SetProperty( "Tick Period (ms)",	   "5"    );
		SetProperty( "History Buffer Size",	   "256"  );
		SetProperty( "Maximum Running Threads",	   "10"   );
		SetProperty( "Instrumentation Level", "Full Instrumentation" );
		SetProperty( "Maximum Messages",	   "0"   );
		SetProperty( "Maximum Active Memory Pools",	   "0"   );
		SetProperty( "Maximum Active Semaphores",	   "0"   );
		SetProperty( "Maximum Active Device Flags",	   "0"   );
		SetProperty( "Maximum I/O Objects",	   "0"   );
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			if ( g_VdkPlatformInfo.GetNumContextAreas() >1 || 
				g_VdkPlatformInfo.GetNumThreadStacks() > 1) 
			{
				SetProperty( "Semaphores Heap", "system_heap0");
				SetProperty( "Device Flags Heap", "system_heap0");
				SetProperty( "Messages Heap", "system_heap0");
				SetProperty( "I/O Objects Heap", "system_heap0");
			}
			else
			{
				SetProperty( "Semaphores Heap", "system_heap");
				SetProperty( "Device Flags Heap", "system_heap");
				SetProperty( "Messages Heap", "system_heap");
				SetProperty( "I/O Objects Heap", "system_heap");
			}
		}

		m_bExpanded = FALSE;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};


///////////////////////////////////////////////////////////////////////////////
// class CVdkThreadType
///////////////////////////////////////////////////////////////////////////////

class CVdkThreadType : public CVdkElement
{

public:

	CVdkThreadType()
	{
		AddProperty( "Priority",   "5"    );
		if ( g_VdkPlatformInfo.GetNumThreadStacks() > 1)
			AddProperty( "Stack1 Size", "255" );
		AddProperty( "Source File", "" );
		AddProperty( "Header File", "" );
		AddProperty( "Message Enabled","false");
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
			if ( g_VdkPlatformInfo.GetNumContextAreas() >1 || 
				g_VdkPlatformInfo.GetNumThreadStacks() > 1)

				AddProperty( "Thread Structure Heap" , "system_heap0");
			else
				AddProperty( "Thread Structure Heap" , "system_heap");
		}
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkBootThread
///////////////////////////////////////////////////////////////////////////////

class CVdkBootThread : public CVdkElement
{

public:

	CVdkBootThread();
	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkIdleThread
///////////////////////////////////////////////////////////////////////////////

class CVdkIdleThread : public CVdkElement
{

public:

	CVdkIdleThread() {
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
				if ( g_VdkPlatformInfo.GetNumContextAreas() >1 || 
				g_VdkPlatformInfo.GetNumThreadStacks() > 1)
					AddProperty( "Heap",   "system_heap0"    );
				else
					AddProperty( "Heap",   "system_heap"    );
		}
		if (g_VdkPlatformInfo.GetNumThreadStacks() ==  1) {
			AddProperty( "Stack Size", "255" );
		}
		else {
			AddProperty( "Stack0 Size", "255" );
			AddProperty( "Stack1 Size", "255" );
		}
	}
	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkPriority
///////////////////////////////////////////////////////////////////////////////

class CVdkPriority : public CVdkElement
{

public:

	CVdkPriority()
	{
		AddProperty( "Period", "10" );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
	virtual void GetValidNameList( CStdStringVector& ) const;
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkRoutingNode
///////////////////////////////////////////////////////////////////////////////

class CVdkRoutingNode : public CVdkElement
{

public:

	CVdkRoutingNode()
	{
		AddProperty( "Routing Thread", 		"" );
		SetName( "RoutingNode" );

		// don't expand this node by default
		m_bExpanded = FALSE;
	}


	void Reset()
	{
		SetProperty( "Routing Thread", 		"" );
		m_bExpanded = FALSE;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkRoutingThread
///////////////////////////////////////////////////////////////////////////////

class CVdkRoutingThread : public CVdkElement
{

public:

	CVdkRoutingThread();

	void Reset()
	{
		SetProperty( "I/O Object", 		"" );
		SetProperty( "OpenDevice String", "");
		SetProperty( "Message Flow", "outgoing");
		m_bExpanded = FALSE;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkSemaphore
///////////////////////////////////////////////////////////////////////////////

class CVdkSemaphore : public CVdkElement
{

public:

	CVdkSemaphore()
	{
		AddProperty( "Initial Value", "0" );
		AddProperty( "Initial Delay", "0"     );
		AddProperty( "Period",		  "0"     );
		AddProperty( "Max Count", "UINT_MAX" ) ;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkHeap
///////////////////////////////////////////////////////////////////////////////

class CVdkHeap : public CVdkElement
{

public:

	CVdkHeap()
	{
		AddProperty( "ID", "0" );
	}

	CVdkHeap(CStdString name)
	{
		SetName(name);
	}

	CVdkHeap(CStdString name, CStdString value)
	{
		SetName(name);
		AddProperty( "ID", value );

	}


	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkMemoryPool
///////////////////////////////////////////////////////////////////////////////

class CVdkMemoryPool : public CVdkElement
{

public:

	CVdkMemoryPool()
	{
		AddProperty( "Block Size",       "16"  );
		AddProperty( "Number of Blocks", "4"   );
		AddProperty( "Initialize on Pool Creation",	     "false");
		if (g_VdkPlatformInfo.GetHeapSupport().CompareNoCase("true") == 0) {
			AddProperty( "Heap",	     "0"   );
		}
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkBootIOObject
///////////////////////////////////////////////////////////////////////////////

class CVdkBootIOObject : public CVdkElement
{

public:

	CVdkBootIOObject()
	{
		AddProperty( "Template",   ""    );
		AddProperty( "Initializer", ""   );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkMultiprocessorInfo
///////////////////////////////////////////////////////////////////////////////

class CVdkMultiprocessorInfo: public CVdkElement
{

public:

	CVdkMultiprocessorInfo()
	{
		AddProperty( "Local Processor ID", 		"0" );
		AddProperty( "Multiprocessor Messages Allowed", "false");
		SetName( "MultiprocessorInfo" );

		// don't expand this node by default
		m_bExpanded = FALSE;
	}


	void Reset()
	{
		SetProperty( "Local Processor ID", 		"0" );
		SetProperty( "Multiprocessor Messages Allowed", "false");
		m_bExpanded = FALSE;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

class CVdkMarshalledMessage : public CVdkElement
{

public:

	CVdkMarshalledMessage()
	{
		AddProperty( "Marshalling Function", 		"" );
		AddProperty( "Alloc Type",			"");
		AddProperty( "Alloc Area", "0");
	//	AddProperty( "Source File", "" );
		SetName( "MarshalledMessage" );

		// don't expand this node by default
		m_bExpanded = FALSE;
	}


	void Reset()
	{
		SetProperty( "Marshalling Function", 		"" );
		SetProperty( "Alloc Area", "0");
		SetProperty( "Alloc Type", "");
		m_bExpanded = FALSE;
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkMessagePort
///////////////////////////////////////////////////////////////////////////////

class CVdkMessagePort : public CVdkElement
{

public:

	CVdkMessagePort()
	{
		AddProperty( "Flags", 				"0" );
		AddProperty( "Remote Processors", 	"" );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
//	virtual void GetValidNameList( CStdStringVector& ) const;
};
///////////////////////////////////////////////////////////////////////////////
// class CVdkEvent
///////////////////////////////////////////////////////////////////////////////

class CVdkEvent : public CVdkElement
{

public:

	CVdkEvent() :
		m_bBitNodeExpanded( FALSE )
	{
		AddProperty( "Type", "Any" );
	}

	virtual BOOL SetProperty( LPCTSTR, LPCTSTR );
	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );

	// because an event is dependent on any number of predefined
	// event bits (CVdkEventBit), each CVdkEvent element needs to 
	// maintain its own list.

	BOOL IsBitNodeExpanded()         
	{ 
		return( m_bBitNodeExpanded ); 
	}

	void SetBitNodeExpanded( BOOL b ) 
	{ 
		m_bBitNodeExpanded = b; 
	}

	void AddBit( LPCTSTR szn, LPCTSTR szv )
	{
		m_vBits.push_back( CVdkStringPair(szn,szv) );
	}

	UINT GetNumBits() const          
	{
		return( m_vBits.size() ); 
	}
	
	void GetBit( UINT n, CStdString& csName, CStdString& csValue ) 
	{
		ASSERT( m_vBits.size() >= n );
		csName = m_vBits[n].first;
		csValue = m_vBits[n].second;
	}

	void SetBit( LPCTSTR szName, LPCTSTR szValue )
	{
		int bitNum = FindBit( szName );
		if( bitNum != -1 ) m_vBits[bitNum].second = szValue;
	}

	void RemoveBit( LPCTSTR szName )
	{
		int bitNum = FindBit( szName );
		if(  bitNum != -1  ) m_vBits.erase( m_vBits.begin() + bitNum );
	}

	void RenameBit( LPCTSTR szOldName, LPCTSTR szNewName )
	{
		int bitNum = FindBit( szOldName );
		if( bitNum != -1 ) m_vBits[bitNum].first = szNewName;
	}

	int FindBit( LPCTSTR szName )
	{
	//	CVdkStringMap::iterator p;
		
		for (unsigned int i=0; i< m_vBits.size();i++)
			if (m_vBits[i].first == szName) return (i);
		//for( p = m_vBits.begin(); p != m_vBits.end(); p++ )
		//{
		//	if( p->first == szName ) return( p );
		//}

		return( -1 );
	}

	CVdkEvent& operator=( const CVdkEvent& rhs )
	{
		if( &rhs != this )
		{
			CVdkElement::operator =( rhs );
//			m_hBitNode = rhs.m_hBitNode;
			m_vBits = rhs.m_vBits;
			m_bBitNodeExpanded = rhs.m_bBitNodeExpanded;

		}

		return( *this );
	}

protected:

	CVdkStringMap m_vBits;
	BOOL m_bBitNodeExpanded;
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkEventBit
///////////////////////////////////////////////////////////////////////////////

class CVdkEventBit : public CVdkElement
{

public:

	CVdkEventBit()
	{
		AddProperty( "Initial Value", "0" );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
};

///////////////////////////////////////////////////////////////////////////////
// class CVdkISR
///////////////////////////////////////////////////////////////////////////////

class CVdkISR : public CVdkElement
{


public:

	CVdkISR()
	{
		AddProperty( "Enabled at Boot", "false" );
		AddProperty( "Entry Point", "" );
		AddProperty( "Source File", "" );
		SetInterruptListNum(0);
	}

	UINT m_InterruptListNumber;
	void SetInterruptListNum(UINT num) {m_InterruptListNumber = num; }
	UINT GetInterruptListNum() { return (m_InterruptListNumber); }
	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );
	virtual void GetValidNameList( CStdStringVector& ) const;
};

///////////////////////////////////////////////////////////////////////////////////
// class CVdkDeviceDriver 
//
// Note: This is not notonly represets the device driver but a generic io object template
// FIXME: chagne the name 
//////////////////////////////////////////////////////////////////////////////////

class CVdkDeviceDriver : public CVdkElement
{

public:

	CVdkDeviceDriver()
	{
		AddProperty( "Source File", "" );
		AddProperty( "Header File", "" );
	}

	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR );
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& );

};

///////////////////////////////////////////////////////////////////////////////
// class CVdkDeviceFlag
///////////////////////////////////////////////////////////////////////////////

class CVdkDeviceFlag : public CVdkElement
{

public:

	CVdkDeviceFlag(){}
	virtual BOOL ValidateProperty( LPCTSTR, LPCTSTR ){return true;}
	virtual BOOL GetPropertyValues( LPCTSTR, CStdStringVector& ){return true;}
};


///////////////////////////////////////////////////////////////////////////////
// class CVdkTemplateFile
///////////////////////////////////////////////////////////////////////////////

class CVdkTemplateFile 
{

public:

	CVdkTemplateFile() :
		m_bBufferData( FALSE ), m_pFile(NULL)
	{}

	virtual ~CVdkTemplateFile() {}
	virtual void Close();
	virtual void Flush();

	void EnableBuffering( BOOL b ) { m_bBufferData = b; }
	BOOL IsBufferingEnabled()      { return( m_bBufferData ); }
	BOOL Create( LPCTSTR, LPCTSTR );
	UINT ReplaceToken( LPCTSTR, LPCTSTR );
	size_t Write( const void *buffer, size_t count ) { return fwrite(buffer, 1, count, m_pFile);}


protected:

	BOOL m_bBufferData;
	CStdString m_csData;
	FILE* m_pFile;
};

#endif // __VDKPROJECTWND_H__
