	/* Generated by tcl */
	
	#ifndef __VDK_ASM_OFFSETS_H_
	#define __VDK_ASM_OFFSETS_H_
	
	
	#define VDK_MUTEX_SIZE (5U)
	#define VDK_MUTEX_LEN (5U)
	#define __VDK_SIZEOF_IDElement_ (1)
	// BEGIN_INTERNALS_STRIP
	#define OFFSET_IDTable_m_TableSize_ (1)
	#define OFFSET_IDTable_m_IDTable_ (0)
	#define OFFSET_IDTableElement_m_ObjectPtr_ (0)
	#define OFFSET_IDTableElement_m_IndexCount_ (1)
	#define OFFSET_Semaphore_m_Value_ (13)
	#define OFFSET_Semaphore_m_ID_ (0)
	#define OFFSET_Semaphore_m_PendingThreads_ (8)
	#define OFFSET_Semaphore_m_PendingThreads_pHead_ (8)
	#define OFFSET_Semaphore_m_MaxCount_ (12)
	#define OFFSET_Semaphore_m_Period_ (11)
	#define __VDK_SIZEOF_Semaphore_ (14)
	#define ENUM_kThreadError_ (1)
	#define ENUM_kISRError_ (3)
	#define ENUM_kUnknownSemaphore_ (-2147483644)
	#define ENUM_kUnknownEventBit_ (-2147483643)
	#define ENUM_kBadIOID_ (-2147483631)
	#define ENUM_kAbort_ (-2147483596)
	#define ENUM_kThreadStackOverflow_ (-2147483593)
	#define ENUM_kStackCheckFailure_ (6)
	
	#define OFFSET_Thread_m_DebugInfoP_ (14)
	
	#define OFFSET_DebugInfo_m_nNumTimesRun_ (0)
	#define OFFSET_DebugInfo_m_llRunStartCycles_ (3)
	#define OFFSET_DebugInfo_m_llRunLastCycles_ (5)
	#define OFFSET_DebugInfo_m_llRunTotalCycles_ (7)
	#define OFFSET_DebugInfo_m_tRunStartTime_ (10)
	#define OFFSET_DebugInfo_m_tRunLastTime_ (11)
	#define OFFSET_DebugInfo_m_tRunTotalTime_ (12)
	
	#define __VDK_SIZEOF_HistoryEvent_ (4)
	#define __VDK_SIZEOF_HistoryStruct_ (4)
	#define ENUM_VDK_kSemaphorePosted_ (-2147483646)
	#define ENUM_VDK_kEventBitSet_ (-2147483644)
	#define ENUM_VDK_kEventBitCleared_ (-2147483643)
	#define ENUM_VDK_kDeviceActivated_ (-2147483639)
	#define ENUM_VDK_kThreadStatusChange_ (-2147483637)
	#define ENUM_VDK_kThreadSwitched_ (-2147483636)
	
	#define __VDK_SIZEOF_EventBit_ (1)
	
	#define POINTER_CONVERSION_Semaphore_TO_TMK_DpcElement_ (5)
	#define POINTER_CONVERSION_DeviceDriver_TO_TMK_DpcElement_ (2)
	#define POINTER_CONVERSION_Thread_TO_TMK_Thread_ (1)
	
	// END_INTERNALS_STRIP
	
	#endif /* __VDK_ASM_OFFSETS_H_*/
