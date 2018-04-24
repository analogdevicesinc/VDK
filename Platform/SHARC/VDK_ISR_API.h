/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/*
 *=============================================================================
 *
 *   $RCSfile$
 *
 *   Description: This is an example file for header file formatting
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

#ifndef __ECC__
#include <platform_include.h>

/*****************************************************************************/

/*****************************************************************************/

#define VDK_INTERRUPT_LEVEL_ 0x80000000
#define VDK_KERNEL_LEVEL_ 0xC0000000 
/*****************************************************************************/
/* Code                                                                      */
/*****************************************************************************/

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
// macro to indicate we need to work around anomaly 15000011
// Incorrect Execution of VISA CALL(DB) Instructions under specific conditions
#define WA_15000011
#endif

#ifdef VDK_SAVE_RESTORE_40_BITS
// These versions of the VDK_ISR_COMMON_() macro use the PX register to save
// and restore all 40 bits of the data registers that are used (R1 and R4).
//
#ifdef __2136x__

// On 2136x we don't save&restore the top of the PC stack, as this would trash
// the "hidden" bits that are required for return into hardware loops.
//
#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_R1L__3VDK?;                          \
.var _ISR_SAVE_R1H__3VDK?;                          \
.var _ISR_SAVE_PX1__3VDK?;                          \
.var _ISR_SAVE_PX2__3VDK?;                          \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_PX1__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_PX2__3VDK?) = PX2;                 \
    PX = R1;                                        \
    DM(_ISR_SAVE_R1L__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_R1H__3VDK?) = PX2;                 \
    CALL func_(DB);                                 \
    PX = R4;                                        \
    R4 = item_;                                     \
    R4 = PX;                                        \
    PX1 = DM(_ISR_SAVE_R1L__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_R1H__3VDK?);                 \
    R1 = PX;                                        \
    PX1 = DM(_ISR_SAVE_PX1__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_PX2__3VDK?);

#elif defined (WA_15000011)
// Work around anomaly 15000011. We cannot use call(db)

#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_PCTOP__3VDK?;                        \
.var _ISR_SAVE_R1L__3VDK?;                          \
.var _ISR_SAVE_R1H__3VDK?;                          \
.var _ISR_SAVE_PX1__3VDK?;                          \
.var _ISR_SAVE_PX2__3VDK?;                          \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_PCTOP__3VDK?) = PCSTK;             \
    POP PCSTK;                                      \
    DM(_ISR_SAVE_PX1__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_PX2__3VDK?) = PX2;                 \
    PX = R1;                                        \
    DM(_ISR_SAVE_R1L__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_R1H__3VDK?) = PX2;                 \
    PX = R4;                                        \
    R4 = item_;                                     \
    CALL func_;                                     \
    R4 = PX;                                        \
    PX1 = DM(_ISR_SAVE_R1L__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_R1H__3VDK?);                 \
    R1 = PX;                                        \
    PX1 = DM(_ISR_SAVE_PX1__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_PX2__3VDK?);                 \
    PUSH PCSTK;                                     \
    PCSTK = DM(_ISR_SAVE_PCTOP__3VDK?);

#else /* !__2136x__ and ! WA_15000011 */

#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_PCTOP__3VDK?;                        \
.var _ISR_SAVE_R1L__3VDK?;                          \
.var _ISR_SAVE_R1H__3VDK?;                          \
.var _ISR_SAVE_PX1__3VDK?;                          \
.var _ISR_SAVE_PX2__3VDK?;                          \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_PCTOP__3VDK?) = PCSTK;             \
    POP PCSTK;                                      \
    DM(_ISR_SAVE_PX1__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_PX2__3VDK?) = PX2;                 \
    PX = R1;                                        \
    DM(_ISR_SAVE_R1L__3VDK?) = PX1;                 \
    DM(_ISR_SAVE_R1H__3VDK?) = PX2;                 \
    CALL func_(DB);                                 \
    PX = R4;                                        \
    R4 = item_;                                     \
    R4 = PX;                                        \
    PX1 = DM(_ISR_SAVE_R1L__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_R1H__3VDK?);                 \
    R1 = PX;                                        \
    PX1 = DM(_ISR_SAVE_PX1__3VDK?);                 \
    PX2 = DM(_ISR_SAVE_PX2__3VDK?);                 \
    PUSH PCSTK;                                     \
    PCSTK = DM(_ISR_SAVE_PCTOP__3VDK?);

