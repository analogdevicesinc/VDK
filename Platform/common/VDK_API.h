/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The public API
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The public interface to the RTOS, including the public functions,
 *  user accessible global variables, typedefs and enumerations.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/


#ifndef VDK_API_H_
#define VDK_API_H_
//#pragma diag(push)
//#pragma diag(errors)

#define VDK_API_VERSION_NUMBER 0x05000000

#include <limits.h>

enum DeviceDescriptor   {lastDeviceDescriptor = -1};

// BEGIN_INTERNALS_STRIP
// These definitions are in VDK_ISR_API.h and we don't want them twice in the 
// VDK_Public.h
#define VDK_INTERRUPT_LEVEL_ 0x80000000
#define VDK_KERNEL_LEVEL_ 0xC0000000

#ifndef VDK_H_

	// This code is not needed in v 3.0 because the types have been changed
	// from enum's to int's in ALL CASES as opposed to just in C++.
	// The #else case originally used for C/asm is now always used.
	#ifdef __cplusplus
	enum ThreadType 	{};
	enum ThreadID 	{};
	enum SemaphoreID	{};
	enum EventID		{};
	enum EventBitID	{};
	enum DeviceFlagID {};
	enum PoolID   {};
	enum IOTemplateID   {};
	enum IOID           {};
	enum HeapID           {};
	enum MessageID      {};
	enum MutexID      {};
    enum FifoID     {};
	enum RoutingNodeID      {};
	enum MarshalledTypeID      {};
	#else
	typedef int ThreadType;
	typedef int ThreadID;
	typedef int SemaphoreID;
	typedef int EventID;
	typedef int EventBitID;
	typedef int DeviceFlagID;
	typedef int PoolID;
	typedef int IOTemplateID;
	typedef int IOID;
	typedef int HeapID;
	typedef int MessageID;
	typedef int MutexID;
    typedef int FifoID;
	typedef int RoutingNodeID;
	typedef int MarshalledTypeID;
	#endif  /* __cplusplus */


#endif /* VDK_H__ */

// END_INTERNALS_STRIP

#ifdef _MISRA_RULES
#include <misra_types.h>
#pragma diag(push)
#pragma diag(suppress: misra_rule_5_1:"VDK Public APIs have mangled names which may be longer than 31 characters.")
#pragma diag(suppress: misra_rule_5_3:"Type declarations have to be compatible with C++")
#pragma diag(suppress:misra_rule_5_4:"Typedefs need to be compatible with C++ so the identifiers need to appear twice" )
#pragma diag(suppress:misra_rule_5_6:"Typedefs need to be compatible with C++ so the identifiers need to appear twice" )
#pragma diag(suppress:misra_rule_5_7:"HistoryEvent and PayloadDetails have an element called type")
#pragma diag(suppress:misra_rule_6_3:"API types are published and should not change to specify size and sign. This warning only happens misra-strict")
#pragma diag(suppress: misra_rule_8_5:"VDK needs to define some constant variables")
#pragma diag(suppress: misra_rule_18_1:"Some type definitions are not available to users but VDK needs to declare structures to create pointers")
#pragma diag(suppress: misra_rule_18_4:"DispatchUnion must be a union and they are not allowed in MISRA-C")
#pragma diag(suppress: misra_rule_19_4:"NAMESPACE_DECL_ cannot have the format required for macros in MISRA")
#pragma diag(suppress: misra_rule_19_6:"Header needs to undef NAMESPACE_DECL_ macros")
#pragma diag(suppress: misra_rule_19_7:"VDK uses NAMESPACE_DECL_ macro to have the correct API and type names in C and C++")
#pragma diag(suppress: misra_rule_19_13:"NAMESPACE_DECL_ requires the use of the ## operator")
#endif

/* Get the namespacing macros */
#ifdef __cplusplus

#define NAMESPACE_(X_)	    VDK::X_
#define NAMESPACE_DECL_(X_)	X_

#else

#define NAMESPACE_(X_)	    VDK_ ## X_
#define NAMESPACE_DECL_(X_)	VDK_ ## X_

#endif /* __cplusplus */



#ifdef __cplusplus
namespace VDK
{
#endif /* __cplusplus */

// Get the processor specific types
#include <ProcessorSpecificTypes.h>


 	typedef enum ThreadType NAMESPACE_DECL_(ThreadType);
 	typedef enum ThreadID NAMESPACE_DECL_(ThreadID);
	typedef unsigned int NAMESPACE_DECL_(Ticks);
	typedef enum SemaphoreID NAMESPACE_DECL_(SemaphoreID);
	typedef enum PoolID NAMESPACE_DECL_(PoolID);
	typedef enum DeviceFlagID NAMESPACE_DECL_(DeviceFlagID);
	typedef enum IOTemplateID NAMESPACE_DECL_(IOTemplateID);
	typedef enum IOID NAMESPACE_DECL_(IOID);
    typedef enum DeviceDescriptor NAMESPACE_DECL_(DeviceDescriptor);
	typedef enum EventID NAMESPACE_DECL_(EventID);
	typedef enum EventBitID NAMESPACE_DECL_(EventBitID);
	typedef enum HeapID NAMESPACE_DECL_(HeapID);
	typedef enum MessageID NAMESPACE_DECL_(MessageID);
	typedef enum MutexID NAMESPACE_DECL_(MutexID);
    typedef enum FifoID NAMESPACE_DECL_(FifoID);
	typedef enum RoutingNodeID NAMESPACE_DECL_(RoutingNodeID);
	typedef enum MarshalledTypeID NAMESPACE_DECL_(MarshalledTypeID);

	#define INVALID_ID  -1
	typedef bool (*NAMESPACE_DECL_(BootFuncPointers) )( void); 
	// Boot init function prototypes
	bool InitBootThreads(void);
	bool InitBootSemaphores(void);
	bool InitBootMemoryPools(void);
	bool InitBootIOObjects(void);
	bool InitBootDeviceFlags(void);
	bool InitRoutingThreads(void);
	bool InitISRMask(void);
	bool SetTimer(void);
	bool InitRoundRobin(void);

#if defined (__ADSPTS__) && !defined (__ADSPTS1XX__)
	bool SetTimer0(void);
#endif
	
	typedef enum
  	{
	    NAMESPACE_DECL_(kUnknown) = -1,    // required by TMK
		NAMESPACE_DECL_(kReady) = 0,      // required by TMK
		NAMESPACE_DECL_(kSemaphoreBlocked) = 1,
		NAMESPACE_DECL_(kEventBlocked) = 2,
        NAMESPACE_DECL_(kDeviceFlagBlocked) = 3,
		NAMESPACE_DECL_(kSemaphoreBlockedWithTimeout) = 4,
		NAMESPACE_DECL_(kEventBlockedWithTimeout) = 5,
        NAMESPACE_DECL_(kDeviceFlagBlockedWithTimeout) = 6,
		NAMESPACE_DECL_(kSleeping) = 7,
		NAMESPACE_DECL_(kMessageBlocked) = 8,
		NAMESPACE_DECL_(kMessageBlockedWithTimeout) = 9,
		NAMESPACE_DECL_(kMutexBlocked) = 12,
        NAMESPACE_DECL_(kFifoPendBlocked) = 13,
        NAMESPACE_DECL_(kFifoPendBlockedWithTimeout) = 14,
        NAMESPACE_DECL_(kFifoPostBlocked) = 15,
        NAMESPACE_DECL_(kFifoPostBlockedWithTimeout) = 16

	} NAMESPACE_DECL_(ThreadStatus);

