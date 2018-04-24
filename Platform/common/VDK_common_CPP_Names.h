/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/**
 *  Aliases for C++ mangled names for access from C/asm
 *
 */

#ifndef __cplusplus

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress: misra_rule_5_1:"VDK Public APIs have mangled names which may be longer than 31 characters.")
#endif


    /* Interrupt handling */

#ifdef __ECC__
#define  VDK_PushCriticalRegion               (PushCriticalRegion__3VDKFv)
#else
#define _VDK_PushCriticalRegion               _PushCriticalRegion__3VDKFv
#endif

#ifdef __ECC__
#define  VDK_PopCriticalRegion                (PopCriticalRegion__3VDKFv)
#else
#define _VDK_PopCriticalRegion                _PopCriticalRegion__3VDKFv
#endif

#ifdef __ECC__
#define  VDK_PopNestedCriticalRegions         (PopNestedCriticalRegions__3VDKFv)
#else
#define _VDK_PopNestedCriticalRegions         _PopNestedCriticalRegions__3VDKFv
#endif


    /* Scheduler Management */

#ifdef __ECC__
#define VDK_PushUnscheduledRegion             (PushUnscheduledRegion__3VDKFv)
#else
#define _VDK_PushUnscheduledRegion            _PushUnscheduledRegion__3VDKFv
#endif

#ifdef __ECC__
#define VDK_PopUnscheduledRegion              (PopUnscheduledRegion__3VDKFv)
#else
#define _VDK_PopUnscheduledRegion             _PopUnscheduledRegion__3VDKFv
#endif

#ifdef __ECC__
#define VDK_PopNestedUnscheduledRegions       (PopNestedUnscheduledRegions__3VDKFv)
#else
#define _VDK_PopNestedUnscheduledRegions      _PopNestedUnscheduledRegions__3VDKFv
#endif


    /* Thread and System information */

#ifdef __ECC__
#define VDK_GetUptime                         (GetUptime__3VDKFv)
#else
#define _VDK_GetUptime                        _GetUptime__3VDKFv
#endif

#ifdef __ECC__
#define VDK_GetThreadID                       (GetThreadID__3VDKFv)
#else
#define _VDK_GetThreadID                      _GetThreadID__3VDKFv
#endif

#ifndef __ADSPTS__
#ifdef __ECC__
#define VDK_GetContextRecordSize                       (GetContextRecordSize__3VDKFv)
#else
#define _VDK_GetContextRecordSize                      _GetContextRecordSize__3VDKFv
#endif
#endif

#ifdef __ECC__
#define VDK_GetThreadStatus                   (GetThreadStatus__3VDKF8ThreadID)
#else
#define _VDK_GetThreadStatus                  _GetThreadStatus__3VDKF8ThreadID
#endif

#ifdef __ECC__
#define VDK_GetThreadType                     (GetThreadType__3VDKF8ThreadID)
#else
#define _VDK_GetThreadType                    _GetThreadType__3VDKF8ThreadID
#endif

#ifdef __ECC__
#define VDK_GetThreadHandle                   (GetThreadHandle__3VDKFv)
#else
#define _VDK_GetThreadHandle                  _GetThreadHandle__3VDKFv
#endif

#ifdef __ECC__
#define VDK_GetAllThreads                     (GetAllThreads__3VDKFP8ThreadIDi)
#else
#define _VDK_GetAllThreads                    _GetAllThreads__3VDKFP8ThreadIDi
#endif

#ifdef __ECC__
#define VDK_GetThreadBlockingID               (GetThreadBlockingID__3VDKF8ThreadIDPi)
#else
#define _VDK_GetThreadBlockingID              _GetThreadBlockingID__3VDKF8ThreadIDPi
#endif

#ifdef __ECC__
#define VDK_GetThreadTemplateName             (GetThreadTemplateName__3VDKF8ThreadIDPPc)
#else
#define _VDK_GetThreadTemplateName            _GetThreadTemplateName__3VDKF8ThreadIDPPc
#endif