#endif /* !__2136x__ */
#else /* !VDK_SAVE_RESTORE_40_BITS */
#ifdef __2136x__

// On 2136x we don't save&restore the top of the PC stack, as this would trash
// the "hidden" bits that are required for return into hardware loops.
//
#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_R4__3VDK?;                           \
.var _ISR_SAVE_R1__3VDK?;                           \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_R1__3VDK?) = R1;                   \
    CALL func_(DB);                                 \
    DM(_ISR_SAVE_R4__3VDK?) = R4;                   \
    R4 = item_;                                     \
    R4 = DM(_ISR_SAVE_R4__3VDK?);                   \
    R1 = DM(_ISR_SAVE_R1__3VDK?);

#elif defined (WA_15000011)
// Work around anomaly 15000011. We cannot use call(db)
#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_PCTOP__3VDK?;                        \
.var _ISR_SAVE_R4__3VDK?;                           \
.var _ISR_SAVE_R1__3VDK?;                           \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_PCTOP__3VDK?) = PCSTK;             \
    POP PCSTK;                                      \
    DM(_ISR_SAVE_R1__3VDK?) = R1;                   \
    DM(_ISR_SAVE_R4__3VDK?) = R4;                   \
    R4 = item_;                                     \
    CALL func_;                                 \
    R4 = DM(_ISR_SAVE_R4__3VDK?);                   \
    R1 = DM(_ISR_SAVE_R1__3VDK?);                   \
    PUSH PCSTK;                                     \
    PCSTK = DM(_ISR_SAVE_PCTOP__3VDK?);

#else /* !__2136x__ and ! WA_15000011*/

#define VDK_ISR_COMMON_(func_,item_)                \
.SECTION/dm internal_memory_data;                   \
.var _ISR_SAVE_PCTOP__3VDK?;                        \
.var _ISR_SAVE_R4__3VDK?;                           \
.var _ISR_SAVE_R1__3VDK?;                           \
.PREVIOUS;                                          \
    DM(_ISR_SAVE_PCTOP__3VDK?) = PCSTK;             \
    POP PCSTK;                                      \
    DM(_ISR_SAVE_R1__3VDK?) = R1;                   \
    CALL func_(DB);                                 \
    DM(_ISR_SAVE_R4__3VDK?) = R4;                   \
    R4 = item_;                                     \
    R4 = DM(_ISR_SAVE_R4__3VDK?);                   \
    R1 = DM(_ISR_SAVE_R1__3VDK?);                   \
    PUSH PCSTK;                                     \
    PCSTK = DM(_ISR_SAVE_PCTOP__3VDK?);

#endif /* !__2136x__ */
#endif /* !VDK_SAVE_RESTORE_40_BITS */

/*****************************************************************************/
/**
 * Posts a semaphore.
 */
#define VDK_ISR_POST_SEMAPHORE_(semaphore_num_) \
.extern _ISR_POST_SEMAPHORE_code__3VDK;         \
    VDK_ISR_COMMON_(_ISR_POST_SEMAPHORE_code__3VDK, semaphore_num_)

/**
 * Set an event bit
 */
#define VDK_ISR_SET_EVENTBIT_(eventbit_num_)    \
.extern _ISR_SET_EVENTBIT_code__3VDK;           \
    VDK_ISR_COMMON_(_ISR_SET_EVENTBIT_code__3VDK, eventbit_num_)

/**
 * Clear an event bit
 */
#define VDK_ISR_CLEAR_EVENTBIT_(eventbit_num_)  \
.extern _ISR_CLEAR_EVENTBIT_code__3VDK;         \
    VDK_ISR_COMMON_(_ISR_CLEAR_EVENTBIT_code__3VDK, eventbit_num_)

/**
 * Activate a device driver
 */
