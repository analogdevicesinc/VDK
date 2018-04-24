/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The API for VDK callable from the interrupt domain
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: This is a prose block which describes any general working notes
 *  of immediate relevence, including the purpose of the class or functions, any
 *  known bugs, work to be done, or specific instructions for building this file.  
 *  Bugs fixes and changes belong in the footer, not here.
 * -----------------------------------------------------------------------------
 * ===========================================================================*/



#ifndef VDK_ISR_API_H_
#define VDK_ISR_API_H_

#define VDK_INTERRUPT_LEVEL_ 0x80000000 
#define VDK_KERNEL_LEVEL_ 0xC0000000 
/*****************************************************************************/


/*****************************************************************************/
/* Code                                                                      */
/*****************************************************************************/


/*****************************************************************************/
/**
 * Posts a semaphore.
 */
#ifndef __ECC__
#define VDK_ISR_POST_SEMAPHORE_REG_(semaphore_num_) \
.extern _VDK_ISR_POST_SEMAPHORE_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0 = semaphore_num_;							\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_POST_SEMAPHORE_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]										

#define VDK_ISR_POST_SEMAPHORE_(semaphore_num_)     \
.extern _VDK_ISR_POST_SEMAPHORE_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0.L = semaphore_num_ & 0xFFFF;					\
	R0.H = (semaphore_num_ >> 16) & 0xFFFF;			\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_POST_SEMAPHORE_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]										

/*****************************************************************************/
/**
 * Activate a device driver
 */
#define VDK_ISR_ACTIVATE_DEVICE_REG_(dev_num_)      \
.extern _VDK_ISR_ACTIVATE_DEVICE_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0 = dev_num_;									\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_ACTIVATE_DEVICE_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]

#define VDK_ISR_ACTIVATE_DEVICE_(dev_num_)      	\
.extern _VDK_ISR_ACTIVATE_DEVICE_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0.L = dev_num_ & 0xFFFF;						\
	R0.H = (dev_num_ >> 16) & 0xFFFF;				\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_ACTIVATE_DEVICE_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]


/*****************************************************************************/
/**
 * Sets an eventbit.
 */
#define VDK_ISR_SET_EVENTBIT_REG_(eventbit_num_)    \
.extern _VDK_ISR_SET_EVENTBIT_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0 = eventbit_num_;								\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_SET_EVENTBIT_code__3VDK;			\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]

#define VDK_ISR_SET_EVENTBIT_(eventbit_num_)        \
.extern _VDK_ISR_SET_EVENTBIT_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0.L = eventbit_num_ & 0xFFFF;					\
	R0.H = (eventbit_num_ >> 16) & 0xFFFF;			\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_SET_EVENTBIT_code__3VDK;			\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]

/**
 * Clears an eventbit.
 */
#define VDK_ISR_CLEAR_EVENTBIT_REG_(eventbit_num_)  \
.extern _VDK_ISR_CLEAR_EVENTBIT_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0 = eventbit_num_;								\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_CLEAR_EVENTBIT_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]

#define VDK_ISR_CLEAR_EVENTBIT_(eventbit_num_)      \
.extern _VDK_ISR_CLEAR_EVENTBIT_code__3VDK; \
	[--SP] = R0;									\
    [--SP] = P1;                                    \
	R0.L = eventbit_num_ & 0xFFFF;					\
	R0.H = (eventbit_num_ >> 16) & 0xFFFF;			\
	[--SP] = RETS;									\
	CALL.X _VDK_ISR_CLEAR_EVENTBIT_code__3VDK;		\
	RETS = 	[SP++];									\
    P1 = [SP++];                                    \
	R0 = [SP++]



#if (VDK_INSTRUMENTATION_LEVEL_==2)
/*****************************************************************************/
/**
 * Log a history event 
 */
#define VDK_ISR_LOG_HISTORY_REG_(enum_, value_, threadID_)  \
.extern _VDK_History;									\
.extern g_pfVDKISRHistoryLogging;						\
	[--SP] = R0;										\
	[--SP] = R1;										\
    [--SP] = R2;                                        \
	[--SP] = P1;                                        \
	[--SP] = RETS;										\
	R0 = enum_;											\
	R1 = value_;										\
	R2.L = LO(VDK_INTERRUPT_LEVEL_);					\
	R2.H = HI(VDK_INTERRUPT_LEVEL_);					\
	CALL.X _VDK_History;								\
	RETS = 	[SP++];										\
	P1 = [SP++];                                        \
    R2 = [SP++];                                        \
    R1 = [SP++];                                        \
	R0 = [SP++]

#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_History;									\
.extern g_pfVDKISRHistoryLogging;						\
	[--SP] = R0;										\
	[--SP] = R1;										\
	[--SP] = R2;										\
	[--SP] = P1;                                        \
	[--SP] = RETS;										\
	R0.L = enum_ & 0xFFFF;								\
	R0.H = (enum_ >> 16) & 0xFFFF;						\
	R1.L = value_ & 0xFFFF;								\
	R1.H = (value_ >> 16) & 0xFFFF;						\
	R2.L = LO(VDK_INTERRUPT_LEVEL_);					\
	R2.H = HI(VDK_INTERRUPT_LEVEL_);					\
	CALL.X _VDK_History;								\
	RETS = 	[SP++];										\
	P1 = [SP++];                                        \
	R2 = [SP++];                                        \
	R1 = [SP++];										\
	R0 = [SP++]

// BEGIN_INTERNALS_STRIP

// This Macro takes-in:
// enum - a History enumeration
// value - a register that contains the appropriate
//			val for the given enumeration.
// ThreadID is not required as it will always be
// at interrupt level.
#define VDK_ISR_INTERNAL_HISTORY_LOG_(enum_, value_reg_) \
        [--SP] = RETS;                          \
        [--SP] = R2;                            \
        [--SP] = R1;                            \
        [--SP] = R0;                            \
        [--SP] = P1;							\
		R2.L = LO(VDK_INTERRUPT_LEVEL_);		\
		R2.H = HI(VDK_INTERRUPT_LEVEL_);		\
        R1 = value_reg_;						\
		R0.L = enum_ & 0xFFFF;					\
		R0.H = (enum_ >> 16) & 0xFFFF;			\
		CALL.X _VDK_History;					\
		P1 = [SP++];							\
        R0 = [SP++] ;							\
        R1 = [SP++] ;							\
        R2 = [SP++] ;							\
        RETS = [SP++]

// END_INTERNALS_STRIP

#else  /* VDK_INSTRUMENTATION_LEVEL_) */

#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)
#define VDK_ISR_LOG_HISTORY_REG_(enum_, value_, threadID_)

#endif /* VDK_INSTRUMENTATION_LEVEL_ */
#endif /* ECC */
#endif /* VDK_ISR_API_H_ */