#ifdef __ECC__
#define VDK_GetThreadStackDetails             (GetThreadStackDetails__3VDKF8ThreadIDPUiPPv)
#else
#define _VDK_GetThreadStackDetails            _GetThreadStackDetails__3VDKF8ThreadIDPUiPPv
#endif

#ifdef __ECC__
#define VDK_GetNumTimesRun                    (GetNumTimesRun__3VDKF8ThreadIDPUi)
#else
#define _VDK_GetNumTimesRun                   _GetNumTimesRun__3VDKF8ThreadIDPUi
#endif

#ifdef __ECC__
#define VDK_GetThreadTickData                 (GetThreadTickData__3VDKF8ThreadIDPUiN32)
#else
#define _VDK_GetThreadTickData                _GetThreadTickData__3VDKF8ThreadIDPUiN32
#endif

#ifdef __ECC__
#define VDK_GetVersion                        (GetVersion__3VDKFv)
#else
#define _VDK_GetVersion                       _GetVersion__3VDKFv
#endif
                                                                                                        
    /* Thread Creation/Destruction */                                                                    
                                                                                                        
#ifdef __ECC__
#define VDK_CreateThread                      (CreateThread__3VDKF10ThreadType)
#else
#define _VDK_CreateThread                     _CreateThread__3VDKF10ThreadType
#endif

#ifdef __ECC__
#define VDK_CreateThreadEx                    (CreateThreadEx__3VDKFPQ2_3VDK19ThreadCreationBlock)
#else
#define _VDK_CreateThreadEx                   _CreateThreadEx__3VDKFPQ2_3VDK19ThreadCreationBlock
#endif

#ifndef __ADSPTS__
#ifdef __ECC__
#define VDK_CreateThreadEx2                    (CreateThreadEx2__3VDKFPQ2_3VDK19ThreadCreationBlocki)
#else
#define _VDK_CreateThreadEx2                   _CreateThreadEx2__3VDKFPQ2_3VDK19ThreadCreationBlocki
#endif
#endif

#ifdef __ECC__
#define VDK_DestroyThread                     (DestroyThread__3VDKF8ThreadIDb)
#else
#define _VDK_DestroyThread                    _DestroyThread__3VDKF8ThreadIDb
#endif

#ifdef __ECC__
#define VDK_FreeDestroyedThreads              (FreeDestroyedThreads__3VDKFv)
#else
#define _VDK_FreeDestroyedThreads             _FreeDestroyedThreads__3VDKFv
#endif


    /* Thread Error Management */

#ifdef __ECC__
#define VDK_DispatchThreadError               (DispatchThreadError__3VDKFQ2_3VDK11SystemErrori)
#else
#define _VDK_DispatchThreadError              _DispatchThreadError__3VDKFQ2_3VDK11SystemErrori
#endif

#ifdef __ECC__
#define VDK_SetThreadError                    (SetThreadError__3VDKFQ2_3VDK11SystemErrori)
#else
#define _VDK_SetThreadError                   _SetThreadError__3VDKFQ2_3VDK11SystemErrori                          
#endif

#ifdef __ECC__
#define VDK_ClearThreadError                  (ClearThreadError__3VDKFv)
#else
#define _VDK_ClearThreadError                 _ClearThreadError__3VDKFv
#endif

#ifdef __ECC__
#define VDK_GetLastThreadError                (GetLastThreadError__3VDKFv)
#else
#define _VDK_GetLastThreadError               _GetLastThreadError__3VDKFv
#endif

#ifdef __ECC__
#define VDK_GetLastThreadErrorValue           (GetLastThreadErrorValue__3VDKFv)
#else
#define _VDK_GetLastThreadErrorValue          _GetLastThreadErrorValue__3VDKFv
#endif


    /* Thread Priority Management */

#ifdef __ECC__
#define VDK_GetPriority                       (GetPriority__3VDKF8ThreadID)
#else
#define _VDK_GetPriority                      _GetPriority__3VDKF8ThreadID
#endif

#ifdef __ECC__
#define VDK_ResetPriority                     (ResetPriority__3VDKF8ThreadID)
#else
#define _VDK_ResetPriority                    _ResetPriority__3VDKF8ThreadID
#endif