#define VDK_ISR_ACTIVATE_DEVICE_(dev_num_)      \
.extern _VDK_ACTIVATE_DEVICE_code__3VDK;        \
    VDK_ISR_COMMON_(_VDK_ACTIVATE_DEVICE_code__3VDK, dev_num_)


/*****************************************************************************/
/**
 * Log a history event
 */
#if VDK_INSTRUMENTATION_LEVEL_ == 2

#ifdef VDK_SAVE_RESTORE_40_BITS
// These versions of the VDK_ISR_LOG_HISTORY_() macro use the PX register to save
// and restore all 40 bits of the data registers that are used (R1, R4, R8, and R12).
//
#ifdef __2136x__

// On 2136x we don't save&restore the top of the PC stack, as this would trash
// the "hidden" bits that are required for return into hardware loops. This applies
// to VDK_ISR_LOG_HISTORY_() which can be called from a usr ISR, but not to
// VDK_ISR_INTERNAL_HISTORY_LOG_(), which can't.
//
// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;								\
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_R1?;							\
.var VDK_ISR_HISTORY_SV_R4?;							\
.var VDK_ISR_HISTORY_SV_R8?;							\
.var VDK_ISR_HISTORY_SV_R12?;							\
.PREVIOUS;												\
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;				\
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;				\
        DM(VDK_ISR_HISTORY_SV_PX1?) = PX1;				\
        DM(VDK_ISR_HISTORY_SV_PX2?) = PX2;				\
        PX = R8;                                        \
        DM(VDK_ISR_HISTORY_SV_R8L?) = PX1;				\
        DM(VDK_ISR_HISTORY_SV_R8H?) = PX2;				\
        PX = R12;                                       \
        R4 = enum_;										\
		CALL _VDK_ASM_History (DB);						\
		R8 = value_;									\
		R12 = VDK_INTERRUPT_LEVEL_;						\
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);				\
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);				\
        R12 = PX;                                       \
        PX1 = DM(VDK_ISR_HISTORY_SV_R8L?);				\
        PX2 = DM(VDK_ISR_HISTORY_SV_R8K?);				\
        R8 = PX;                                        \
		PX1 = DM(VDK_ISR_HISTORY_SV_PX1?)               \
		PX2 = DM(VDK_ISR_HISTORY_SV_PX2?)

#elif defined (WA_15000011)
// Work around anomaly 15000011. We cannot use call(db)
// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;                               \
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_PCTOP?;                         \
.var VDK_ISR_HISTORY_SV_R1?;                            \
.var VDK_ISR_HISTORY_SV_R4?;                            \
.var VDK_ISR_HISTORY_SV_R8L?;                           \
.var VDK_ISR_HISTORY_SV_R8H?;                           \
.var VDK_ISR_HISTORY_SV_PX1?;                           \
.var VDK_ISR_HISTORY_SV_PX2?;                           \
.PREVIOUS;                                              \
        DM(VDK_ISR_HISTORY_SV_PCTOP?) = PCSTK;          \
        POP PCSTK;                                      \
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;                \
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;                \
        DM(VDK_ISR_HISTORY_SV_PX1?) = PX1;              \
        DM(VDK_ISR_HISTORY_SV_PX2?) = PX2;              \
        PX = R8;                                        \
        DM(VDK_ISR_HISTORY_SV_R8L?) = PX1;              \
        DM(VDK_ISR_HISTORY_SV_R8H?) = PX2;              \
        PX = R12;                                       \
        R4 = enum_;                                     \
        R8 = value_;                                    \
        R12 = VDK_INTERRUPT_LEVEL_;                     \
        CALL _VDK_ASM_History ;                         \
        PUSH PCSTK;                                     \
        PCSTK = DM(VDK_ISR_HISTORY_SV_PCTOP?);          \
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);                \
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);                \
        R12 = PX;                                       \
        PX1 = DM(VDK_ISR_HISTORY_SV_R8L?);              \
        PX2 = DM(VDK_ISR_HISTORY_SV_R8K?);              \
        R8 = PX;                                        \
        PX1 = DM(VDK_ISR_HISTORY_SV_PX1?)               \
        PX2 = DM(VDK_ISR_HISTORY_SV_PX2?)

