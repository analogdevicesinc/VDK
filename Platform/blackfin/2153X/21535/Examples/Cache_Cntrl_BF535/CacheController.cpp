/* =============================================================================
 *
 *  Description: This is a C++ implementation of a VDK Device Driver for CacheController
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

#include "VDK.h"

#ifdef VDK_INCLUDE_IO_
#include "CacheController.h"

/* globals */

/* 
 * This flag controls the initialization of the
 * cache. It can be changed using the debugger to
 * allow investigation of performance and behaviour
 * with and without caching, without the need for
 * recompilation.
 *
 * The flag should be set false during algorithm
 * development to avoid cache-induced problems
 * with breakpoints, single-stepping and file IO.
 */
bool use_cache = true;

/* externs */
extern "C" void init_cache(); /* in cache_init.asm */


/******************************************************************************
 * Dispatch Function for the CacheController Device Driver
 *
 * This function is called in a critical region when a device
 * is being Initialized or Activated.
 */
 
void*
CacheController::DispatchFunction(VDK::DispatchID inCode, VDK::DispatchUnion &inUnion)
{
    switch(inCode)
    {
        case VDK::kIO_Init:      
                if (use_cache)
					init_cache();
            break;
    }
    return 0;
}

#endif /* VDK_INCLUDE_IO_ */

/* ========================================================================== */