#ifdef __ECC__
#define VDK_SetPriority                       (SetPriority__3VDKF8ThreadIDQ2_3VDK8Priority)
#else
#define _VDK_SetPriority                      _SetPriority__3VDKF8ThreadIDQ2_3VDK8Priority
#endif


    /* Thread Scheduling Control */

#ifdef __ECC__
#define VDK_Yield                             (Yield__3VDKFv)
#else
#define _VDK_Yield                            _Yield__3VDKFv
#endif

#ifdef __ECC__
#define VDK_Sleep                             (Sleep__3VDKFUi)
#else
#define _VDK_Sleep                            _Sleep__3VDKFUi
#endif


    /* Semaphores Management */
#ifdef __ECC__
#define VDK_CreateSemaphore                   (CreateSemaphore__3VDKFUiN31)
#else
#define _VDK_CreateSemaphore                  _CreateSemaphore__3VDKFUiN31
#endif

#ifdef __ECC__
#define VDK_DestroySemaphore                  (DestroySemaphore__3VDKF11SemaphoreID)
#else
#define _VDK_DestroySemaphore                 _DestroySemaphore__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_PendSemaphore                     (PendSemaphore__3VDKF11SemaphoreIDUi)
#else
#define _VDK_PendSemaphore                    _PendSemaphore__3VDKF11SemaphoreIDUi
#endif

#ifdef __ECC__
#define VDK_PostSemaphore                     (PostSemaphore__3VDKF11SemaphoreID)
#else
#define _VDK_PostSemaphore                    _PostSemaphore__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_C_ISR_PostSemaphore               (C_ISR_PostSemaphore__3VDKF11SemaphoreID)
#else
#define _VDK_C_ISR_PostSemaphore              _C_ISR_PostSemaphore__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_GetSemaphoreValue                 (GetSemaphoreValue__3VDKF11SemaphoreID)
#else
#define _VDK_GetSemaphoreValue                _GetSemaphoreValue__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_MakePeriodic                      (MakePeriodic__3VDKF11SemaphoreIDUiT2)
#else
#define _VDK_MakePeriodic                     _MakePeriodic__3VDKF11SemaphoreIDUiT2
#endif

#ifdef __ECC__
#define VDK_RemovePeriodic                    (RemovePeriodic__3VDKF11SemaphoreID)
#else
#define _VDK_RemovePeriodic                   _RemovePeriodic__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_GetSemaphorePendingThreads        (GetSemaphorePendingThreads__3VDKF11SemaphoreIDPiP8ThreadIDi)
#else
#define _VDK_GetSemaphorePendingThreads       _GetSemaphorePendingThreads__3VDKF11SemaphoreIDPiP8ThreadIDi
#endif

#ifdef __ECC__
#define VDK_GetSemaphoreDetails               (GetSemaphoreDetails__3VDKF11SemaphoreIDPUiT2)
#else
#define _VDK_GetSemaphoreDetails              _GetSemaphoreDetails__3VDKF11SemaphoreIDPUiT2
#endif

#ifdef __ECC__
#define VDK_GetAllSemaphores                  (GetAllSemaphores__3VDKFP11SemaphoreIDi)
#else
#define _VDK_GetAllSemaphores                 _GetAllSemaphores__3VDKFP11SemaphoreIDi
#endif

    /* Fifos */
#ifdef __ECC__
#define VDK_CreateFifo                 (CreateFifo__3VDKFUiPvPFPv_v)
#else
#define _VDK_CreateFifo                 _CreateFifo__3VDKFUiPvPFPv_v
#endif

#ifdef __ECC__
#define VDK_DestroyFifo                 (DestroyFifo__3VDKF6FifoID)
#else
#define _VDK_DestroyFifo                 _DestroyFifo__3VDKF6FifoID
#endif

#ifdef __ECC__
#define VDK_C_ISR_PostFifo                 (C_ISR_PostFifo__3VDKF6FifoIDPv)
#else
#define _VDK_C_ISR_PostFifo                 _C_ISR_PostFifo__3VDKF6FifoIDPv
#endif

