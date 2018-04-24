/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/** @file Globals.c
 */

#if defined(__ADSPBLACKFIN__)
#include <sys/exception.h>
#elif defined(__ADSPTS20x__)
#include <defts201.h>
#elif defined(__ADSPTS101__)
#include <defts101.h>
#elif defined(__ADSP21000__)
#else
#error Unknown processor family
#endif

#include "Trinity.h"

#pragma file_attr("Startup")
#pragma file_attr("OS_Internals")


/// @internal
/// @brief Holds (most of) the microkernel's internal state
///
struct tmk_Globals tmk =
{
    0,                                                              // masterState
    TMK_INIT_RESCHEDULE_INTERRUPT_LOCK(TMK_KERNEL_LOCK_PRECEDENCE), // masterLock
    NULL,                                                           // pCurrentThread
    1,                                                              // schedulerRegionCount
    {                                                               // vpfCtxtSwitch
	tmk_SwitchContext_Runtime,                                  // TMK_RUNTIMEHOOK
	tmk_SwitchContext_ISR,                                      // TMK_INTERRUPTHOOK
	tmk_SwitchContext_FirstThread,                              // TMK_FIRSTTIMEHOOK
	tmk_SwitchContext_KillThread                                // TMK_KILLTHREADHOOK
    },
    tmk_Dispatcher
};

/** @internal
 *  @brief The reschedule interrupt handler.
 *
 * Implemented in assembly, in file Switch.asm.
 */
extern void tmk_RescheduleISR(int, int, int);

/** @internal
 *  @brief The exception handler.
 *
 * Implemented in assembly, in file Switch.asm.
 */
extern void tmk_ExceptionHandler(int, int, int);

/** @internal
 *  @brief The exception handler.
 *
 * Implemented in assembly, in file Timer.asm.
 */
extern void tmk_TimerISR(int, int, int);

#if defined (__ADSPBLACKFIN__) && defined (BUILD_ROM_IMAGE)
void tmk_register_handler(interrupt_kind kind, ex_handler_fn fn){
  volatile ex_handler_fn *evt =
    (volatile ex_handler_fn *)EX_EVENT_VECTOR_TABLE;
  if (ik_emulation <= kind && kind <= ik_ivg15) {
      evt[kind] = fn;
      unsigned int bit = 1<<kind;
      unsigned int mask = __builtin_cli();
      mask |= bit;
      __builtin_sti(mask);
  }
}
#endif

/** Initialise the internals of the microkernel.

Initialises the various internal components, registers
handlers for the reschedule interrupt and for exceptions.
Sets up the initial locking state (master kernel lock held).
*/
void
TMK_Initialize()
{
    // Install the interrupt service routines for the reschedule
    // interrupt, the timer interrupt, and the software exceptions.
    //
#if defined(__ADSPBLACKFIN__)
    // the macrokernel needs to set up the timer interrupt.
    // Otherwise the user might select a different timer interrupt and
    // use the timer for something else and the microkernel will overwrite
    // the setting
    //register_handler(ik_timer,     tmk_TimerISR);
#if defined (BUILD_ROM_IMAGE)
    tmk_register_handler(ik_ivg14,     tmk_RescheduleISR);
    tmk_register_handler(ik_exception, tmk_ExceptionHandler);
#else
    register_handler(ik_ivg14,     tmk_RescheduleISR);
    register_handler(ik_exception, tmk_ExceptionHandler);
#endif /* BUILD_ROM_IMAGE */

#elif defined(__ADSPTS20x__)
#define VECTOR(x) (*((void (**)(int, int, int))(x)))
    // the macrokernel needs to set up the kernel interrupt
    // timer interrupt and exception to the correct ISRs.
    // This code did not actually set RescheduleISR or Timer
    //VECTOR(IVKERNEL_LOC)   = tmk_RescheduleISR;
    //VECTOR(IVTIMER1HP_LOC) = tmk_TimerISR;
    void (*pExHdlr)(int,int,int) = tmk_ExceptionHandler;;
    asm("IVSW = %0;;" : : "y"(pExHdlr));
#undef VECTOR
#elif defined(__ADSPTS101__)
#define VECTOR(x) (*((void (**)(int, int, int))(x)))

    // the macrokernel needs to set up the timer interrupt to the correct ISRs.
    // Otherwise the user might select a different timer interrupt and
    // use the timer for something else and the microkernel will overwrite
    // the setting
    void (*pReHdlr)(int,int,int) = tmk_RescheduleISR;
    asm("IVTIMER0LP = %0;;" : : "y"(pReHdlr));
    //void (*pTiHdlr)(int,int,int) = tmk_TimerISR;
    //asm("IVTIMER1HP = %0;;" : : "y"(pTiHdlr));

    void (*pExHdlr)(int,int,int) = tmk_ExceptionHandler;;
    asm("IVSW = %0;;" : : "y"(pExHdlr));

    //We initialize timer0 to a minimum count so that once enabled
    // expires almost immediately
    asm("TMRIN0H = 0;; TMRIN0L = 2;;");
#elif defined(__ADSP21000__)
#else
#error Unknown processor family
#endif
}


