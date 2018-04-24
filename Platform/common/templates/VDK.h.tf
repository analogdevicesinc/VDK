/* =============================================================================
 *
 *  Description: This VDK Configuration file is automatically generated
 *               by the VisualDSP++ IDDE and should not be modified.
 *
 *  Generated by VDKGen version %VDKGenVersion%
 * ===========================================================================*/

#ifndef VDK_H_    
#define VDK_H_

#ifdef __ECC__    
#pragma no_implicit_inclusion

#ifndef _ADI_THREADS
#warning "VDK projects should be compiled with -threads option"
#endif

#endif

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress: misra_rule_19_1:"VDK needs the enum type definitions for the particular project before including VDK_Public.h")
#endif

/* Define which optional VDK features are to be used in this build. */

/**
 * Semaphores are used in this project 
 */
 
%SemaphoresUsed%#define VDK_INCLUDE_SEMAPHORES_ %SemaphoresUsedEnd%
%BootSemaphoresUsed%#define VDK_BOOT_SEMAPHORES_ %BootSemaphoresUsedEnd%

/**
 * MemoryPools are used in this project 
 */
 
%MemoryPoolsUsed%#define VDK_INCLUDE_MEMORYPOOLS_ %MemoryPoolsUsedEnd%
%BootMemoryPoolsUsed%#define VDK_BOOT_MEMORYPOOLS_ %BootMemoryPoolsUsedEnd%

/**
 * DeviceFlags are used in this project 
 */
 
%DevFlagsUsed%#define VDK_INCLUDE_DEVICE_FLAGS_ %DevFlagsUsedEnd%
%BootDevFlagsUsed%#define VDK_BOOT_DEVICE_FLAGS_ %BootDevFlagsUsedEnd%

/**
 * IO templates are used in this project
 */
 
%IOTemplatesUsed%#define VDK_INCLUDE_IO_ %IOTemplatesUsedEnd%
%BootIOObjectsUsed%#define VDK_BOOT_OBJECTS_IO_ %BootIOObjectsUsedEnd%

/* 
 * There are heaps specified 
 */

%MultipleHeapsUsed%#define VDK_MULTIPLE_HEAPS_ %MultipleHeapsUsedEnd%

/**
 * Messages are used in this project
 */

%MessagesUsed%#define VDK_INCLUDE_MESSAGES_ %MessagesUsedEnd%

/**
 * Multiprocessor messaging is used in this project
 */

%MultiprocessorMessagingUsed%#define VDK_MULTIPROCESSOR_MESSAGING_ %MultiprocessorMessagingUsedEnd%

/**
 * RoutingThreads are defined in this project
 */

%RoutingThreadsUsed%#define VDK_INCLUDE_ROUTING_THREADS_ %RoutingThreadsUsedEnd%

/**
 * Routing Nodes are defined in this project
 */

%RoutingNodesUsed%#define VDK_INCLUDE_ROUTING_NODES_ %RoutingNodesUsedEnd%

/**
 * Marshalled Message Types are defined in this project
 */

%MarshalledMessagesUsed%#define VDK_INCLUDE_MARSHALLED_MESSAGES_ %MarshalledMessagesUsedEnd%

/**
 * Events and EventBits are used in this project 
 */
 
%EventsUsed%#define VDK_INCLUDE_EVENTS_ %EventsUsedEnd%

/**
 * Enhanced API error checking and runtime Instrumentation is included
 */
 
#define VDK_INSTRUMENTATION_LEVEL_ %InstrumentationLevel%

#ifndef VDK_LDF_

#include <def%TargetProcessor%.h>

/**
 * ALL of the following enums are generated in format suitable for C/C++
 * and a format suitable for assembly inclusion
 */

/*************************************************************************
 * ThreadType
 *
 * These are used in calls to VDK::CreateThread().
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum ThreadType
{
kDynamicThreadType = -1,
%C_ThreadTypeEnumList%
};
#else    /* for asm access */
#define kDynamicThreadType  -1
%ASM_ThreadTypeEnumList%
#endif     /* __ECC__ */

/*************************************************************************
 * ThreadID
 *
 * These are used in calls to VDK::CreateThread().
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum ThreadID
{
%C_ThreadIDEnumList%
%C_RoutThreadEnumList%
};
#else    /* for asm access */
%ASM_ThreadIDEnumList%
%ASM_RoutThreadEnumList%
#endif     /* __ECC__ */


/*************************************************************************
 * SemaphoreID
 *
 * Used in all semaphore APIs
 *
 */