#ifdef __ECC__
#define VDK_PostFifo                 (PostFifo__3VDKF6FifoIDPvUi)
#else
#define _VDK_PostFifo                 _PostFifo__3VDKF6FifoIDPvUi
#endif

#ifdef __ECC__
#define VDK_PendFifo                 (PendFifo__3VDKF6FifoIDPPvUi)
#else
#define _VDK_PendFifo                 _PendFifo__3VDKF6FifoIDPPvUi
#endif

#ifdef __ECC__
#define VDK_GetFifoDetails                 (GetFifoDetails__3VDKF6FifoIDPUiT2PPv)
#else
#define _GetFifoDetails                 _GetFifoDetails__3VDKF6FifoIDPUiT2PPv
#endif

    /* Events & EventBits */

#ifdef __ECC__
#define VDK_PendEvent                         (PendEvent__3VDKF7EventIDUi)
#else
#define _VDK_PendEvent                        _PendEvent__3VDKF7EventIDUi
#endif

#ifdef __ECC__
#define VDK_GetEventValue                     (GetEventValue__3VDKF7EventID)
#else
#define _VDK_GetEventValue                    _GetEventValue__3VDKF7EventID
#endif

#ifdef __ECC__
#define VDK_SetEventBit                       (SetEventBit__3VDKF10EventBitID)
#else
#define _VDK_SetEventBit                      _SetEventBit__3VDKF10EventBitID
#endif

#ifdef __ECC__
#define VDK_C_ISR_SetEventBit                 (C_ISR_SetEventBit__3VDKF10EventBitID)
#else
#define _VDK_C_ISR_SetEventBit                _C_ISR_SetEventBit__3VDKF10EventBitID
#endif


#ifdef __ECC__
#define VDK_ClearEventBit                     (ClearEventBit__3VDKF10EventBitID)
#else
#define _VDK_ClearEventBit                    _ClearEventBit__3VDKF10EventBitID
#endif

#ifdef __ECC__
#define VDK_C_ISR_ClearEventBit               (C_ISR_ClearEventBit__3VDKF10EventBitID)
#else
#define _VDK_C_ISR_ClearEventBit              _C_ISR_ClearEventBit__3VDKF10EventBitID
#endif

#ifdef __ECC__
#define VDK_GetEventBitValue                  (GetEventBitValue__3VDKF10EventBitID)
#else
#define _VDK_GetEventBitValue                 _GetEventBitValue__3VDKF10EventBitID
#endif

#ifdef __ECC__
#define VDK_GetEventData                      (GetEventData__3VDKF7EventID)
#else
#define _VDK_GetEventData                     _GetEventData__3VDKF7EventID
#endif

#ifdef __ECC__
#define VDK_LoadEvent                         (LoadEvent__3VDKF7EventIDQ2_3VDK9EventData)
#else
#define _VDK_LoadEvent                        _LoadEvent__3VDKF7EventIDQ2_3VDK9EventData
#endif

#ifdef __ECC__
#define VDK_GetEventPendingThreads            (GetEventPendingThreads__3VDKF7EventIDPiP8ThreadIDi)
#else
#define _VDK_GetEventPendingThreads           _GetEventPendingThreads__3VDKF7EventIDPiP8ThreadIDi
#endif


    /* Device Drivers */

#ifdef __ECC__
#define VDK_OpenDevice                        (OpenDevice__3VDKF4IOIDPc)
#else
#define _VDK_OpenDevice                       _OpenDevice__3VDKF4IOIDPc
#endif

#ifdef __ECC__
#define VDK_CloseDevice                       (CloseDevice__3VDKF16DeviceDescriptor)
#else
#define _VDK_CloseDevice                      _CloseDevice__3VDKF16DeviceDescriptor
#endif

#ifdef __ECC__
#define VDK_SyncRead                          (SyncRead__3VDKF16DeviceDescriptorPcUiT3)
#else
#define _VDK_SyncRead                         _SyncRead__3VDKF16DeviceDescriptorPcUiT3
#endif