	typedef enum
    	{
		NAMESPACE_DECL_(kUnknownThreadType) =          INT_MIN,     //0x80000000
		NAMESPACE_DECL_(kUnknownThread) =              -2147483647, //0x80000001
		NAMESPACE_DECL_(kInvalidThread) =              -2147483646, //0x80000002
		NAMESPACE_DECL_(kThreadCreationFailure) =      -2147483645, //0x80000003
		NAMESPACE_DECL_(kUnknownSemaphore) =           -2147483644, //0x80000004
		NAMESPACE_DECL_(kUnknownEventBit) =            -2147483643, //0x80000005
		NAMESPACE_DECL_(kUnknownEvent) =               -2147483642, //0x80000006
		NAMESPACE_DECL_(kInvalidPriority) =            -2147483641, //0x80000007
		NAMESPACE_DECL_(kInvalidDelay) =               -2147483640, //0x80000008
		NAMESPACE_DECL_(kSemaphoreTimeout) =           -2147483639, //0x80000009
		NAMESPACE_DECL_(kEventTimeout) =               -2147483638, //0x8000000a
		NAMESPACE_DECL_(kBlockInInvalidRegion) =       -2147483637, //0x8000000b
		NAMESPACE_DECL_(kDbgPossibleBlockInRegion) =   -2147483636, //0x8000000c
		NAMESPACE_DECL_(kInvalidPeriod) =              -2147483635, //0x8000000d
		NAMESPACE_DECL_(kAlreadyPeriodic) =            -2147483634, //0x8000000e
		NAMESPACE_DECL_(kNonperiodicSemaphore) =       -2147483633, //0x8000000f
		NAMESPACE_DECL_(kDbgPopUnderflow) =            -2147483632, //0x80000010
		NAMESPACE_DECL_(kBadIOID) =                    -2147483631, //0x80000011
		NAMESPACE_DECL_(kBadDeviceDescriptor) =        -2147483630, //0x80000012
		NAMESPACE_DECL_(kOpenFailure) =                -2147483629, //0x80000013
		NAMESPACE_DECL_(kCloseFailure) =               -2147483628, //0x80000014
		NAMESPACE_DECL_(kReadFailure) =                -2147483627, //0x80000015
		NAMESPACE_DECL_(kWriteFailure)=                -2147483626, //0x80000016
		NAMESPACE_DECL_(kIOCtlFailure) =               -2147483625, //0x80000017
		NAMESPACE_DECL_(kBadIOTemplateID)=             -2147483624, //0x80000018
		NAMESPACE_DECL_(kInvalidDeviceFlag) =          -2147483623, //0x80000019
		NAMESPACE_DECL_(kDeviceTimeout) =              -2147483622, //0x8000001a
		NAMESPACE_DECL_(kDeviceFlagCreationFailure) =  -2147483621, //0x8000001b
		NAMESPACE_DECL_(kMaxCountExceeded) =           -2147483620, //0x8000001c
		NAMESPACE_DECL_(kSemaphoreCreationFailure) =   -2147483619, //0x8000001d
		NAMESPACE_DECL_(kSemaphoreDestructionFailure) =-2147483618, //0x8000001e
		NAMESPACE_DECL_(kPoolCreationFailure) =        -2147483617, //0x8000001f
		NAMESPACE_DECL_(kInvalidBlockPointer) =        -2147483616, //0x80000020
		NAMESPACE_DECL_(kInvalidPoolParms) =           -2147483615, //0x80000021
		NAMESPACE_DECL_(kInvalidPoolID) =              -2147483614, //0x80000022
		NAMESPACE_DECL_(kErrorPoolNotEmpty) =          -2147483613, //0x80000023
		NAMESPACE_DECL_(kErrorMallocBlock) =           -2147483612, //0x80000024
		NAMESPACE_DECL_(kMessageCreationFailure) =     -2147483611, //0x80000025
		NAMESPACE_DECL_(kInvalidMessageID) =           -2147483610, //0x80000026
		NAMESPACE_DECL_(kInvalidMessageOwner) =        -2147483609, //0x80000027
		NAMESPACE_DECL_(kInvalidMessageChannel) =      -2147483608, //0x80000028
		NAMESPACE_DECL_(kInvalidMessageRecipient) =    -2147483607, //0x80000029
		NAMESPACE_DECL_(kMessageTimeout) =             -2147483606, //0x8000002a
		NAMESPACE_DECL_(kMessageInQueue) =             -2147483605, //0x8000002b
		NAMESPACE_DECL_(kInvalidTimeout) =             -2147483604, //0x8000002c
		NAMESPACE_DECL_(kInvalidTargetDSP) =           -2147483603, //0x8000002d
		NAMESPACE_DECL_(kIOCreateFailure) =            -2147483602, //0x8000002e
		NAMESPACE_DECL_(kHeapInitialisationFailure) =  -2147483601, //0x8000002f
		NAMESPACE_DECL_(kInvalidHeapID) =              -2147483600, //0x80000030
		NAMESPACE_DECL_(kNewFailure) =                 -2147483599, //0x80000031
		NAMESPACE_DECL_(kInvalidMarshalledType) =      -2147483598, //0x80000032
		NAMESPACE_DECL_(kUncaughtException) =          -2147483597, //0x80000033
		NAMESPACE_DECL_(kAbort) =                      -2147483596, //0x80000034
		NAMESPACE_DECL_(kInvalidMaskBit) =             -2147483595, //0x80000035
		NAMESPACE_DECL_(kInvalidThreadStatus) =        -2147483594, //0x80000036
		NAMESPACE_DECL_(kThreadStackOverflow) =        -2147483593, //0x80000037
		NAMESPACE_DECL_(kMaxIDExceeded) =              -2147483592, //0x80000038
		NAMESPACE_DECL_(kThreadDestroyedInInvalidRegion)=-2147483591, //0x80000039
		NAMESPACE_DECL_(kNotMutexOwner) =              -2147483590, //0x8000003a
		NAMESPACE_DECL_(kMutexNotOwned) =              -2147483589, //0x8000003b
		NAMESPACE_DECL_(kMutexCreationFailure) =       -2147483588, //0x8000003c
		NAMESPACE_DECL_(kMutexDestructionFailure) =    -2147483587, //0x8000003d
		NAMESPACE_DECL_(kMutexSpaceTooSmall) =         -2147483586, //0x8000003e
		NAMESPACE_DECL_(kInvalidMutexID) =             -2147483585, //0x8000003f
		NAMESPACE_DECL_(kInvalidMutexOwner) =          -2147483584, //0x80000040
		NAMESPACE_DECL_(kAPIUsedfromISR) =             -2147483583, //0x80000041
		NAMESPACE_DECL_(kMaxHistoryEventExceeded) =    -2147483582, //0x80000042
		NAMESPACE_DECL_(kInvalidPointer) =             -2147483581, //0x80000043
        NAMESPACE_DECL_(kFifoCreationFailure) =        -2147483580, //0x80000044
        NAMESPACE_DECL_(kFifoDestructionFailure) =     -2147483579, //0x80000045
        NAMESPACE_DECL_(kInvalidFifoID) =              -2147483578, //0x80000046
        NAMESPACE_DECL_(kPendFifoTimeout) =            -2147483577, //0x80000047
        NAMESPACE_DECL_(kPostFifoTimeout) =            -2147483576, //0x80000048
        NAMESPACE_DECL_(kIncompatibleArguments) =      -2147483575, //0x80000049
		NAMESPACE_DECL_(kSSLInitFailure) =             -2147483566, //0x80000052
        NAMESPACE_DECL_(kUnhandledExceptionError) =    -2147483565, //0x80000053
		NAMESPACE_DECL_(kIntsAreDisabled) =            -100,        
		NAMESPACE_DECL_(kRescheduleIntIsMasked) =      -99,        
		NAMESPACE_DECL_(kNoError) =                    0,        
		NAMESPACE_DECL_(kFirstUserError) =             1,
        NAMESPACE_DECL_(kLastUserError) =              INT_MAX
    } NAMESPACE_DECL_(SystemError);

	typedef enum
    {
	  NAMESPACE_DECL_(kNoPanic)=0,
	  NAMESPACE_DECL_(kThreadError)=1,
	  NAMESPACE_DECL_(kBootError)=2,
	  NAMESPACE_DECL_(kISRError)=3,
	  NAMESPACE_DECL_(kDeprecatedAPI)=4,
	  NAMESPACE_DECL_(kInternalError)=5,
	  NAMESPACE_DECL_(kStackCheckFailure)=6,
      NAMESPACE_DECL_(kUnhandledException)=7
    } NAMESPACE_DECL_(PanicCode);
 
#if defined (VDK_INSTRUMENTATION_LEVEL_) && (VDK_INSTRUMENTATION_LEVEL_>0)
	typedef enum
    {
		NAMESPACE_DECL_(kFromPopCriticalRegion),
		NAMESPACE_DECL_(kFromPopNestedCriticalRegions),
		NAMESPACE_DECL_(kFromPopUnscheduledRegion),
		NAMESPACE_DECL_(kFromPopNestedUnscheduledRegions)
    } NAMESPACE_DECL_(PopUnderflowError);
#endif

    typedef enum
    {
        /* enumeration value */     /* what's stored in m_Value of HistoryEvent */
		NAMESPACE_DECL_(kThreadCreated) =       INT_MIN, //0x80000000 ThreadID of created thread
		NAMESPACE_DECL_(kThreadDestroyed)=      -2147483647, //0x80000001 ThreadID of destroyed thread
		NAMESPACE_DECL_(kSemaphorePosted)=      -2147483646, //0x80000002 SemaphoreID of posted semaphore (high bit is SemaphoreID's old value)
		NAMESPACE_DECL_(kSemaphorePended)=      -2147483645, //0x80000003 SemaphoreID of pended semaphore (high bit is SemaphoreID's old value)
		NAMESPACE_DECL_(kEventBitSet)=          -2147483644, //0x80000004 EventBitID of set bit (hgh bit is EventBitID's old value)
		NAMESPACE_DECL_(kEventBitCleared)=      -2147483643, //0x80000005EventBitID of cleared bit (high bit is EventBitID's old value)
		NAMESPACE_DECL_(kEventPended)=          -2147483642, //0x80000006 EventID of pended event (high bit is EventID's value)
		NAMESPACE_DECL_(kDeviceFlagPended)=     -2147483641, //0x80000007 DeviceFlagID of pended device flag
		NAMESPACE_DECL_(kDeviceFlagPosted)=     -2147483640, //0x80000008 DeviceFlagID of posted device flag
		NAMESPACE_DECL_(kDeviceActivated)=      -2147483639, //0x80000009DeviceID of activated device
		NAMESPACE_DECL_(kThreadTimedOut)=       -2147483638, //0x8000000a Semaphore/Event/DeviceFlagID thread timed out on
		NAMESPACE_DECL_(kThreadStatusChange)=   -2147483637, //0x8000000b ThreadStatus of thread BEFORE change
		NAMESPACE_DECL_(kThreadSwitched)=       -2147483636, //0x8000000c ThreadID of LAST running thread
		NAMESPACE_DECL_(kMaxStackUsed)=         -2147483635, //0x8000000d Maximum stack used
		NAMESPACE_DECL_(kPoolCreated)=          -2147483634, //0x8000000e MemoryPool Created
		NAMESPACE_DECL_(kPoolDestroyed)=        -2147483633, //0x8000000f MemoryPool Destroyed
		NAMESPACE_DECL_(kDeviceFlagCreated)=    -2147483632, //0x80000010 Device flag created
		NAMESPACE_DECL_(kDeviceFlagDestroyed)=  -2147483631, //0x80000011 Device flag destroyed
		NAMESPACE_DECL_(kMessagePosted)=        -2147483630, //0x80000012 MessageID of message posted
		NAMESPACE_DECL_(kMessagePended)=        -2147483629, //0x80000013 Pend channel mask OR'd with current message pending mask
		NAMESPACE_DECL_(kSemaphoreCreated)=     -2147483628, //0x80000014 SemaphoreID of semaphore created
		NAMESPACE_DECL_(kSemaphoreDestroyed)=   -2147483627, //0x80000015 SemaphoreID of semaphore destroyed
		NAMESPACE_DECL_(kMessageCreated)=       -2147483626, //0x80000016 MessageID of message created
		NAMESPACE_DECL_(kMessageDestroyed)=     -2147483625, //0x80000017 MessageID of message destroyed
		NAMESPACE_DECL_(kMessageTakenFromQueue)=-2147483624, //0x80000018 MessageID of message retrieved
		NAMESPACE_DECL_(kThreadResourcesFreed)= -2147483623, //0x80000019 we freed the thread's resources after a destruction
		NAMESPACE_DECL_(kThreadPriorityChanged)=-2147483622, //0x8000001a the thread's priority has  changed
		NAMESPACE_DECL_(kMutexCreated)=         -2147483621, //0x8000001b MutexID of created mutex
		NAMESPACE_DECL_(kMutexDestroyed)=       -2147483620, //0x8000001c MutexID of destroyed mutex
		NAMESPACE_DECL_(kMutexAcquired)=        -2147483619, //0x8000001d MutexID of acquired mutex
		NAMESPACE_DECL_(kMutexReleased)=        -2147483618, //0x8000001e MutexID of released mutex
        NAMESPACE_DECL_(kFifoCreated)=          -2147483617, //0x8000001f FifoID of the created Fifo
        NAMESPACE_DECL_(kFifoDestroyed)=        -2147483616, //0x80000020 FifoID of the destroyed Fifo
        NAMESPACE_DECL_(kFifoPended)=           -2147483615, //0x80000021 FifoID of the pended Fifo
        NAMESPACE_DECL_(kFifoPosted)=           -2147483614, //0x80000022 FifoID of the posted Fifo
		NAMESPACE_DECL_(kUserEvent)=            1
    } NAMESPACE_DECL_(HistoryEnum);

	typedef unsigned int NAMESPACE_DECL_(Bitfield);

	typedef struct NAMESPACE_DECL_(EventData)		 // TODO: Check this syntax...
	{
		bool			            matchAll;
		unsigned int	values;
		unsigned int	mask;
	} NAMESPACE_DECL_(EventData);

	typedef enum 
	{ 
		NAMESPACE_DECL_(kBlockedOnFifoPost),
		NAMESPACE_DECL_(kBlockedOnFifoPend) 
	} NAMESPACE_DECL_(FifoBlockingDirection); 
	
	// Holds the boot object information
	//
	typedef struct BootIOObjectInfo	
	{
		 NAMESPACE_(IOTemplateID) 	m_ioTemplateID;
		 NAMESPACE_(IOID)       m_ioDeviceID;
		 long                   m_instanceNum;
	}NAMESPACE_DECL_(BootIOObjectInfo); 