#ifdef __ECC__    /* for C/C++ access */
enum SemaphoreID 
{
%C_SemaphoreEnumList%
    last_semaphore__3VDK=-1
};
#else    /* for asm access */
%ASM_SemaphoreEnumList%
#endif     /* __ECC__ */


/*************************************************************************
 * MemoryPoolID
 *
 * Used in all Memory Pool APIs
 *
 */

#ifdef __ECC__        /* for C/C++ access */
enum PoolID
{
%C_MemoryPoolEnumList%
      last_mempool__3VDK=-1
};
#else /* for asm access */
%ASM_MemoryPoolEnumList%
#endif        /* __ECC__ */


/*************************************************************************
 * EventID
 *
 * Used in Event(...) APIs.
 *
 * There cannot be more than NATURAL_WORD_SIZE
 * EventIDs.
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum EventID
{
%C_EventEnumList%
    last_event__3VDK=-1
};
#else    /* for asm access */
%ASM_EventEnumList%
#endif     /* __ECC__ */


/*************************************************************************
 * EventBitID
 *
 * Used in EventBit(...) APIs
 *
 * There cannot be more than NATURAL_WORD_SIZE
 * EventBitIDs.
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum EventBitID
{
%C_EventBitEnumList%
    last_eventbit__3VDK=-1
};
#else    /* for asm access */
%ASM_EventBitEnumList%
#endif     /* __ECC__ */

/*************************************************************************
 * DeviceID
 *
 * Used in Device Drivers
 *
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum IOID
{
%C_BootIODeviceList%
    last_bootdevice__3VDK=-1
};
#else    /* for asm access */
%ASM_BootIODeviceList%
#endif     /* __ECC__ */


/*************************************************************************
 * IO template IDs
 *
 * Used in Device Drivers
 *
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum IOTemplateID
{
%C_IOTemplateEnumList%
    last_iotemplate__3VDK=-1
};
#else    /* for asm access */
%ASM_IOTemplateEnumList%
#endif     /* __ECC__ */

/*************************************************************************
 * DeviceFlagID
 *
 * Used in Device Drivers
 *
 */
 
#ifdef __ECC__    /* for C/C++ access */
enum DeviceFlagID
{
%C_DeviceFlagEnumList%
    last_deviceflag__3VDK=-1
};
#else    /* for asm access */
%ASM_DeviceFlagEnumList%
#endif     /* __ECC__ */


/************************************************************************** 
* Messages are dynamic entities but we create a type to be able to do type
* checking in APIs 
*
*/

#ifdef __ECC__  /* for C/C++ access */
enum MessageID
{
        last_message__3VDK=-1
};
#endif  /* __ECC__ */


/************************************************************************** 
* Mutexes are dynamic entities but we create a type to be able to do type
* checking in APIs 
*
*/
#ifdef __ECC__  /* for C/C++ access */
enum MutexID
{
        last_mutex__3VDK=-1
};
#endif  /* __ECC__ */

/************************************************************************** 
* Fifos are currently dynamic entities but we create a type to be able to 
* do type checking in APIs 
*
*/
#ifdef __ECC__  /* for C/C++ access */
enum FifoID
{
        last_fifo__3VDK=-1
};
#endif  /* __ECC__ */

#ifdef VDK_MULTIPLE_HEAPS_
#ifdef __ECC__  /* for C/C++ access */
enum HeapID {
%C_HeapEnumList%
};
#else /* for asm access */
%ASM_HeapEnumList%
#endif /* __ECC__ */
#endif

#ifdef __ECC__  /* for C/C++ access */
enum RoutingNodeID {
%C_RoutTableEnumList%
};
#else /* for asm access */
%ASM_RoutTableEnumList%
#endif /* __ECC__ */

#ifdef __ECC__  /* for C/C++ access */
enum MarshalledTypeID {
%C_MarshalledTypeEnumList%
};
#else /* for asm access */
%ASM_MarshalledTypeEnumList%
#endif /* __ECC__ */

/* include the VisualDSP Kernel API and library */
#include <VDK_Public.h>

#define VDK_kMaxNumThreads %MaxNumThreads%
#define VDK_kMaxNumActiveSemaphores %MaxNumActiveSemaphores%
#define VDK_kMaxNumActiveMemoryPools %MaxNumActiveMemoryPools%
#define VDK_kMaxNumIOObjects %MaxNumIOObjects%
#define VDK_kMaxNumActiveMessages %MaxNumActiveMessages%
#define VDK_kMaxNumActiveDevFlags %MaxNumActiveDevFlags%

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* VDK_LDF_ */
#endif /* VDK_H_ */

/* ========================================================================== */
