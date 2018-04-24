/* =============================================================================
 *
 *  Description: This VDK Configuration file is automatically generated 
 *               by the VisualDSP++ IDDE and should not be modified.
 *
 *  Generated by VDKGen version %VDKGenVersion%
 * ===========================================================================*/

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"
#include <VDK_Internals.h>
#pragma diag(pop)

#ifndef NULL
#define NULL 0
#endif


%IncludeThreadHeaderFiles%

%IncludeDeviceDriverHeaderFiles%

#pragma file_attr("Startup")

%UserDefinedISRPrototypes%

namespace VDK
{

    /*************************************************************************
     *    Thread Types:
     */

    VDK::IDTableElement g_ThreadIDElements[%MaxNumThreads%+1];
    VDK::ThreadTable g_ThreadTable = {g_ThreadIDElements, %MaxNumThreads%+1};
    

    VDK::ThreadTemplate    g_ThreadTemplates[] = 
    {
%ThreadTemplateConstructors%
    };

     // Number of thread templates
    unsigned int kNumThreadTemplates = sizeof(g_ThreadTemplates)/sizeof(ThreadTemplate);

#ifdef VDK_INCLUDE_IO_
    /***************************************************************************
     * IO Templates: 
     */

HeapID g_IOObjectHeap = %IOObjectsHeap%;

    VDK::IDTableElement g_DevIDElements[%MaxNumIOObjects%];

    VDK::IODeviceTable g_IOdevTable = {g_DevIDElements, %MaxNumIOObjects%};

    extern const unsigned int kMaxNumIOObjects=%MaxNumIOObjects%;
    VDK::IODeviceTemplate g_IOTemplates[] = 
    {
%IODeviceTemplates%
    };
    unsigned int kNumIOTemplates = sizeof(g_IOTemplates)/sizeof(IODeviceTemplate);

#ifdef VDK_BOOT_OBJECTS_IO_

    BootIOObjectInfo g_BootIOObjects[] = 
    {
%BootIOTemplates%
    };
    unsigned int kNumBootIOObjects = sizeof(g_BootIOObjects)/sizeof(BootIOObjectInfo);

#endif // VDK_BOOT_OBJECTS_IO_
#endif // VDK_INCLUDE_IO_


    /*************************************************************************
     *    Idle Thread:
     */
    VDK::ThreadTemplate g_IdleThreadTplate = %IdleThreadTemplate%;

    /*************************************************************************

     *    Boot Threads:
     */


    // The maximum number of running threads in a system
    extern const unsigned int kMaxNumThreads = %MaxNumThreads%;

    // The list of threads to create at boot time
     BootThreadInfo g_BootThreadInfo[] =     
    { 
%BootThreadIDs%
    }; 

    unsigned int kNumBootThreads = sizeof( g_BootThreadInfo ) / sizeof( BootThreadInfo );

    /**************************************************************************
     * Round Robin (time-slice) mode: 
     * 
     * Defines which priority levels will execute in round robin (time sliced) mode.
     */
    
    unsigned int g_RoundRobinInitPriority = (
%RoundRobinPriorities%
        0 );

    // If the priority is in round robin mode, set the period in Ticks.
    // For all priorities not in RR mode, the period MUST be set to 1.
    
    VDK::Ticks  g_RoundRobinInitPeriod[] =         
    {
        1,            // The Idle thread priority
%RoundRobinPeriods%
    };

#ifdef VDK_INCLUDE_MEMORYPOOLS_ 
    /*************************************************************************
     *    Memory Pools:
     */                    
    extern const unsigned int kMaxNumActiveMemoryPools=%MaxNumActiveMemoryPools%;

    VDK::IDTableElement g_MemIDElements[%MaxNumActiveMemoryPools%];

    VDK::MemoryPoolTable g_MemoryPoolTable = {g_MemIDElements, %MaxNumActiveMemoryPools%};

#ifdef VDK_BOOT_MEMORYPOOLS_

    struct BootPoolInfo g_BootMemoryPools[] =
    { 
%MemoryPools% 
    };

    unsigned int kNumBootPools = sizeof (g_BootMemoryPools)/sizeof(BootPoolInfo); 

#ifdef VDK_INCLUDE_MESSAGES_
    VDK::PoolID g_MessagePoolID;
#endif // VDK_INCLUDE_MESSAGES

#else

    unsigned int kNumBootPools = 0;    // Number of boot pools

#endif //VDK_BOOT_MEMORYPOOLS_
#endif // VDK_INCLUDE_MEMORYPOOLS_


    float g_TickPeriod = %TickPeriod%;

    float g_ClockFrequency = %ClockFrequency%;

    unsigned int g_ClockPeriod = %ClockPeriod%;

    unsigned int g_StackAlignment   = VDK_STACK_ALIGNMENT_ ;
#if VDK_INSTRUMENTATION_LEVEL_==2
    // Setup the history buffer
    HistoryStruct       g_HistoryEnums[%HistoryBufferSize%];
    HistoryBuffer        g_History = {%HistoryBufferSize%, 0, g_HistoryEnums};
#endif

    /*************************************************************************
     * Semaphores:
     */
    