	// Holds the boot thread information
	//
	struct NAMESPACE_DECL_(ThreadTemplate);
	typedef struct BootThreadInfo	
	{
		 struct NAMESPACE_(ThreadTemplate*) m_templatePtr;
		 long                   m_instanceNum;
	}NAMESPACE_DECL_(BootThreadInfo); 

	typedef enum 
	{
		NAMESPACE_DECL_(kUnknownFamily),
		NAMESPACE_DECL_(kSHARC),
		NAMESPACE_DECL_(kTSXXX),
		NAMESPACE_DECL_(kBLACKFIN)
	} NAMESPACE_DECL_(DSP_Family);

	typedef enum 
	{
        NAMESPACE_DECL_(k21060) = 1,    // 0x00000001
        NAMESPACE_DECL_(k21061) = 2,    // 0x00000002
        NAMESPACE_DECL_(k21062) = 3,    // 0x00000003
        NAMESPACE_DECL_(k21065) = 4,    // 0x00000004
        NAMESPACE_DECL_(k21160) = 5,    // 0x00000005
        NAMESPACE_DECL_(k21161) = 6,    // 0x00000006
        NAMESPACE_DECL_(k21261) = 7,    // 0x00000007
        NAMESPACE_DECL_(k21262) = 8,    // 0x00000008
        NAMESPACE_DECL_(k21266) = 9,    // 0x00000009
        NAMESPACE_DECL_(k21267) = 10,   // 0x0000000a
        NAMESPACE_DECL_(k21362) = 11,   // 0x0000000b
        NAMESPACE_DECL_(k21363) = 12,   // 0x0000000c
        NAMESPACE_DECL_(k21364) = 13,   // 0x0000000d
        NAMESPACE_DECL_(k21365) = 14,   // 0x0000000e
        NAMESPACE_DECL_(k21366) = 15,   // 0x0000000f
        NAMESPACE_DECL_(k21367) = 16,   // 0x00000010
        NAMESPACE_DECL_(k21368) = 17,   // 0x00000011
        NAMESPACE_DECL_(k21369) = 18,   // 0x00000012
        NAMESPACE_DECL_(k21371) = 19,   // 0x00000013
        NAMESPACE_DECL_(k21375) = 20,   // 0x00000014
        NAMESPACE_DECL_(k21462) = 21,   // 0x00000015
        NAMESPACE_DECL_(k21465) = 22,   // 0x00000016
        NAMESPACE_DECL_(k21467) = 23,   // 0x00000017
        NAMESPACE_DECL_(k21469) = 24,   // 0x00000018
        NAMESPACE_DECL_(k21471) = 26,   // 0x0000001a
        NAMESPACE_DECL_(k21472) = 27,   // 0x0000001b
        NAMESPACE_DECL_(k21475) = 30,   // 0x0000001e
        NAMESPACE_DECL_(k21478) = 33,   // 0x00000021
        NAMESPACE_DECL_(k21479) = 34,   // 0x00000022
        NAMESPACE_DECL_(k21481) = 36,   // 0x00000024
        NAMESPACE_DECL_(k21482) = 37,   // 0x00000025
        NAMESPACE_DECL_(k21483) = 38,   // 0x00000026
        NAMESPACE_DECL_(k21485) = 40,   // 0x00000028
        NAMESPACE_DECL_(k21486) = 41,   // 0x00000029
        NAMESPACE_DECL_(k21487) = 42,   // 0x0000002a
        NAMESPACE_DECL_(k21488) = 43,   // 0x0000002b
        NAMESPACE_DECL_(k21489) = 44,   // 0x0000002c

        NAMESPACE_DECL_(kBF512) = 4097, // 0x00001001
        NAMESPACE_DECL_(kBF514) = 4098, // 0x00001002
        NAMESPACE_DECL_(kBF516) = 4099, // 0x00001003
        NAMESPACE_DECL_(kBF518) = 4100, // 0x00001004
        NAMESPACE_DECL_(kBF522) = 4101, // 0x00001005
        NAMESPACE_DECL_(kBF523) = 4102, // 0x00001006
        NAMESPACE_DECL_(kBF524) = 4103, // 0x00001007
        NAMESPACE_DECL_(kBF525) = 4104, // 0x00001008
        NAMESPACE_DECL_(kBF526) = 4105, // 0x00001009
        NAMESPACE_DECL_(kBF527) = 4106, // 0x0000100a
        NAMESPACE_DECL_(kBF531) = 4107, // 0x0000100b
        NAMESPACE_DECL_(kBF532) = 4108, // 0x0000100c
        NAMESPACE_DECL_(kBF533) = 4109, // 0x0000100d
        NAMESPACE_DECL_(kBF534) = 4110, // 0x0000100e
        NAMESPACE_DECL_(kBF535) = 4111, // 0x0000100f
        NAMESPACE_DECL_(kBF536) = 4112, // 0x00001010
        NAMESPACE_DECL_(kBF537) = 4113, // 0x00001011
        NAMESPACE_DECL_(kBF538) = 4114, // 0x00001012
        NAMESPACE_DECL_(kBF539) = 4115, // 0x00001013
        NAMESPACE_DECL_(kBF542) = 4117, // 0x00001015
        NAMESPACE_DECL_(kBF544) = 4118, // 0x00001016
        NAMESPACE_DECL_(kBF547) = 4119, // 0x00001017
        NAMESPACE_DECL_(kBF548) = 4120, // 0x00001018
        NAMESPACE_DECL_(kBF549) = 4121, // 0x00001019
        NAMESPACE_DECL_(kBF542M)= 4122, // 0x0000101a
        NAMESPACE_DECL_(kBF544M)= 4123, // 0x0000101b
        NAMESPACE_DECL_(kBF547M)= 4124, // 0x0000101c
        NAMESPACE_DECL_(kBF548M)= 4125, // 0x0000101d
        NAMESPACE_DECL_(kBF549M)= 4126, // 0x0000101e
        NAMESPACE_DECL_(kBF561) = 4127, // 0x0000101f
        NAMESPACE_DECL_(kBF504) = 4128, // 0x00001020
        NAMESPACE_DECL_(kBF504F)= 4129, // 0x00001021
        NAMESPACE_DECL_(kBF506F)= 4130, // 0x00001022
		NAMESPACE_DECL_(kBF592A)= 4131, // 0x00001023

		NAMESPACE_DECL_(kTS101) = 8193, // 0x00002001
		NAMESPACE_DECL_(kTS201) = 8194, // 0x00002002
		NAMESPACE_DECL_(kTS202) = 8195, // 0x00002003
		NAMESPACE_DECL_(kTS203) = 8196  // 0x00002004
	} NAMESPACE_DECL_(DSP_Product);

	typedef struct NAMESPACE_DECL_(VersionStruct)		 // TODO: Check this syntax...
	{
		int								mAPIVersion;
		NAMESPACE_(DSP_Family)			mFamily;
		NAMESPACE_(DSP_Product)			mProduct;
		long							mBuildNumber;
	} NAMESPACE_DECL_(VersionStruct);
	
#define INT_BITS_ (sizeof(int) * (unsigned int) CHAR_BIT)
	typedef enum
	{                               
		NAMESPACE_DECL_(kMinPriority)  = 1U,
		NAMESPACE_DECL_(kPriority1)    = INT_BITS_ - 2U ,
		NAMESPACE_DECL_(kPriority2)    = INT_BITS_ - 3U ,
		NAMESPACE_DECL_(kPriority3)    = INT_BITS_ - 4U ,
		NAMESPACE_DECL_(kPriority4)    = INT_BITS_ - 5U ,
		NAMESPACE_DECL_(kPriority5)    = INT_BITS_ - 6U ,
		NAMESPACE_DECL_(kPriority6)    = INT_BITS_ - 7U ,
		NAMESPACE_DECL_(kPriority7)    = INT_BITS_ - 8U ,
		NAMESPACE_DECL_(kPriority8)    = INT_BITS_ - 9U ,
		NAMESPACE_DECL_(kPriority9)    = INT_BITS_ - 10U,
		NAMESPACE_DECL_(kPriority10)   = INT_BITS_ - 11U,
		NAMESPACE_DECL_(kPriority11)   = INT_BITS_ - 12U,
		NAMESPACE_DECL_(kPriority12)   = INT_BITS_ - 13U,
		NAMESPACE_DECL_(kPriority13)   = INT_BITS_ - 14U,
		NAMESPACE_DECL_(kPriority14)   = INT_BITS_ - 15U,
#if UINT_MAX > 65535U    
		NAMESPACE_DECL_(kPriority15)   = INT_BITS_ - 16U,
		NAMESPACE_DECL_(kPriority16)   = INT_BITS_ - 17U,
		NAMESPACE_DECL_(kPriority17)   = INT_BITS_ - 18U,
		NAMESPACE_DECL_(kPriority18)   = INT_BITS_ - 19U,
		NAMESPACE_DECL_(kPriority19)   = INT_BITS_ - 20U,
		NAMESPACE_DECL_(kPriority20)   = INT_BITS_ - 21U,
		NAMESPACE_DECL_(kPriority21)   = INT_BITS_ - 22U,
		NAMESPACE_DECL_(kPriority22)   = INT_BITS_ - 23U,
		NAMESPACE_DECL_(kPriority23)   = INT_BITS_ - 24U,
		NAMESPACE_DECL_(kPriority24)   = INT_BITS_ - 25U,
		NAMESPACE_DECL_(kPriority25)   = INT_BITS_ - 26U,
		NAMESPACE_DECL_(kPriority26)   = INT_BITS_ - 27U,
		NAMESPACE_DECL_(kPriority27)   = INT_BITS_ - 28U,
		NAMESPACE_DECL_(kPriority28)   = INT_BITS_ - 29U,
		NAMESPACE_DECL_(kPriority29)   = INT_BITS_ - 30U,
		NAMESPACE_DECL_(kPriority30)   = INT_BITS_ - 31U,
#endif
		NAMESPACE_DECL_(kMaxPriority)  = NAMESPACE_DECL_(kPriority1)

	} NAMESPACE_DECL_(Priority);