#ifdef __ECC__
#define VDK_SyncWrite                         (SyncWrite__3VDKF16DeviceDescriptorPcUiT3)
#else
#define _VDK_SyncWrite                        _SyncWrite__3VDKF16DeviceDescriptorPcUiT3
#endif

#ifdef __ECC__
#define VDK_DeviceIOCtl                       (DeviceIOCtl__3VDKF16DeviceDescriptorPvPc)
#else
#define _VDK_DeviceIOCtl                      _DeviceIOCtl__3VDKF16DeviceDescriptorPvPc
#endif

#ifdef __ECC__
#define VDK_C_ISR_ActivateDevice              (C_ISR_ActivateDevice__3VDKF4IOID)
#else
#define _VDK_C_ISR_ActivateDevice             _C_ISR_ActivateDevice__3VDKF4IOID
#endif

    /* Dynamic Device flags */

#ifdef __ECC__
#define VDK_CreateDeviceFlag                  (CreateDeviceFlag__3VDKFv)
#else
#define _VDK_CreateDeviceFlag                 _CreateDeviceFlag__3VDKFv
#endif

#ifdef __ECC__
#define VDK_DestroyDeviceFlag                 (DestroyDeviceFlag__3VDKF12DeviceFlagID)
#else
#define _VDK_DestroyDeviceFlag                _DestroyDeviceFlag__3VDKF12DeviceFlagID
#endif

#ifdef __ECC__
#define VDK_PendDeviceFlag                    (PendDeviceFlag__3VDKF12DeviceFlagIDUi)
#else
#define _VDK_PendDeviceFlag                   _PendDeviceFlag__3VDKF12DeviceFlagIDUi
#endif

#ifdef __ECC__
#define VDK_PostDeviceFlag                    (PostDeviceFlag__3VDKF12DeviceFlagID)
#else
#define _VDK_PostDeviceFlag                   _PostDeviceFlag__3VDKF12DeviceFlagID
#endif

#ifdef __ECC__
#define VDK_GetDevFlagPendingThreads          (GetDevFlagPendingThreads__3VDKF12DeviceFlagIDPiP8ThreadIDi)
#else
#define _VDK_GetDevFlagPendingThreads         _GetDevFlagPendingThreads__3VDKF12DeviceFlagIDPiP8ThreadIDi
#endif

#ifdef __ECC__
#define VDK_GetAllDeviceFlags                 (GetAllDeviceFlags__3VDKFP12DeviceFlagIDi)
#else
#define _VDK_GetAllDeviceFlags                _GetAllDeviceFlags__3VDKFP12DeviceFlagIDi
#endif

        /* Stack Measurement API */
#ifdef __ECC__
#define VDK_GetThreadStackUsage               (GetThreadStackUsage__3VDKF8ThreadID)
#else
#define _VDK_GetThreadStackUsage              _GetThreadStackUsage__3VDKF8ThreadID
#endif

#ifdef __ECC__
#define VDK_InstrumentStack                   (InstrumentStack__3VDKFv)
#else
#define _VDK_InstrumentStack                  _InstrumentStack__3VDKFv
#endif

    /* History */

#ifdef __ECC__
#define VDK_GetHistoryEvent                   (GetHistoryEvent__3VDKFUiPQ2_3VDK12HistoryEvent)
#else
#define _VDK_GetHistoryEvent                  _GetHistoryEvent__3VDKFUiPQ2_3VDK12HistoryEvent
#endif

#ifdef __ECC__
#define VDK_GetCurrentHistoryEventNum         (GetCurrentHistoryEventNum__3VDKFv)
#else
#define _VDK_GetCurrentHistoryEventNum        _GetCurrentHistoryEventNum__3VDKFv
#endif

#ifdef __ECC__
#define VDK_GetHistoryBufferSize              (GetHistoryBufferSize__3VDKFv)
#else
#define _VDK_GetHistoryBufferSize             _GetHistoryBufferSize__3VDKFv
#endif

#ifdef __ECC__
#define VDK_LogHistoryEvent                   (LogHistoryEvent__3VDKFQ2_3VDK11HistoryEnumi)
#else
#define _VDK_LogHistoryEvent                  _LogHistoryEvent__3VDKFQ2_3VDK11HistoryEnumi
#endif