#else /* !__2136x__ && !WA_15000011 */

// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;								\
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_PCTOP?;							\
.var VDK_ISR_HISTORY_SV_R1?;							\
.var VDK_ISR_HISTORY_SV_R4?;							\
.var VDK_ISR_HISTORY_SV_R8L?;							\
.var VDK_ISR_HISTORY_SV_R8H?;							\
.var VDK_ISR_HISTORY_SV_PX1?;							\
.var VDK_ISR_HISTORY_SV_PX2?;							\
.PREVIOUS;												\
		DM(VDK_ISR_HISTORY_SV_PCTOP?) = PCSTK;			\
		POP PCSTK;										\
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;				\
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;				\
        DM(VDK_ISR_HISTORY_SV_PX1?) = PX1;				\
        DM(VDK_ISR_HISTORY_SV_PX2?) = PX2;				\
        PX = R8;                                        \
        DM(VDK_ISR_HISTORY_SV_R8L?) = PX1;				\
        DM(VDK_ISR_HISTORY_SV_R8H?) = PX2;				\
        PX = R12;                                       \
        R4 = enum_;										\
		CALL _VDK_ASM_History (DB);						\
		R8 = value_;									\
		R12 = VDK_INTERRUPT_LEVEL_;						\
		PUSH PCSTK;										\
		PCSTK = DM(VDK_ISR_HISTORY_SV_PCTOP?);			\
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);				\
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);				\
        R12 = PX;                                       \
        PX1 = DM(VDK_ISR_HISTORY_SV_R8L?);				\
        PX2 = DM(VDK_ISR_HISTORY_SV_R8K?);				\
        R8 = PX;                                        \
		PX1 = DM(VDK_ISR_HISTORY_SV_PX1?)               \
		PX2 = DM(VDK_ISR_HISTORY_SV_PX2?)

#endif /* !__2136x__ */
#else /* !VDK_SAVE_RESTORE_40_BITS */
#ifdef __2136x__

// On 2136x we don't save&restore the top of the PC stack, as this would trash
// the "hidden" bits that are required for return into hardware loops. This applies
// to VDK_ISR_LOG_HISTORY_() which can be called from a usr ISR, but not to
// VDK_ISR_INTERNAL_HISTORY_LOG_(), which can't.
//
// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;								\
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_R1?;							\
.var VDK_ISR_HISTORY_SV_R4?;							\
.var VDK_ISR_HISTORY_SV_R8?;							\
.var VDK_ISR_HISTORY_SV_R12?;							\
.PREVIOUS;												\
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;				\
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;				\
        DM(VDK_ISR_HISTORY_SV_R8?) = R8;				\
		DM(VDK_ISR_HISTORY_SV_R12?) = R12;				\
        R4 = enum_;										\
		CALL _VDK_ASM_History (DB);						\
		R8 = value_;									\
		R12 = VDK_INTERRUPT_LEVEL_;						\
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);				\
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);				\
        R8 = DM(VDK_ISR_HISTORY_SV_R8?);				\
		R12 = DM(VDK_ISR_HISTORY_SV_R12?)

#elif defined (WA_15000011)
// Work around anomaly 15000011. We cannot use call(db)
// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;								\
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_PCTOP?;							\
.var VDK_ISR_HISTORY_SV_R1?;							\
.var VDK_ISR_HISTORY_SV_R4?;							\
.var VDK_ISR_HISTORY_SV_R8?;							\
.var VDK_ISR_HISTORY_SV_R12?;							\
.PREVIOUS;												\
		DM(VDK_ISR_HISTORY_SV_PCTOP?) = PCSTK;			\
		POP PCSTK;										\
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;				\
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;				\
        DM(VDK_ISR_HISTORY_SV_R8?) = R8;				\
		DM(VDK_ISR_HISTORY_SV_R12?) = R12;				\
        R4 = enum_;										\
		R8 = value_;									\
		R12 = VDK_INTERRUPT_LEVEL_;						\
		CALL _VDK_ASM_History;   						\
		PUSH PCSTK;										\
		PCSTK = DM(VDK_ISR_HISTORY_SV_PCTOP?);			\
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);				\
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);				\
        R8 = DM(VDK_ISR_HISTORY_SV_R8?);				\
		R12 = DM(VDK_ISR_HISTORY_SV_R12?)