	typedef enum
	{									 
		NAMESPACE_DECL_(kMsgWaitForAll)= 1Ul << 15,
		NAMESPACE_DECL_(kMsgChannel1 ) = 1Ul << 14,
		NAMESPACE_DECL_(kMsgChannel2 ) = 1Ul << 13,
		NAMESPACE_DECL_(kMsgChannel3 ) = 1Ul << 12,
		NAMESPACE_DECL_(kMsgChannel4 ) = 1Ul << 11,
		NAMESPACE_DECL_(kMsgChannel5 ) = 1Ul << 10,
		NAMESPACE_DECL_(kMsgChannel6 ) = 1Ul << 9,
		NAMESPACE_DECL_(kMsgChannel7 ) = 1Ul << 8,
		NAMESPACE_DECL_(kMsgChannel8 ) = 1Ul << 7,
		NAMESPACE_DECL_(kMsgChannel9 ) = 1Ul << 6,
		NAMESPACE_DECL_(kMsgChannel10) = 1Ul << 5,
		NAMESPACE_DECL_(kMsgChannel11) = 1Ul << 4,
		NAMESPACE_DECL_(kMsgChannel12) = 1Ul << 3,
		NAMESPACE_DECL_(kMsgChannel13) = 1Ul << 2,
		NAMESPACE_DECL_(kMsgChannel14) = 1Ul << 1,
		NAMESPACE_DECL_(kMsgChannel15) = 1Ul << 0,
        NAMESPACE_DECL_(kAnyMsgChannel) = NAMESPACE_DECL_(kMsgChannel1 )
                                        | NAMESPACE_DECL_(kMsgChannel2 )
                                        | NAMESPACE_DECL_(kMsgChannel3 )
                                        | NAMESPACE_DECL_(kMsgChannel4 )
                                        | NAMESPACE_DECL_(kMsgChannel5 )
                                        | NAMESPACE_DECL_(kMsgChannel6 )
                                        | NAMESPACE_DECL_(kMsgChannel7 )
                                        | NAMESPACE_DECL_(kMsgChannel8 )
                                        | NAMESPACE_DECL_(kMsgChannel9 )
                                        | NAMESPACE_DECL_(kMsgChannel10 )
                                        | NAMESPACE_DECL_(kMsgChannel11 )
                                        | NAMESPACE_DECL_(kMsgChannel12 )
                                        | NAMESPACE_DECL_(kMsgChannel13 )
                                        | NAMESPACE_DECL_(kMsgChannel14 )
                                        | NAMESPACE_DECL_(kMsgChannel15 )
	} NAMESPACE_DECL_(MsgChannel);	   



#ifdef __cplusplus
    static const unsigned int kNoTimeoutError = (unsigned int)INT_MIN;
    static const unsigned int kDoNotWait = UINT_MAX;
#else    /* not __cplusplus */
    #define VDK_kNoTimeoutError ( (unsigned int) INT_MIN )
    #define VDK_kDoNotWait  (UINT_MAX)
#endif /* __cplusplus */

    /**
     * Sets up VDK's interrupt handlers, etc. and creates
     * the Idle thread.
     * 
	 * Implemented in file: Initialize.cpp
     */
    void
    NAMESPACE_DECL_(Initialize)( void );

    /**
     * Transfers control to the VDK scheduler and begins
     * running threads.
     * 
     * Never returns.
	 *
	 * Implemented in file: Initialize.cpp
     */
    void
    NAMESPACE_DECL_(Run)(void);


    /**
     * Disables interrupts so that a group of instructions can be executed
     * atomically.  Note that critical regions may be nested.  A count is
     * maintained, such that an equal number of calls to VDK::ExitCriticalRegion()
     * must be made before interrupts are restored.
	 *
	 * Implemented in file: VDK_API_Regions.cpp
     */
    void
    NAMESPACE_DECL_(PushCriticalRegion)( void );

    /**
     * Re-enables interrupts following a group of atomically executed
     * instructions.  Note that critical regions may be nested.  A count is
     * maintained, such that each call to VDK::EnterCriticalRegion() must be
     * matched by a call to VDK::ExitCriticalRegion() before interrupts are
     * restored.  Additional calls to VDK::ExitCriticalRegion() while interrupts
     * are enabled are ignored.
	 *
	 * Implemented in file: VDK_API_Regions.cpp
     */
    void
    NAMESPACE_DECL_(PopCriticalRegion)( void );

    /**
     * Re-enables interrupts following a group of atomically executed
     * instructions.  Note that critical regions may be nested.  A count is
     * maintained, such that each call to VDK::EnterCriticalRegion() must be
     * matched by a call to VDK::ExitCriticalRegion() before interrupts are
     * restored.  Additional calls to VDK::ExitCriticalRegion() while interrupts
     * are enabled are ignored.
 	 *
	 * Implemented in file: VDK_API_Regions.cpp
     */
    void
    NAMESPACE_DECL_(PopNestedCriticalRegions)( void );

    /**
	 * This function disables the scheduler.  Subsequent calls to Pend(), 
	 * Post(), SetEventBit() and ClearEventBit() operate on signals as normal,
	 * but the high priority thread is not reselected from the ready queue.  
	 * This permits a thread to run without being preempted by another thread, 
	 * but without disabling interrupts.  If scheduling is already disabled, 
	 * the function call does nothing.  Note that unscheduled regions may be 
	 * nested.  A count is maintained, such that an equal number of calls to 
	 * PopUnscheduledRegion() must be made before scheduling is resumed.
 	 *
	 * Implemented in file: VDK_API_Regions.cpp
     */
    void
    NAMESPACE_DECL_(PushUnscheduledRegion)( void );

    /**
     * Restores normal scheduling operation and should be used to close bracket
     * a call to PushUnscheduledRegion().  .  Note that unscheduled regions may
     * be nested.  A count is maintained, such that each unscheduled region 
     * entered must be matched by call to PopUnscheduledRegion() before scheduling
     * is resumed.Additional calls to PopUnscheduledRegion() while scheduling 
     * is enabled are ignored.   
     * 
     * This function invokes the scheduler and may result in a context switch.
     *  A context switch will not occur if scheduling was already enabled
     *  prior to the function call.
 	 *
	 * Implemented in file: VDK_API_Regions.cpp
     */
    void
    NAMESPACE_DECL_(PopUnscheduledRegion)( void );

	/**
	 * Restores normal scheduling operation.  This function ignores and resets
	 * the current nesting count.  Additional calls to this function while 
	 * scheduling is enabled are ignored.
 	 *
	 * Implemented in file: VDK_API_Regions.cpp
	 */
  	void
	NAMESPACE_DECL_(PopNestedUnscheduledRegions)( void );

    /**
     * Creates a thread using the given template ID and returns
     * the new thread ID.  A NULL return value indicates a creation
     * failure.
     *
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_CreateThread.cpp
     */
   	NAMESPACE_(ThreadID)
	NAMESPACE_DECL_(CreateThread)( NAMESPACE_(ThreadType) inType );

	/**
	 * Used to pass data to our constructor through a developer's
	 * constructor without having them pass individual members up the
	 * construction call chain.
	 */
	typedef struct NAMESPACE_DECL_(ThreadCreationBlock)
	{
		NAMESPACE_(ThreadType)	template_id;
		NAMESPACE_(ThreadID) 	thread_id;
		unsigned int            thread_stack_size;
		NAMESPACE_(Priority) 	thread_priority;
		void                    *user_data_ptr;
		struct NAMESPACE_(ThreadTemplate)	*pTemplate;
#ifndef __ADSPTS__
        /* TigerSHARC does not support CreateThread with stack pointer */
        unsigned int            *stack_pointer;
#endif
	} NAMESPACE_DECL_(ThreadCreationBlock);

#ifndef __ADSPTS__
    /* TigerSHARC does not support CreateThread with stack pointer */
    typedef enum NAMESPACE_DECL_(TCBBitfield)
    {
        NAMESPACE_DECL_(kSetThreadTemplateID)       = 1 << 0,
        NAMESPACE_DECL_(kSetThreadStackSize)        = 1 << 1,
        NAMESPACE_DECL_(kSetThreadPriority)         = 1 << 2,
        NAMESPACE_DECL_(kSetUserDataPointer)        = 1 << 3,
        NAMESPACE_DECL_(kSetThreadTemplatePointer)  = 1 << 4,
        NAMESPACE_DECL_(kSetThreadStackPointer)     = 1 << 5
    } NAMESPACE_DECL_(TCBBitfield);

   	NAMESPACE_(ThreadID)
	NAMESPACE_DECL_(CreateThreadEx2)( NAMESPACE_(ThreadCreationBlock)* inOutTCB, int fieldsRequired );

    unsigned int
    NAMESPACE_DECL_(GetContextRecordSize)( void );
#endif

   	NAMESPACE_(ThreadID)
	NAMESPACE_DECL_(CreateThreadEx)( NAMESPACE_(ThreadCreationBlock)* inOutTCB );

    /**
     * This function initiates the process of removing a thread from the 
     * system.  Although the thread will never be scheduled again once this
     * function completes, the resources associated with the thread are
     * deallocated by the kernel at a low priority level when time permits
	 * if inDestroyNow has a value of false.  If true, the memory is free 
     * before continuing execution.  A thread may remove itself from the 
     * system by passing its own thread ID to VDK::DestroyThread().
     *
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_DestroyThread.cpp
     */
    void
    NAMESPACE_DECL_(DestroyThread)(NAMESPACE_(ThreadID) inThreadID, bool inDestroyNow );


	/** 
	 * This function is intended to be called only after a call to 
	 * DestroyThread(xxx,false).  In such a case, a system would rely on the
	 * idle thread to free the memory associated with the destroyed thread.
	 * This API causes any such memory to be freed before continuing execution,
	 * It may be called from any thread and frees the memory from any number
	 * of threads (zero or more).
	 */
	void
	NAMESPACE_DECL_(FreeDestroyedThreads)( void );


    /**
     * Returns the Thread ID of the currently running thread....
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
    NAMESPACE_(ThreadID)
    NAMESPACE_DECL_(GetThreadID)( void );

    /**
     * Returns the ID of the template used to create the thread.
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
    NAMESPACE_ (ThreadType)
    NAMESPACE_DECL_(GetThreadType)( const NAMESPACE_(ThreadID) inThreadID );

    /**
     * Returns the status of the thread with the given ID as a value from an
     * enumeration.  The possible values are READY, RUNNING, WAITING and
     * UNDEFINED (thread ID is not valid).
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
    NAMESPACE_(ThreadStatus)
    NAMESPACE_DECL_(GetThreadStatus)( const NAMESPACE_(ThreadID) inThreadID );	   

    /**
     * Returns a handle (void**) that can be used in C and asm threads for
	 * holding thread local state (i.e. member variables)
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
    void**
    NAMESPACE_DECL_(GetThreadHandle)( void );

	/** 
	 * Retrieves the number of threads currently in the system, and 
	 * optionally fills the given array with the ThreadIDs
	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
	 */
	int
	NAMESPACE_DECL_(GetAllThreads)( NAMESPACE_(ThreadID) outThreadIDArray[], 
								    int inArraySize );

    /**
     * Retrieves the ID of the object that the given thread
	 * last blocked on.
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetThreadBlockingID)( const NAMESPACE_(ThreadID) inThreadID, 
										  int *outBlockingID );
    
	/**
     * Retrieves the template name for a given thread.
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetThreadTemplateName)( const NAMESPACE_(ThreadID) inThreadID, 
											char 	**outName );
    
	/**
     * Retrieves the stack details (size, start address) for a given thread.
 	 *
	 * Implemented in file: VDK_API_ThreadStack.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetThreadStackDetails)( const NAMESPACE_(ThreadID) inThreadID, 
											unsigned int 	*outStackSize,
											void 			**outStackAddress );
								
#ifdef __ADSPTS__
    /**
     * Retrieves the stack details (size, start address) for the second
	 * stack of a given thread.
 	 * Only available for TS.
	 *
	 * Implemented in file: VDK_API_ThreadStack.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetThreadStack2Details)( const NAMESPACE_(ThreadID) inThreadID, 
											 unsigned int 	*outStackSize,
											 void 			**outStackAddress );
#endif


	/** 
	* Thread DebugInfo (number of times run, Cycle data and Tick data) is only 
	* logged when using Fully Instrumentated libraries. Using conditional 
	* compilation to produce a compile time error if a customer calls these
	* APIs using error checking and non-error checking libraries.
	*/
#if defined (VDK_INSTRUMENTATION_LEVEL_) && (VDK_INSTRUMENTATION_LEVEL_==2)
    