#ifdef __ECC__
#define VDK_ReplaceHistorySubroutine          (ReplaceHistorySubroutine__3VDKFPFUiQ2_3VDK11HistoryEnumi8ThreadID_v)
#else
#define _VDK_ReplaceHistorySubroutine         _ReplaceHistorySubroutine__3VDKFPFUiQ2_3VDK11HistoryEnumi8ThreadID_v
#endif


    /* Memory Pools */
#ifdef __ECC__
#define VDK_CreatePool                        (CreatePool__3VDKFUiT1b)
#else
#define _VDK_CreatePool                       _CreatePool__3VDKFUiT1b
#endif

#ifdef __ECC__
#define VDK_CreatePoolEx                      (CreatePoolEx__3VDKFUiT1bi)
#else
#define _VDK_CreatePoolEx                     _CreatePoolEx__3VDKFUiT1bi
#endif

#ifdef __ECC__
#define VDK_DestroyPool                       (DestroyPool__3VDKF6PoolID)
#else
#define _VDK_DestroyPool                      _DestroyPool__3VDKF6PoolID
#endif

#ifdef __ECC__
#define VDK_MallocBlock                       (MallocBlock__3VDKF6PoolID)
#else
#define _VDK_MallocBlock                      _MallocBlock__3VDKF6PoolID
#endif

#ifdef __ECC__
#define VDK_FreeBlock                         (FreeBlock__3VDKF6PoolIDPv)
#else
#define _VDK_FreeBlock                        _FreeBlock__3VDKF6PoolIDPv
#endif

#ifdef __ECC__
#define VDK_LocateAndFreeBlock                (LocateAndFreeBlock__3VDKFPv)
#else
#define _VDK_LocateAndFreeBlock               _LocateAndFreeBlock__3VDKFPv
#endif

#ifdef __ECC__
#define VDK_GetNumFreeBlocks                  (GetNumFreeBlocks__3VDKF6PoolID)
#else
#define _VDK_GetNumFreeBlocks                 _GetNumFreeBlocks__3VDKF6PoolID
#endif

#ifdef __ECC__
#define VDK_GetNumAllocatedBlocks             (GetNumAllocatedBlocks__3VDKF6PoolID)
#else
#define _VDK_GetNumAllocatedBlocks            _GetNumAllocatedBlocks__3VDKF6PoolID
#endif

#ifdef __ECC__
#define VDK_GetBlockSize                      (GetBlockSize__3VDKF6PoolID)
#else
#define _VDK_GetBlockSize                     _GetBlockSize__3VDKF6PoolID
#endif

#ifdef __ECC__
#define VDK_GetPoolDetails                    (GetPoolDetails__3VDKF6PoolIDPUiPPv)
#else
#define _VDK_GetPoolDetails                   _GetPoolDetails__3VDKF6PoolIDPUiPPv
#endif

#ifdef __ECC__
#define VDK_GetAllMemoryPools                 (GetAllMemoryPools__3VDKFP6PoolIDi)
#else
#define _VDK_GetAllMemoryPools                _GetAllMemoryPools__3VDKFP6PoolIDi
#endif

/* Thread Local Storage */

#ifdef __ECC__
#define VDK_AllocateThreadSlot                (AllocateThreadSlot__3VDKFPi)
#else
#define _VDK_AllocateThreadSlot               _AllocateThreadSlot__3VDKFPi
#endif

#ifdef __ECC__
#define VDK_AllocateThreadSlotEx              (AllocateThreadSlotEx__3VDKFPiPFPv_v)
#else
#define _VDK_AllocateThreadSlotEx             _AllocateThreadSlotEx__3VDKFPiPFPv_v
#endif

#ifdef __ECC__
#define VDK_FreeThreadSlot                    (FreeThreadSlot__3VDKFi)
#else
#define _VDK_FreeThreadSlot                   _FreeThreadSlot__3VDKFi
#endif