    extern const unsigned int kMaxNumActiveSemaphores=%MaxNumActiveSemaphores%;
    VDK::IDTableElement g_SemIDElements[%MaxNumActiveSemaphores% + 1];
    SemaphoreTable g_SemaphoreTable = {g_SemIDElements, %MaxNumActiveSemaphores%};

#ifdef VDK_INCLUDE_SEMAPHORES_
    HeapID g_SemaphoreHeap = %SemaphoresHeap%; 


#ifdef VDK_BOOT_SEMAPHORES_
    SemaphoreInfo  g_BootSemaphores[] = 
    {
%BootSemaphores%
    };
    unsigned int kNumBootSemaphores = sizeof (g_BootSemaphores)/sizeof(SemaphoreInfo);    // Number of boot semaphores

#endif // VDK_BOOT_SEMAPHORES_
#endif // VDK_INCLUDE_SEMAPHORES_

#ifdef VDK_MULTIPLE_HEAPS_
    HeapInfo g_Heaps[] =
    {
%HeapInit%
    };

    unsigned int kNumHeaps = sizeof (g_Heaps)/sizeof(HeapInfo);    // Number of heaps
#endif

#ifdef VDK_INCLUDE_EVENTS_

    /*************************************************************************
     * EventBits: 
     */
    
    // Initialize the global system EventBit state.  This is a bitfield, so the
    // bit value needs to be shifted into position and OR'd to the total. 
    
    volatile unsigned int      g_EventBitState = (
%InitialEventBitStates%
        0 );
    
    // Allocate memory for EventBits using the same value as kNumEventBits.  
    
    unsigned int kNumEventBits = %NumEventBits%;    //  total number of entries in enum 
    VDK::EventBit g_EventBits[%NumEventBits%];

    /*************************************************************************
     * Events: 
     */
    
    VDK::Event g_Events[] =        
    {
%EventConstructors%
    };

    unsigned int kNumEvents = sizeof (g_Events)/sizeof(Event);

#endif // ifdef VDK_INCLUDE_EVENTS_

    /*************************************************************************
     * Device Flags: 
     */

#ifdef VDK_INCLUDE_DEVICE_FLAGS_
HeapID g_DeviceFlagHeap = %DeviceFlagsHeap%;

    extern const unsigned int kMaxNumActiveDevFlags=%MaxNumActiveDevFlags%;
    VDK::IDTableElement g_DevFlagsIDElements[%MaxNumActiveDevFlags%];
    VDK::DeviceFlagTable     g_DeviceFlagTable = {g_DevFlagsIDElements, %MaxNumActiveDevFlags%};

#ifdef VDK_BOOT_DEVICE_FLAGS_
    unsigned int kNumBootDeviceFlags = %NumBootDevFlags%;

#endif // VDK_BOOT_DEVICE_FLAGS_

#endif // VDK_INCLUDE_DEVICE_FLAGS_


    /*************************************************************************
     * Messages: 
     */

#ifdef VDK_INCLUDE_MESSAGES_
    extern const unsigned int kMaxNumActiveMessages=%MaxNumActiveMessages%;

    void MessageQueueCleanup(MessageQueue*);

    void (*g_MessageQueueCleanup)(VDK::MessageQueue *) = MessageQueueCleanup;
#else
    void (*g_MessageQueueCleanup)(VDK::MessageQueue *) = NULL;

#endif

    unsigned int g_localNode =%LocalProcessorID%;

    unsigned short g_ChannelRoutingMask =%ChannelRoutingMask%;


    ThreadID g_vRoutingThreads[]=
    {
%RoutingTableConstructor%
    };

    ThreadID g_vRoutingThreads2[]=
    {
%RoutingTable2Constructor%
    };

    unsigned int kNumRoutingNodes = %NumRoutingNodes%;    // Number of routing nodes

    MsgThreadEntry g_RoutingThreadInfo[] =
    {
%RoutingThreadConstructor%
    };

    unsigned int kNumRoutingThreads = %NumRoutingThreads%; // Number of routing threads

%CustomMarshalledIncludes%

    VDK::MarshallingEntry g_vMarshallingTable[] =
    {
%MarshalledMessagesConstructor%
    };

    unsigned int kNumMarshalledTypes = %NumMarshalledTypes% ;    // Number of marshalling message types



    /*************************************************************************
     * Interrupt Service Routines
     */

    IMASKStruct g_InitialISRMask = 
    {
        %TimerInterrupt% 
%InstantiatedInterrupts%
        %StackOverflowInterrupt%
        %RescheduleInterrupt% 
    };

#ifdef __ADSP21000__
// this variable is only used in SHARC processors that have some interrupts in
// LIRPTL
    IMASKStruct g_InitialISRMask2 =
    {
%InstantiatedInterrupts2%
    0
    };
#endif

    void UserDefinedInterruptServiceRoutines( void )
    {
%TimerInterruptRoutine%
%DefinedInterrupts%
    }

    BootFuncPointers g_InitBootFunctionP[] = 
    { 
%BootFunctionPointers% 
    }; 

%TimerFuncPtr%
    unsigned int kNumBootEntries = sizeof (g_InitBootFunctionP)/sizeof(BootFuncPointers) ; 



}  // namespace VDK

/* ========================================================================== */