	/**
     * Retrieves the number of times that a thread has run. Returns
	 * kNoError if successful, and kUnknownThread if the thread does not exist.
 	 *
	 * Implemented in file: VDK_API_ThreadDbg.cpp
     */
    NAMESPACE_(SystemError)
    NAMESPACE_DECL_(GetNumTimesRun)( const NAMESPACE_(ThreadID) inThreadID, 
									 unsigned int *outRunCount );

#ifdef __ADSP21000__ 

	/**
     * Retrieves the debug cycle data recorded for the given thread. Returns
	 * kNoError if successful, and kUnknownThread if the thread does not exist.
 	 * This API is currently not available for SHARC.
	 * 
	 * Implemented in file: VDK_API_ThreadDbg.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetSharcThreadCycleData)( const NAMESPACE_(ThreadID) inThreadID,
											  unsigned int outCreationTime[2],
											  unsigned int outStartTime[2],
											  unsigned int outLastTime[2],
											  unsigned int outTotalTime[2] );

#else
	/**
     * Retrieves the debug cycle data recorded for the given thread. Returns
	 * kNoError if successful, and kUnknownThread if the thread does not exist.
 	 * This API is currently not available for SHARC due to the compiler not 
	 * supporting long long ints.
	 * 
	 * Implemented in file: VDK_API_ThreadDbg.cpp
     */
    NAMESPACE_(SystemError)
    NAMESPACE_DECL_(GetThreadCycleData)( const NAMESPACE_(ThreadID) inThreadID, 
										 unsigned long long int *outCreationTime,
										 unsigned long long int *outStartTime,
										 unsigned long long int *outLastTime,
										 unsigned long long int *outTotalTime );

#endif

	/**
     * Retrieves the debug Tick data recorded for the given thread. Returns
	 * kNoError if successful, and kUnknownThread if the thread does not exist.
	 * 
	 * Implemented in file: VDK_API_ThreadDbg.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetThreadTickData)( const NAMESPACE_(ThreadID) inThreadID,
										NAMESPACE_(Ticks) *outCreationTime,
										NAMESPACE_(Ticks) *outStartTime,
										NAMESPACE_(Ticks) *outLastTime,
										NAMESPACE_(Ticks) *outTotalTime);

#endif /* VDK_INSTRUMENTATION_LEVEL_ */

	/**
	 * Returns the current build number in a VersionStruct
	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
	 */
	NAMESPACE_(VersionStruct)    
    NAMESPACE_DECL_(GetVersion)( void );


    /**
     * Returns the priority of the thread with the given ID.
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
     */
    NAMESPACE_(Priority)
    NAMESPACE_DECL_(GetPriority)( const NAMESPACE_(ThreadID) inThreadID );

    /**
     * Set the priority of the thread with the given ID to the given level.
 	 *
	 * Implemented in file: VDK_API_Priority.cpp
     */
    void
    NAMESPACE_DECL_(SetPriority)( const NAMESPACE_(ThreadID) inThreadID,
                                  const NAMESPACE_(Priority) inPriority );

    /**
     * Resets the priority of the thread with the given ID to the default
     * priority as given in the thread's template.  This VDK::SetPriority uses
     * the thread table to find a ptr to the thread.  The ID of the thread
     * template is stored in VDK::Thread.
     * 
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_Priority.cpp
     */
    void
    NAMESPACE_DECL_(ResetPriority)( const NAMESPACE_(ThreadID) inThreadID );

    /**
     * Yields control of the processor when called from a thread at a priority
     * level using cooperative multitasking.  This command (or a Pend()
     * command that results in a context switch) will cause the thread to
     * be move to the end of the queue of threads at that priority.  If
     * called from a thread at a priority level using round robin multitasking,
     * the call also yield the remainder of the thread's time slice.  
 	 *
	 * Implemented in file: VDK_API_Yield.cpp
     */
    void
    NAMESPACE_DECL_(Yield)( void );

    /**
     * This function causes a thread to pause execution for at least the given
     * number of clock ticks.  After sleepTicks ticks have elapsed, the
     * calling thread is marked by the scheduler as being ready to run.  The
     * thread will actually resume execution at that time only if it is the
     * highest priority thread with ready status.
 	 *
	 * Implemented in file: VDK_API_Yield.cpp
     */
    void
    NAMESPACE_DECL_(Sleep)( NAMESPACE_(Ticks) inSleepTicks );

/****************************************************************************/
/* Timer functions                                                          */
/****************************************************************************/
	/**
	 * Returns the time, in ticks, since the system was last reset.
 	 *
	 * Implemented in file: VDK_API_ThreadAcc.cpp
	 */
#pragma linkage_name _TMK_GetUptime
  	NAMESPACE_(Ticks)
	NAMESPACE_DECL_(GetUptime)( void );

	/**
	 * Allows users to change the clock frequency and will recalculate
	 * the timer parameters. For that, it needs to stop and restart it.
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    void
    NAMESPACE_DECL_(SetClockFrequency)( unsigned int inFrequency);

	/**
	 * Returns the value of variable g_ClockFrequency
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

	/**
	 * Allows users to change the clock frequency and will recalculate
	 * the timer parameters. For that, it needs to stop and restart it.
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    void
    NAMESPACE_DECL_(SetClockFrequencyKHz)( float inFrequency);

	/**
	 * Returns the value of variable g_ClockFrequency
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    unsigned int
    NAMESPACE_DECL_(GetClockFrequency)( void );

	/**
	 * Allows users to change the tick period and will recalculate
	 * the timer parameters. For that, it needs to stop and restart it.
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    float
    NAMESPACE_DECL_(GetClockFrequencyKHz)( void );

	/**
	 * Allows users to change the tick period and will recalculate
	 * the timer parameters. For that, it needs to stop and restart it.
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    void
    NAMESPACE_DECL_(SetTickPeriod)( float inPeriod);

	/**
	 * Returns the value of variable g_ClockFrequency
	 *
	 * Implemented in VDK_API_Time.cpp
	 */

    float
    NAMESPACE_DECL_(GetTickPeriod)( void );


/****************************************************************************/
/* Device Driver code                                                       */
/****************************************************************************/


    typedef enum NAMESPACE_DECL_(DispatchID)
    {
        NAMESPACE_DECL_(kIO_Init),
        NAMESPACE_DECL_(kIO_Activate),
        NAMESPACE_DECL_(kIO_Open),
        NAMESPACE_DECL_(kIO_Close),
        NAMESPACE_DECL_(kIO_SyncRead),
        NAMESPACE_DECL_(kIO_SyncWrite),
        NAMESPACE_DECL_(kIO_IOCtl)
    }NAMESPACE_DECL_(DispatchID);

    typedef union NAMESPACE_DECL_(DispatchUnion)		 // TODO: Check this syntax...
    {
        struct
        {
            void        **dataH;
            char        *flags;     /* used for kIO_Open only */
        } OpenClose_t;
        struct
        {
            void                **dataH;
            NAMESPACE_(Ticks)   timeout;
            unsigned int        dataSize;
            char                *data;
        } ReadWrite_t;
        struct
        {
            void        **dataH;
            void        *command;
            char        *parameters;
        } IOCtl_t;
		struct
		{
			void        *pInitInfo;
		} Init_t;
    } NAMESPACE_DECL_(DispatchUnion);


    /**
     * Opens the specified device
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    NAMESPACE_(DeviceDescriptor)
    NAMESPACE_DECL_(OpenDevice)( NAMESPACE_(IOID)	inIDNum,
                                 			char        *inFlags );

    /**
     * Closes the specified device
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    void
    NAMESPACE_DECL_(CloseDevice)( NAMESPACE_(DeviceDescriptor)    inDD );


    /**
     * Reads data synchronously from the device
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    unsigned int
    NAMESPACE_DECL_(SyncRead)(  NAMESPACE_(DeviceDescriptor)    inDD,
                                char                            *outBuffer,
                                const unsigned int              inSize,
                                NAMESPACE_(Ticks)               inTimeout   );

    /**
     * Reads data synchronously from the device
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    unsigned int
    NAMESPACE_DECL_(SyncWrite)( NAMESPACE_(DeviceDescriptor)    inDD,
                                char                            *inBuffer,
                                const unsigned int              inSize,
                                NAMESPACE_(Ticks)               inTimeout   );

    /**
     * Controls a specified device
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    int
    NAMESPACE_DECL_(DeviceIOCtl)(   NAMESPACE_(DeviceDescriptor)    inDD,
                                    void                            *inCommand,
                                    char                            *inParameters );


	/* Create Device flag object
	 */
	NAMESPACE_(DeviceFlagID)
	NAMESPACE_DECL_(CreateDeviceFlag)(void);

	/*
	 *  Destroy device flag object
	 */ 
	void
	NAMESPACE_DECL_(DestroyDeviceFlag)(NAMESPACE_(DeviceFlagID) inFlag);
    
	/**
     * Makes a thread block on a device flag
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    bool
    NAMESPACE_DECL_(PendDeviceFlag)(    NAMESPACE_(DeviceFlagID)	inFlag,
                                        NAMESPACE_(Ticks)   inTimeout );

    /**
     * Posts a specified device flag
 	 *
	 * Implemented in file: VDK_API_DeviceDrivers.cpp
     */
    void
    NAMESPACE_DECL_(PostDeviceFlag)(    NAMESPACE_(DeviceFlagID) 	inFlag );


	/**
	 * Gets all of the threads that are pending on a given DeviceFlagID.
 	 *
	 * Implemented in file: VDK_API_DeviceFlags.cpp
	 */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetDevFlagPendingThreads)( const NAMESPACE_(DeviceFlagID) inDevFlagID, 
											   int *outNumThreads, 
											   NAMESPACE_(ThreadID) outThreadArray[],
											   int inArraySize);


	/**
	 * Retrieves the number of device flags currently in the system, and 
	 * optionally fills the given array with the DeviceFlagIDs
	 *
	 * Implemented in file: VDK_API_DeviceFlags.cpp
	 */
	int
	NAMESPACE_DECL_(GetAllDeviceFlags)( NAMESPACE_(DeviceFlagID) outDevFlagIDArray[], 
										int inArraySize );


	/**
	 * IO device API's
	 */
    NAMESPACE_(IOID)
    NAMESPACE_DECL_(CreateIOObject)(NAMESPACE_(IOTemplateID) inTemplateID,
									void *pInitInfo);

	void
	NAMESPACE_DECL_(DestroyIOObject)(const NAMESPACE_(IOID) inID );