#ifdef __ECC__
#define VDK_GetThreadSlotValue                (GetThreadSlotValue__3VDKFi)
#else
#define _VDK_GetThreadSlotValue               _GetThreadSlotValue__3VDKFi
#endif

#ifdef __ECC__
#define VDK_SetThreadSlotValue                (SetThreadSlotValue__3VDKFiPv)
#else
#define _VDK_SetThreadSlotValue               _SetThreadSlotValue__3VDKFiPv
#endif

    /* Messages */
#ifdef __ECC__
#define VDK_CreateMessage                     (CreateMessage__3VDKFiUiPv)
#else
#define _VDK_CreateMessage                    _CreateMessage__3VDKFiUiPv
#endif

#ifdef __ECC__
#define VDK_DestroyMessage                    (DestroyMessage__3VDKF9MessageID)
#else
#define _VDK_DestroyMessage                   _DestroyMessage__3VDKF9MessageID
#endif

#ifdef __ECC__
#define VDK_DestroyMessageAndFreePayload      (DestroyMessageAndFreePayload__3VDKF9MessageID)
#else
#define _VDK_DestroyMessageAndFreePayload     _DestroyMessageAndFreePayload__3VDKF9MessageID
#endif

#ifdef __ECC__
#define VDK_FreeMessagePayload                (FreeMessagePayload__3VDKF9MessageID)
#else
#define _VDK_FreeMessagePayload               _FreeMessagePayload__3VDKF9MessageID
#endif

#ifdef __ECC__
#define VDK_PostMessage                       (PostMessage__3VDKF8ThreadID9MessageIDQ2_3VDK10MsgChannel)
#else
#define _VDK_PostMessage                      _PostMessage__3VDKF8ThreadID9MessageIDQ2_3VDK10MsgChannel
#endif

#ifdef __ECC__
#define VDK_PendMessage                       (PendMessage__3VDKFUiT1)
#else
#define _VDK_PendMessage                      _PendMessage__3VDKFUiT1
#endif

#ifdef __ECC__
#define VDK_MessageAvailable                  (MessageAvailable__3VDKFUi)
#else
#define _VDK_MessageAvailable                 _MessageAvailable__3VDKFUi
#endif

#ifdef __ECC__
#define VDK_GetMessagePayload                 (GetMessagePayload__3VDKF9MessageIDPiPUiPPv)
#else
#define _VDK_GetMessagePayload                _GetMessagePayload__3VDKF9MessageIDPiPUiPPv
#endif

#ifdef __ECC__
#define VDK_SetMessagePayload                 (SetMessagePayload__3VDKF9MessageIDiUiPv)
#else
#define _VDK_SetMessagePayload                _SetMessagePayload__3VDKF9MessageIDiUiPv
#endif

#ifdef __ECC__
#define VDK_GetMessageReceiveInfo             (GetMessageReceiveInfo__3VDKF9MessageIDPQ2_3VDK10MsgChannelP8ThreadID)
#else
#define _VDK_GetMessageReceiveInfo            _GetMessageReceiveInfo__3VDKF9MessageIDPQ2_3VDK10MsgChannelP8ThreadID
#endif

#ifdef __ECC__
#define VDK_GetAllMessages                    (GetAllMessages__3VDKFP9MessageIDi)
#else
#define _VDK_GetAllMessages                   _GetAllMessages__3VDKFP9MessageIDi
#endif

#ifdef __ECC__
#define VDK_GetMessageDetails                 (GetMessageDetails__3VDKF9MessageIDPQ2_3VDK14MessageDetailsPQ2_3VDK14PayloadDetails)
#else
#define _VDK_GetMessageDetails                _GetMessageDetails__3VDKF9MessageIDPQ2_3VDK14MessageDetailsPQ2_3VDK14PayloadDetails
#endif

#ifdef __ECC__
#define VDK_GetMessageStatusInfo              (GetMessageStatusInfo__3VDKF9MessageIDPQ2_3VDK14MessageDetailsPQ2_3VDK14PayloadDetails)
#else
#define _VDK_GetMessageStatusInfo             _GetMessageStatusInfo__3VDKF9MessageIDPQ2_3VDK14MessageDetailsPQ2_3VDK14PayloadDetails
#endif