#else /* !__2136x__ && !WA_15000011 */

// threadID_ is unused
#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)  \
.extern _VDK_ASM_History;								\
.SECTION/dm seg_dmda;                                   \
.var VDK_ISR_HISTORY_SV_PCTOP?;							\
.var VDK_ISR_HISTORY_SV_R1?;							\
.var VDK_ISR_HISTORY_SV_R4?;							\
.var VDK_ISR_HISTORY_SV_R8?;							\
.var VDK_ISR_HISTORY_SV_R12?;							\
.PREVIOUS;												\
		DM(VDK_ISR_HISTORY_SV_PCTOP?) = PCSTK;			\
		POP PCSTK;										\
        DM(VDK_ISR_HISTORY_SV_R1?) = R1;				\
        DM(VDK_ISR_HISTORY_SV_R4?) = R4;				\
        DM(VDK_ISR_HISTORY_SV_R8?) = R8;				\
		DM(VDK_ISR_HISTORY_SV_R12?) = R12;				\
        R4 = enum_;										\
		CALL _VDK_ASM_History (DB);						\
		R8 = value_;									\
		R12 = VDK_INTERRUPT_LEVEL_;						\
		PUSH PCSTK;										\
		PCSTK = DM(VDK_ISR_HISTORY_SV_PCTOP?);			\
        R1 = DM(VDK_ISR_HISTORY_SV_R1?);				\
        R4 = DM(VDK_ISR_HISTORY_SV_R4?);				\
        R8 = DM(VDK_ISR_HISTORY_SV_R8?);				\
		R12 = DM(VDK_ISR_HISTORY_SV_R12?)

#endif /* !__2136x__ */
#endif /* !VDK_SAVE_RESTORE_40_BITS */

// BEGIN_INTERNALS_STRIP

// We don't need to provide a 2136x-specific version of this macro,
// because it is only ever used from within the "body" subroutines
// if the ISR APIS, and at those places we know that the top PC
// stack entry is the CALL return address and not an interrupt
// return address.
//
// This also means that the full 40 bits of R1 and R4 will have been
// saved by the ISR macro wrapper, so we can get away with 32-bit stores
// here, and only need to do all 40 bits for R8 and R12. However, since
// the uses of this macro get compiled into the VDK libraries the 40-bit
// saving can't be conditional, so we always do it for R8 and R12
//
// Interrupts must be disabled when using this macro.
//
#if defined (WA_15000011)
// Work around anomaly 15000011. We cannot use call(db)
#define VDK_ISR_INTERNAL_HISTORY_LOG_(enum_, value_reg_)\
.extern _VDK_ASM_History;                               \
.extern VDK_INTERNAL_HISTORY_SV_PCTOP;                  \
.extern VDK_INTERNAL_HISTORY_SV_R1;                     \
.extern VDK_INTERNAL_HISTORY_SV_R4;                     \
.extern VDK_INTERNAL_HISTORY_SV_R8L;                    \
.extern VDK_INTERNAL_HISTORY_SV_R8H;                    \
.extern VDK_INTERNAL_HISTORY_SV_PX1;                    \
.extern VDK_INTERNAL_HISTORY_SV_PX2;                    \
        DM(VDK_INTERNAL_HISTORY_SV_PCTOP) = PCSTK;      \
        POP PCSTK;                                      \
        DM(VDK_INTERNAL_HISTORY_SV_R1) = R1;            \
        DM(VDK_INTERNAL_HISTORY_SV_R4) = R4;            \
        DM(VDK_INTERNAL_HISTORY_SV_PX1) = PX1;          \
        DM(VDK_INTERNAL_HISTORY_SV_PX2) = PX2;          \
        PX = R8;                                        \
        DM(VDK_INTERNAL_HISTORY_SV_R8L) = PX1;          \
        DM(VDK_INTERNAL_HISTORY_SV_R8H) = PX2;          \
        PX = R12;                                       \
        R8 = value_reg_;                                \
        R4 = enum_;                                     \
        R12 = VDK_INTERRUPT_LEVEL_;                     \
        CALL _VDK_ASM_History ;                         \
        PUSH PCSTK;                                     \
        PCSTK = DM(VDK_INTERNAL_HISTORY_SV_PCTOP);      \
        R1 = DM(VDK_INTERNAL_HISTORY_SV_R1);            \
        R4 = DM(VDK_INTERNAL_HISTORY_SV_R4);            \
        R12 = PX;                                       \
        PX1 = DM(VDK_INTERNAL_HISTORY_SV_R8L);          \
        PX2 = DM(VDK_INTERNAL_HISTORY_SV_R8H);          \
        R8 = PX;                                        \
        PX1 = DM(VDK_INTERNAL_HISTORY_SV_PX1);          \
        PX2 = DM(VDK_INTERNAL_HISTORY_SV_PX2)