/****************************************
 * MESSAGES
****************************************/

    /**
     * This function provides the mechanism by which threads pend on signals.
	 *
	 * Creates and initializes a new message object.  If inNumElements is non-zero, 
	 *   the payload buffer must be allocated before the message is created.
	 * 
	 * Implemented in file: VDK_API_Message.cpp
	 */

	NAMESPACE_(MessageID)
	NAMESPACE_DECL_(CreateMessage)( int inPayloadType,
									unsigned int inPayloadSize,
									void *inPayloadAddr);


    /**
     * Destroys a message object.  The message payload memory is assumed to 
     *   already have been freed, either by the user thread or by the kernel, prior 
     *   to this API call.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	void 
	NAMESPACE_DECL_(DestroyMessage)(NAMESPACE_(MessageID) inID);



    /**
     * Destroys a message object.  The message payload memory is also freed
     *   if the payload type is a marshalled type.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	void 
	NAMESPACE_DECL_(DestroyMessageAndFreePayload)(NAMESPACE_(MessageID) inID);

    /**
     * Frees the payload of a message if it is a marshalled type.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	void
	NAMESPACE_DECL_(FreeMessagePayload)(NAMESPACE_(MessageID) inID);


    /**
     * Appends the message inMessageID to the message queue of the inRecipient 
     *   thread on the channel inChannel.  PostMessage() is a non-blocking function,
     *   and returns execution to the calling thread without waiting for the 
     *   recipient to run or acknowledge new the message in its queue.  The message
     *   is considered "delivered" when PostMessage() returns.  Once delivered, all
     *   memory references to the payload and to the message itself that are held
     *   by the sending thread should be considered invalid.  Ownership, memory
     *   read and write priviledges and responsibility for freeing the payload
     *   memory are passed to the recipient thread.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	void
	NAMESPACE_DECL_(PostMessage)(NAMESPACE_(ThreadID) inRecipient,
									NAMESPACE_(MessageID) inMessageID,
									NAMESPACE_(MsgChannel) inChannel);

    /**
     * Similar to PostMessage(), but sets the sender ID of the message to the value
	 *   specified in the fourth argument.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	void
	NAMESPACE_DECL_(ForwardMessage)(NAMESPACE_(ThreadID) inRecipient,
									NAMESPACE_(MessageID) inMessageID,
									NAMESPACE_(MsgChannel) inChannel,
									NAMESPACE_(ThreadID) inPseudoSender);

    /**
	 * If there is a message at any of the priority levels set in the mask, this function
	 *   returns the MessageID of the highest priority message set in that mask.  
	 *   Otherwise, the thread is removed from the ready queue until a message is posted 
	 *   to the thread's message queue at a priority level set in the mask.  The mask 
	 *   allows the thread to specify on which channels to receive a message.  The channels 
	 *   are numbered zero through fifteen, with zero being the highest priority.  For 
	 *   example, if the mask has the value 0x7FFF, messages will be received on all channels.  
	 *   A minimum of one message on an unmasked channel must be waiting in its message queue 
	 *   to prevent a thread from blocking on a call to PendMessage(). If bit 15 (kMsgWaitForAll)
	 *   is set in the mask then the call will block until at least one message is available
	 *   on all of the specified channels.  The mask may not have a value of zero (or 0x8000).
	 *   The function returns the ID of the highest 
	 *   priority message on an unmasked channel in the thread's message queue.  
	 *   Note: If the thread is moved to the ready queue by an incoming message, and then an 
	 *   additional higher priority message is received before the thread is run, the higher 
	 *   priority thread will be returned by PendMessage() when the thread resumes execution.  
	 *   The thread made ready by the first sufficient PostMessage() call, but does not fetch 
	 *   from its message until it actually runs.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	NAMESPACE_(MessageID)
	NAMESPACE_DECL_(PendMessage)(unsigned int inMsgChannelMask,
									NAMESPACE_(Ticks) inTimeout);

    /**
	 * MessageAvailable() enables a thread to poll its message queue.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */

	bool
	NAMESPACE_DECL_(MessageAvailable)(unsigned int inMsgPriorityMask);

	void
	NAMESPACE_DECL_(GetMessagePayload)(NAMESPACE_(MessageID) inMessageID,
									   int *pOutPayloadType,
									   unsigned int *pOutPayloadSize,
									   void **ppOutPayloadAddr);
	void 
	NAMESPACE_DECL_(SetMessagePayload)(NAMESPACE_(MessageID) inMessageID, 
									   int inPayloadType,
									   unsigned int inPayloadSize,
									   void *inPayloadAddr);
	void
	NAMESPACE_DECL_(GetMessageReceiveInfo)(NAMESPACE_(MessageID)  inMessageID,
										   NAMESPACE_(MsgChannel) *pOutChannel,
										   NAMESPACE_(ThreadID)   *pOutSender);

	/**
	 * Gets the total number of messages in the system, and optionally fills
	 * an array with the MessageIDs. 
	 *
	 * Implemented in file: VDK_API_Message.cpp	
	 */
	int 
	NAMESPACE_DECL_(GetAllMessages)( NAMESPACE_(MessageID) outMessageIDArray[],
									 int inArraySize );


	// New stuff for multiprocessor messaging
	typedef struct NAMESPACE_DECL_(PayloadDetails)
	{
		int type;
		unsigned int size;
		void *addr;
	} NAMESPACE_DECL_(PayloadDetails);

	typedef struct NAMESPACE_DECL_(MessageDetails)
	{
		NAMESPACE_(MsgChannel) channel;
		NAMESPACE_(ThreadID)   sender;
		NAMESPACE_(ThreadID)   target;
	} NAMESPACE_DECL_(MessageDetails);
	
	typedef enum NAMESPACE_DECL_(MarshallingCode)
	{
		TRANSMIT_AND_RELEASE,
		ALLOCATE_AND_RECEIVE,
		ALLOCATE,
		RELEASE
	} NAMESPACE_DECL_(MarshallingCode);

	struct NAMESPACE_DECL_(IOAbstractBase);
	struct NAMESPACE_DECL_(DeviceControlBlock);
	union NAMESPACE_DECL_(DispatchUnion);
	typedef void* (*NAMESPACE_DECL_(PFDispatchFunction))(struct NAMESPACE_(IOAbstractBase) *pThis,
														 enum NAMESPACE_(DispatchID) inCode,
														 union NAMESPACE_(DispatchUnion) *inUnion);

	typedef struct NAMESPACE_DECL_(DeviceInfoBlock)
	{
		NAMESPACE_(DeviceDescriptor) dd;
		NAMESPACE_(PFDispatchFunction) pfDispatchFunction;
		struct NAMESPACE_(IOAbstractBase) *pDevObj;
		struct NAMESPACE_(DeviceControlBlock) *pDcb;
	} NAMESPACE_DECL_(DeviceInfoBlock);

	typedef struct NAMESPACE_DECL_(MsgWireFormat)
	{
		unsigned int header;
		NAMESPACE_(PayloadDetails) payload;
	} NAMESPACE_DECL_(MsgWireFormat);

	typedef int (*NAMESPACE_DECL_(PFMarshaller))(NAMESPACE_(MarshallingCode) code,
								 NAMESPACE_(MsgWireFormat) *inOutMsgPacket,
								 NAMESPACE_(DeviceInfoBlock) *pDev,
								 unsigned int inArea,
								 NAMESPACE_(Ticks) inTimeout);
					 
	typedef struct NAMESPACE_DECL_(MarshallingEntry)
	{
		NAMESPACE_(PFMarshaller) pfMarshaller;
		unsigned int area;
	} NAMESPACE_DECL_(MarshallingEntry);

	typedef enum NAMESPACE_DECL_(RoutingDirection)
	{
		kINCOMING,
		kOUTGOING
	} NAMESPACE_DECL_(RoutingDirection);


#define MARSHALLINGINDEX(x) (-(x) - 1)
#define ISMARSHALLEDTYPE(x) ((x) < 0)

	struct NAMESPACE_DECL_(IOAbstractBase);
	union NAMESPACE_DECL_(DispatchUnion);

	void
	NAMESPACE_DECL_(GetMessageDetails)(NAMESPACE_(MessageID)  inMessageID,
									   NAMESPACE_(MessageDetails) *pOutMessageDetails,
									   NAMESPACE_(PayloadDetails) *pOutPayloadDetails);

	 /**
     * Similar to GetMessageDetails but with two main differences:
	 *		The calling thread does not have to be the owner.
	 *		The message can be in a queue.
 	 *
	 * Implemented in file: VDK_API_Message.cpp
	 */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetMessageStatusInfo)( NAMESPACE_(MessageID) inMessageID,
								NAMESPACE_(MessageDetails) *outMessageDetails,
								NAMESPACE_(PayloadDetails) *outPayloadDetails );

	int
	NAMESPACE_DECL_(PoolMarshaller) (NAMESPACE_(MarshallingCode) code,
									 NAMESPACE_(MsgWireFormat) *pMsgPacket,
									 NAMESPACE_(DeviceInfoBlock) *pDev,
									 unsigned int inArea,
									 NAMESPACE_(Ticks) inTimeout);

	int
	NAMESPACE_DECL_(EmptyPoolMarshaller) (NAMESPACE_(MarshallingCode) code,
										  NAMESPACE_(MsgWireFormat) *pMsgPacket,
										  NAMESPACE_(DeviceInfoBlock) *pDev,
										  unsigned int inArea,
										  NAMESPACE_(Ticks) inTimeout);

	int
	NAMESPACE_DECL_(HeapMarshaller) (NAMESPACE_(MarshallingCode) code,
									 NAMESPACE_(MsgWireFormat) *pMsgPacket,
									 NAMESPACE_(DeviceInfoBlock) *pDev,
									 unsigned int heapID,
									 NAMESPACE_(Ticks) inTimeout);

	int
	NAMESPACE_DECL_(EmptyHeapMarshaller) (NAMESPACE_(MarshallingCode) code,
										  NAMESPACE_(MsgWireFormat) *pMsgPacket,
										  NAMESPACE_(DeviceInfoBlock) *pDev,
										  unsigned int heapID,
										  NAMESPACE_(Ticks) inTimeout);
#if defined(__ADSPTS__) || defined(__2106x__) || defined(__2116x__)
	int
	NAMESPACE_DECL_(ClusterBusMarshaller) (NAMESPACE_(MarshallingCode) code,
										   NAMESPACE_(MsgWireFormat) *pMsgPacket,
										   NAMESPACE_(DeviceInfoBlock) *pDev,
										   unsigned int heapID,
										   NAMESPACE_(Ticks) inTimeout);
#endif

	void
	NAMESPACE_DECL_(InstallMessageControlSemaphore)(NAMESPACE_(SemaphoreID) inSemaphore);

	unsigned int
	NAMESPACE_DECL_(GetHeapIndex)(NAMESPACE_(HeapID) inHeapID);

#ifdef __ADSPTS__

#define INIT_THREADTEMPLATE_(name,priority,stacksize,stack2size,createfunction,contextheap, stackheap, context2heap, stack2heap,threadheap, messagesallowed) \
	{ (name), (contextheap), (stackheap), (context2heap), (stack2heap), (threadheap), \
	(priority),(stacksize),(stack2size),(createfunction), (messagesallowed) }