#ifdef __ECC__
#define VDK_ForwardMessage                    (ForwardMessage__3VDKF8ThreadID9MessageIDQ2_3VDK10MsgChannelT1)
#else
#define _VDK_ForwardMessage                   _ForwardMessage__3VDKF8ThreadID9MessageIDQ2_3VDK10MsgChannelT1
#endif

#ifdef __ECC__
#define VDK_InstallMessageControlSemaphore    (InstallMessageControlSemaphore__3VDKF11SemaphoreID)
#else
#define _VDK_InstallMessageControlSemaphore   _InstallMessageControlSemaphore__3VDKF11SemaphoreID
#endif

#ifdef __ECC__
#define VDK_GetTickPeriod                     (GetTickPeriod__3VDKFv)
#else
#define _VDK_GetTickPeriod                    _GetTickPeriod__3VDKFv
#endif

#ifdef __ECC__
#define VDK_SetTickPeriod                     (SetTickPeriod__3VDKFf)
#else
#define _VDK_SetTickPeriod                    _SetTickPeriod__3VDKFf
#endif

#ifdef __ECC__
#define VDK_GetClockFrequency                 (GetClockFrequency__3VDKFv)
#else
#define _VDK_GetClockFrequency                _GetClockFrequency__3VDKFv
#endif

#ifdef __ECC__
#define VDK_SetClockFrequency                 (SetClockFrequency__3VDKFUi)
#else
#define _VDK_SetClockFrequency                _SetClockFrequency__3VDKFUi
#endif

#ifdef __ECC__
#define VDK_GetClockFrequencyKHz              (GetClockFrequencyKHz__3VDKFv)
#else
#define _VDK_GetClockFrequencyKHz             _GetClockFrequencyKHz__3VDKFv
#endif

#ifdef __ECC__
#define VDK_SetClockFrequencyKHz              (SetClockFrequencyKHz__3VDKFf)
#else
#define   _VDK_SetClockFrequencyKHz           _SetClockFrequencyKHz__3VDKFf
#endif

#ifdef __ECC__
#define VDK_GetHeapIndex                      (GetHeapIndex__3VDKF6HeapID)
#else
#define _VDK_GetHeapIndex                     _GetHeapIndex__3VDKF6HeapID
#endif

#ifdef __ECC__
#define VDK_CreateMutex                       (CreateMutex__3VDKFv)
#else
#define _VDK_CreateMutex                      _CreateMutex__3VDKFv
#endif

#ifdef __ECC__
#define VDK_DestroyMutex                      (DestroyMutex__3VDKF7MutexID)
#else
#define _VDK_DestroyMutex                     _DestroyMutex__3VDKF7MutexID
#endif

#ifdef __ECC__
#define VDK_AcquireMutex                      (AcquireMutex__3VDKF7MutexID)
#else
#define _VDK_AcquireMutex                     _AcquireMutex__3VDKF7MutexID
#endif

#ifdef __ECC__
#define VDK_ReleaseMutex                      (ReleaseMutex__3VDKF7MutexID)
#else
#define _VDK_ReleaseMutex                     _ReleaseMutex__3VDKF7MutexID
#endif

/* functions required if users want to replace main */

#ifdef __ECC__
#define  VDK_Initialize                       (Initialize__3VDKFv)
#else
#define  _VDK_Initialize                      _Initialize__3VDKFv
#endif

#ifdef __ECC__
#define  VDK_Run                              (Run__3VDKFv)
#else
#define  _VDK_Run                             _Run__3VDKFv
#endif

/* Undocumented function taken from Andromeda to support OSAL */
#ifdef __ECC__
#define  VDK_IsRunning                        (IsRunning__3VDKFv)
#else
#define  _VDK_IsRunning                       _IsRunning__3VDKFv
#endif

#ifdef __ECC__
#define  VDK_IsInRegion                       (IsInRegion__3VDKFv)
#else
#define  _VDK_IsInRegion                      _IsInRegion__3VDKFv
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* __cplusplus */