#else /* ! WA_15000011 */
#define VDK_ISR_INTERNAL_HISTORY_LOG_(enum_, value_reg_)\
.extern _VDK_ASM_History;								\
.extern VDK_INTERNAL_HISTORY_SV_PCTOP;					\
.extern VDK_INTERNAL_HISTORY_SV_R1;						\
.extern VDK_INTERNAL_HISTORY_SV_R4;						\
.extern VDK_INTERNAL_HISTORY_SV_R8L;					\
.extern VDK_INTERNAL_HISTORY_SV_R8H;					\
.extern VDK_INTERNAL_HISTORY_SV_PX1;					\
.extern VDK_INTERNAL_HISTORY_SV_PX2;					\
		DM(VDK_INTERNAL_HISTORY_SV_PCTOP) = PCSTK;		\
		POP PCSTK;										\
        DM(VDK_INTERNAL_HISTORY_SV_R1) = R1;			\
        DM(VDK_INTERNAL_HISTORY_SV_R4) = R4;			\
        DM(VDK_INTERNAL_HISTORY_SV_PX1) = PX1;			\
        DM(VDK_INTERNAL_HISTORY_SV_PX2) = PX2;			\
        PX = R8;                                        \
		DM(VDK_INTERNAL_HISTORY_SV_R8L) = PX1;			\
		DM(VDK_INTERNAL_HISTORY_SV_R8H) = PX2;			\
        PX = R12;                                       \
        R8 = value_reg_;								\
		CALL _VDK_ASM_History (DB);						\
		R4 = enum_;										\
		R12 = VDK_INTERRUPT_LEVEL_;						\
		PUSH PCSTK;										\
		PCSTK = DM(VDK_INTERNAL_HISTORY_SV_PCTOP);		\
        R1 = DM(VDK_INTERNAL_HISTORY_SV_R1);			\
        R4 = DM(VDK_INTERNAL_HISTORY_SV_R4);			\
        R12 = PX;                                       \
        PX1 = DM(VDK_INTERNAL_HISTORY_SV_R8L);			\
        PX2 = DM(VDK_INTERNAL_HISTORY_SV_R8H);			\
        R8 = PX;                                        \
        PX1 = DM(VDK_INTERNAL_HISTORY_SV_PX1);			\
		PX2 = DM(VDK_INTERNAL_HISTORY_SV_PX2)
#endif /* ! WA_15000011 */

// END_INTERNALS_STRIP
#else /* VDK_INSTRUMENTATION_LEVEL_ != 2 */

#define VDK_ISR_LOG_HISTORY_(enum_, value_, threadID_)

// Note that VDK_ISR_INTERNAL_HISTORY_LOG_() is only ever called in
// the VDK source code, from a section that is only compiled-in for
// instrumented builds. This means that there is no need to provide
// an empty version of that macro here.

#endif /* VDK_INSTRUMENTATION_LEVEL_ != 2 */

#endif /* ECC */



#endif /* VDK_ISR_API_H_ */