#define INIT_RTHREAD_(name,priority,stacksize,stack2size,createfunction,contextheap, stackheap, context2heap, stack2heap,threadheap, messagesallowed,id, device, args, direction) \
	{ \
	  INIT_THREADTEMPLATE_((name),(priority),(stacksize),(stack2size),(createfunction),(contextheap), (stackheap), (context2heap), (stack2heap),(threadheap), (messagesallowed)) \
	, (id), (device), (args), (direction) \
	}

	
#elif defined (__ADSPBLACKFIN__)

#define INIT_THREADTEMPLATE_(name,priority,stacksize,createfunction,stackheap, threadheap, messagesallowed) \
	{ (name), (stackheap), (threadheap), \
	(priority), (stacksize), (createfunction), (messagesallowed) }

#define INIT_RTHREAD_(name,priority,stacksize,createfunction,stackheap, threadheap, messagesallowed,id, device, args, direction) \
	{ \
	INIT_THREADTEMPLATE_((name),(priority),(stacksize),(createfunction),(stackheap), (threadheap), (messagesallowed)) \
	, (id), (device), (args), (direction) \
	}

#else 

#define INIT_THREADTEMPLATE_(name,priority,stacksize,createfunction,contextheap, stackheap,threadheap, messagesallowed) \
	{ (name), (contextheap), (stackheap), (threadheap), \
	(priority),(stacksize),(createfunction),(messagesallowed) }

#define INIT_RTHREAD_(name, priority,stacksize,createfunction,contextheap, stackheap,threadheap, messagesallowed, id, device, args, direction) \
	{ \
	INIT_THREADTEMPLATE_((name), (priority),(stacksize),(createfunction),(contextheap), (stackheap),(threadheap), (messagesallowed)) \
	, (id), (device), (args), (direction) \
	}
#endif
/****************************************
 * SEMAPHORES
****************************************/

    NAMESPACE_(SemaphoreID)
    NAMESPACE_DECL_(CreateSemaphore)( unsigned int inInitialValue , 
								      unsigned int inMaxCount , 
								      NAMESPACE_(Ticks) inInitialDelay ,
								      NAMESPACE_(Ticks) inPeriod );
    void
    NAMESPACE_DECL_(DestroySemaphore)( const NAMESPACE_(SemaphoreID) inSemaphoreID);

    /**
     * This function provides the mechanism by which threads pend on signals.
     * If the semaphore's count is greater than zero, the count is decremented
     * and execution returns to the running thread.  If the semaphore's count
     * is zero, the thread pauses execution until the semaphore is posted.
     * If the thread does not resume execution within "timeout" number of
     * clock ticks, the thread's reentry point is changed to be its error
     * function.  If the value of "timeout" is passed as zero (the default),
     * then the thread may pend indefinitely.
     *
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_PendSemaphore.cpp
     */
    bool
    NAMESPACE_DECL_(PendSemaphore)( NAMESPACE_(SemaphoreID) inSemaphoreID,
                                    NAMESPACE_(Ticks) inTimeout );

    /**
     * Returns the boolean value of the semaphore with the given ID.
 	 *
	 * Implemented in file: VDK_API_PendSemaphore.cpp
     */
    unsigned int
    NAMESPACE_DECL_(GetSemaphoreValue)( NAMESPACE_(SemaphoreID) inSemaphoreID ); 

    /**
     * This function provides the mechanism by which threads post signals.
     * Note that ISRs must use a different interface.
     *
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_PostSemaphore.cpp
     */
    void
    NAMESPACE_DECL_(PostSemaphore)( NAMESPACE_(SemaphoreID) inSemID );

	/**
	 * Causes a semaphore to be posted after delay ticks, and then periodically
	 * every period ticks.
 	 *
	 * Implemented in file: VDK_API_Periodic.cpp
	 */
    void
	NAMESPACE_DECL_(MakePeriodic)( NAMESPACE_(SemaphoreID) inSemID,
	                               NAMESPACE_(Ticks) inDelay,
	                               NAMESPACE_(Ticks) inPeriod );

	/**
	 * Removes an element from the periodic queue
 	 *
	 * Implemented in file: VDK_API_Periodic.cpp
	 */
    void
	NAMESPACE_DECL_(RemovePeriodic)( NAMESPACE_(SemaphoreID) inSemID ); 


	/**
	 * Gets all of the threads that are pending on a given semaphore.
 	 *
	 * Implemented in file: VDK_API_PendSemaphore.cpp
	 */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetSemaphorePendingThreads)( NAMESPACE_(SemaphoreID) inSemID, 
											int *outNumThreads, 
											NAMESPACE_(ThreadID) outThreadArray[],
											int inArraySize);

	/**
	 * Retrieves the period and maxcount for a given semaphore.
 	 *
	 * Implemented in file: VDK_API_PendSemaphore.cpp
	 */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetSemaphoreDetails)( const NAMESPACE_(SemaphoreID) inSemID, 
										  NAMESPACE_(Ticks) *outPeriod, 
										  unsigned int *outMaxCount );

	/**
	 * Retrieves the number of semaphores currently in the system, and 
	 * optionally fills the given array with the SemaphoreIDs
 	 *
	 * Implemented in file: VDK_API_PendSemaphore.cpp
	 */
	int
	NAMESPACE_DECL_(GetAllSemaphores)( NAMESPACE_(SemaphoreID) outSemaphoreIDArray[], 
									   int inArraySize );



/****************************************
 * EVENTBITS
****************************************/

    /**
     * Sets (makes equal to eg. TRUE, 1, etc.) the value of the event bit with the
     * given ID.
     * 
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    void
    NAMESPACE_DECL_(SetEventBit)( NAMESPACE_(EventBitID) inEventBitID );

    /**
     * Clears (makes equal to eg. FALSE, NULL, 0, etc.) the value of the event
     * bit with the given ID.
 	 *
     * This function invokes the scheduler and may result in a context switch.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    void
    NAMESPACE_DECL_(ClearEventBit)( NAMESPACE_(EventBitID) inEventBitID ); 


    /**
     * Returns the value of the event bit with the given ID.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    bool
    NAMESPACE_DECL_(GetEventBitValue)( NAMESPACE_(EventBitID) inEventBitID );


    /**
     * Pends the Thread on an event.  If the event value is false, the thread
     * blocks, and is put into the appropriate event waiting queue
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    bool
    NAMESPACE_DECL_(PendEvent)( NAMESPACE_(EventID) inEventID,
                                const NAMESPACE_(Ticks) inTimeout );

    /**
     * Returns the value of the event bit with the given ID.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    bool
    NAMESPACE_DECL_(GetEventValue)( NAMESPACE_(EventID) inEventID );


    /**
     * Returns the bitfield mask associated with the event flag with the
     * given ID.  Threads may use this function to get the current mask of
     * an event flag, but the logic type (AND or OR) is not returned and
     * cannot be changed its initial value.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    NAMESPACE_(EventData)
    NAMESPACE_DECL_(GetEventData)( NAMESPACE_(EventID) inEventID );

    /**
     * Sets the bitfield mask associated with the event flag with the
     * given ID.  The previous mask value is replaced with the new mask value.
     * The logic type parameter takes values of kAnd or kOr and determines
     * the new logical operand.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */
    void
    NAMESPACE_DECL_(LoadEvent)( NAMESPACE_(EventID) inEventID,
                                const NAMESPACE_(EventData) inEventData );

    /**
     * Gets the thread IDs for all the threads that are pending on the 
	 * given Event.
 	 *
	 * Implemented in file: VDK_API_Events.cpp
     */

	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetEventPendingThreads)( const NAMESPACE_(EventID)inEventID, 
											 int *outNumThreads, 
											 NAMESPACE_(ThreadID) outThreadArray[],
											 int inArraySize );


/****************************************
 * Mutex
****************************************/
    /**
	 *
	 * Creates and initializes a new mutex object. 
	 * 
	 * Implemented in file: VDK_RMutex.cpp
	 */

	NAMESPACE_(MutexID)
	NAMESPACE_DECL_(CreateMutex)(void);

    void
    NAMESPACE_DECL_(DestroyMutex)( const NAMESPACE_(MutexID) inMutexID);

    void
    NAMESPACE_DECL_(AcquireMutex)( const NAMESPACE_(MutexID) inMutexID);

    void
    NAMESPACE_DECL_(ReleaseMutex)( const NAMESPACE_(MutexID) inMutexID);

/****************************************
 * Fifo
****************************************/
    /**
	 * Implemented in file: VDK_Fifo.cpp
	 */

	NAMESPACE_(FifoID)
	NAMESPACE_DECL_(CreateFifo)(unsigned int inSize, 
                                void *pInBuffer, 
                                void (*pfInBufferCleanup)(void*));

    void
    NAMESPACE_DECL_(DestroyFifo)( NAMESPACE_(FifoID) inFifoID);

    bool
    NAMESPACE_DECL_(C_ISR_PostFifo)(NAMESPACE_(FifoID) 	inFifoID, 
                                    void 	*pInData);

    bool
    NAMESPACE_DECL_(PostFifo)( NAMESPACE_(FifoID) 	inFifoID, 
                                void 	*pInData, 
                                NAMESPACE_(Ticks) 	inTimeout );
    bool
    NAMESPACE_DECL_(PendFifo)(NAMESPACE_(FifoID) 	inFifoID, 
                                void 	**pOutData, 
                                NAMESPACE_(Ticks) 	inTimeout );

    NAMESPACE_(SystemError)
    NAMESPACE_DECL_(GetFifoDetails)( NAMESPACE_(FifoID) 	fifo,
                                            unsigned int 	*pOutFifoSize,
                                            unsigned int 	*pOutNumFreeSlots, 
                                            void 		    **pOutBuffer);

/****************************************
 * History
****************************************/

	/**
	 * Logs an event in the history buffer.
	 *
	 * This function reduces to a null call if the macro 
	 * VDK_INSTRUMENTATION_LEVEL_ not set to 2.
	 *
	 * Implemented in file: VDK_API_History.cpp
	 */
	void
	NAMESPACE_DECL_(LogHistoryEvent)(NAMESPACE_(HistoryEnum) inEnum, 
															int inValue);
	
	/**
	* History data is only logged when using Full Instrementated VDK libraries. 
	* Using conditional compilation to produce a compile time error for 
	* customers trying to call these APIs when using error checking and non-error 
	* checking libraries.
	*/
#if defined (VDK_INSTRUMENTATION_LEVEL_) && (VDK_INSTRUMENTATION_LEVEL_==2)

	/**
    * Events that can be registered in a history buffer.
	* 
	* Extracted from VDK_History.h
    */
	typedef struct NAMESPACE_DECL_(HistoryEvent) 
    {
		NAMESPACE_(Ticks)      	time;
		NAMESPACE_(ThreadID)   	threadID;
        NAMESPACE_(HistoryEnum) type;
        int             		value;
    } NAMESPACE_DECL_(HistoryEvent);


	typedef void (*NAMESPACE_DECL_(HistoryLogging))( NAMESPACE_(Ticks), 
											const NAMESPACE_(HistoryEnum), 
											const int, 
											const NAMESPACE_(ThreadID) );

    /**
     * Retrieves the history event stored in index inHistVentNum of the history
	 * buffer. 
 	 *
	 * Implemented in file: VDK_API_History.cpp
     */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetHistoryEvent)( unsigned int inHistEventNum, 
									  NAMESPACE_(HistoryEvent) *outHistoryEvent );

	 /**
     * Returns the current position in the history buffer. Note that
	 * this is the position that is about to be written, not the position
	 * of the most recent write.
 	 *
	 * Implemented in file: VDK_API_History.cpp
     */
	unsigned int
	NAMESPACE_DECL_(GetCurrentHistoryEventNum)( void );

	/**
     * Returns the size of the history buffer. 
 	 *
	 * Implemented in file: VDK_API_History.cpp
     */
	unsigned int
	NAMESPACE_DECL_(GetHistoryBufferSize)( void );

	/**
     * Replaces the default logging subroutine with one that is user defined. 
 	 *
	 * Implemented in file: VDK_API_History.cpp
     */
	void 
	NAMESPACE_DECL_(ReplaceHistorySubroutine)( NAMESPACE_(HistoryLogging) newRoutine);

#endif /* VDK_INSTRUMENTATION_LEVEL_ */


/****************************************
 * Error Handling
****************************************/

    /**
     * Used in the error handler of threads written in C, it goes to KernelPanic
     *
     **/

#ifdef __cplusplus
    extern "C" void VDK_CThread_Error(VDK::ThreadID inThreadID);
#else
    void VDK_CThread_Error(VDK_ThreadID inThreadID);
#endif

    /**
     * Dispatches an error to the calling thread
 	 *
	 * Implemented in file: VDK_API_Error.cpp
     */
    int
    NAMESPACE_DECL_(DispatchThreadError)( NAMESPACE_(SystemError) inErrorID, const int inErrorValue );

    /**
     * Sets the error enumeration and value for a thread
 	 *
	 * Implemented in file: VDK_API_Error.cpp
     */
    void
    NAMESPACE_DECL_(SetThreadError)( NAMESPACE_(SystemError) inErrorID, const int inErrorValue );


    /**
     * Clears the error enumeration and value for a thread
 	 *
	 * Implemented in file: VDK_API_Error.cpp
     */
    void
    NAMESPACE_DECL_(ClearThreadError)( void );


    /**
     * Returns the most recent error enumeration for a thread
 	 *
	 * Implemented in file: VDK_API_Error.cpp
     */
    NAMESPACE_(SystemError)
    NAMESPACE_DECL_(GetLastThreadError)( void );


    /**
     * Returns the most recent error value for a thread
 	 *
	 * Implemented in file: VDK_API_Error.cpp
     */
    int
    NAMESPACE_DECL_(GetLastThreadErrorValue)( void );

/*************************
 * IMASK stuff
 *************************/

   /**
    * Gets the IMASK values
    */
   NAMESPACE_(IMASKStruct)
   NAMESPACE_DECL_(GetInterruptMask)( void );

#if defined(__2116x__) || defined(__2126x__) || defined(__2136x__) || defined(__2137x__) || defined (__214xx__)
  void
  NAMESPACE_DECL_(GetInterruptMaskEx) (NAMESPACE_(IMASKStruct)* outValue1, NAMESPACE_(IMASKStruct)* outValue2);
#endif

   /**
    * Set some IMASK bits
    */
   void
   NAMESPACE_DECL_(SetInterruptMaskBits)( const NAMESPACE_(IMASKStruct) inValue );

#if defined(__2116x__) || defined(__2126x__) || defined(__2136x__) || defined(__2137x__) || defined (__214xx__)
   void
   NAMESPACE_DECL_(SetInterruptMaskBitsEx)( const NAMESPACE_(IMASKStruct) inValue1, const NAMESPACE_(IMASKStruct) inValue2 );
#endif

   /**
    * Clear some IMASK bits
    */
   void
   NAMESPACE_DECL_(ClearInterruptMaskBits)( const NAMESPACE_(IMASKStruct) inValue );

#if defined(__2116x__) || defined(__2126x__) || defined(__2136x__) || defined(__2137x__) || defined (__214xx__)
   void
   NAMESPACE_DECL_(ClearInterruptMaskBitsEx)( const NAMESPACE_(IMASKStruct) inValue1, const NAMESPACE_(IMASKStruct) inValue2 );

#endif

   /**
	*  Create Memory Pool 
	*/
	NAMESPACE_(PoolID)  
	NAMESPACE_DECL_(CreatePool) (const unsigned int inBlockSz,const unsigned int inNumBlocks,const bool inLazyCreate);

   /**
	*  Create Memory Pool Ex
	*/
	NAMESPACE_(PoolID)  
	NAMESPACE_DECL_(CreatePoolEx) (const unsigned int inBlockSz,const unsigned int inNumBlocks,const bool inLazyCreate,int inWhichPool);

	/**
	 *  Destroy Memory Pool
	 */
	void
    NAMESPACE_DECL_(DestroyPool) (const NAMESPACE_(PoolID) inPoolID);	

	/** 
	 * Allocate Block
	 */
	void *
	NAMESPACE_DECL_(MallocBlock) (const NAMESPACE_(PoolID) inPoolID);
	
	/**
	 * Free Block
	 */
	void
	NAMESPACE_DECL_(FreeBlock) (const NAMESPACE_(PoolID) inPoolID, void* inBlkPtr);

	/**
	 *  Locate block and free
	 */
	void
	NAMESPACE_DECL_(LocateAndFreeBlock) (void *inBlkPtr);

	/** 
	 *  Get the number of free blocks for a specified pool
	 */
	unsigned int
	NAMESPACE_DECL_(GetNumFreeBlocks) (const NAMESPACE_(PoolID) inPoolID);
	
	/** 
	 *  Get the number of allocated blocks in a specified pool
	 */
	unsigned int
	NAMESPACE_DECL_(GetNumAllocatedBlocks) (const NAMESPACE_(PoolID) inPoolID);

	/** 
	 * Retrieves the total number of blocks in a memory pool, and the starting
	 * address of the pool in memory.
	 *
	 * Implemented in file: VDK_API_PoolMgr.cpp
	 */
	NAMESPACE_(SystemError)
	NAMESPACE_DECL_(GetPoolDetails)( const NAMESPACE_(PoolID) inPoolID,	
									 unsigned int *outNumBlocks, 
									 void **outMemoryAddress );

	/** 
	 * Retrieves the number of pools currently in the system, and 
	 * optionally fills the given array with the PoolIDs
	 *
	 * Implemented in file: VDK_API_PoolMgr.cpp
	 */
	int
	NAMESPACE_DECL_(GetAllMemoryPools)( NAMESPACE_(PoolID) outPoolIDArray[], 
										int inArraySize );

	/** 
	 *  Get the configured block size for the specified pool
	 */
	unsigned int
	NAMESPACE_DECL_(GetBlockSize) (const NAMESPACE_(PoolID) inPoolID);

	unsigned int 
	NAMESPACE_DECL_(GetThreadStackUsage) (const NAMESPACE_(ThreadID) inThreadID);

#ifdef __ADSPTS__
	unsigned int 
	NAMESPACE_DECL_(GetThreadStack2Usage) (const NAMESPACE_(ThreadID) inThreadID);
#endif

	void
	NAMESPACE_DECL_(InstrumentStack) (void);

	/*
	 * Thread-Local Storage API
	 */
#ifdef __cplusplus
	static const int kTLSUnallocated = INT_MIN;
#else
	#define VDK_kTLSUnallocated INT_MIN
#endif

	bool
	NAMESPACE_DECL_(AllocateThreadSlot) (int *ioSlotNum);

	bool
	NAMESPACE_DECL_(AllocateThreadSlotEx) (int *ioSlotNum, void (*pfCleanup)(void *));

	bool
	NAMESPACE_DECL_(SetThreadSlotValue) (int inSlotNum, void *inValue);

	void*
	NAMESPACE_DECL_(GetThreadSlotValue) (int inSlotNum);

	bool
	NAMESPACE_DECL_(FreeThreadSlot) (int inSlotNum);

#ifdef __ADSPBLACKFIN__
#define ___vdkclobber__ "P1 R0"
#define ___vdkclobber_call__ "P1 R0"
#elif defined(__ADSP21000__)
#define ___vdkclobber__ ""
#define ___vdkclobber_call__ "i12"
#else /* TS */
#define ___vdkclobber__ ""
#endif


#if defined(__ADSPBLACKFIN__) || defined(__ADSP21000__)

#pragma regs_clobbered ___vdkclobber_call__
	bool
	NAMESPACE_DECL_(IsInRegion)(void);

#endif

	bool
	NAMESPACE_DECL_(IsRunning)(void);

#pragma regs_clobbered ___vdkclobber__
	void
	NAMESPACE_DECL_(C_ISR_ActivateDevice)(NAMESPACE_(IOID) inID);

#pragma regs_clobbered ___vdkclobber__
	void
	NAMESPACE_DECL_(C_ISR_PostSemaphore)( NAMESPACE_(SemaphoreID) inSemID );

#pragma regs_clobbered ___vdkclobber__
	void
	NAMESPACE_DECL_(C_ISR_SetEventBit)( NAMESPACE_(EventBitID) inEventBitID );

#pragma regs_clobbered ___vdkclobber__
	void
	NAMESPACE_DECL_(C_ISR_ClearEventBit)( NAMESPACE_(EventBitID) inEventBitID );

#pragma regs_clobbered ___vdkclobber__
#ifdef __cplusplus
extern "C" 
#endif 
    void 
    KernelPanic(NAMESPACE_DECL_(PanicCode) inCode, 
                NAMESPACE_DECL_(SystemError) inError, 
                const int inValue);

#if defined(__ADSPBLACKFIN__) 
#pragma regs_clobbered ___vdkclobber__
#ifdef __cplusplus
extern "C" 
#endif 
    void 
    ExceptionPanic(NAMESPACE_DECL_(PanicCode) inCode, 
                NAMESPACE_DECL_(SystemError) inError, 
                const int inValue);

#ifdef __cplusplus
extern "C" 
#endif 
    void 
    InterruptPanic(NAMESPACE_DECL_(PanicCode) inCode, 
                NAMESPACE_DECL_(SystemError) inError, 
                const int inValue);
#endif

/*
 * Definitions of max variables that are only required in C++  because we have
 * macros in C. The variables are defined in VDK.cpp */

#ifdef __cplusplus
	extern const unsigned int NAMESPACE_DECL_(kMaxNumThreads);
	extern const unsigned int NAMESPACE_DECL_(kMaxNumActiveMemoryPools);
	extern const unsigned int NAMESPACE_DECL_(kMaxNumActiveSemaphores);
	extern const unsigned int NAMESPACE_DECL_(kMaxNumActiveDevFlags);
	extern const unsigned int NAMESPACE_DECL_(kMaxNumActiveMessages);
	extern const unsigned int NAMESPACE_DECL_(kMaxNumIOObjects);
#endif /* __cplusplus */



#ifdef __cplusplus
}	// namespace VDK
#endif /* __cplusplus */



/* Get rid of the namespacing macros */
#undef NAMESPACE_
#undef NAMESPACE_DECL_

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif //VDK_API_H_



