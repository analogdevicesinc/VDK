/* =============================================================================
 *
 *  Description: This is an Assembly implementation for Thread %Name%
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/

/* Get access to any of the VDK features & datatypes used */
#include "%HeaderFile%"

#include <ASM_SPRT.H>

.file_attr OS_Component=Threads;
.file_attr Threads;

.section/dm/doubleany seg_dmda;

/* place any static local data here */

#if defined (__214xx__) && defined (__SHORT_WORD_CODE__)
.section/sw/doubleany seg_swco;
#else
.section/pm/doubleany seg_pmco;
#endif


/*************************************************************************
 * void %Name%_RunFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_RunFunction;
_%Name%_RunFunction:
    leaf_entry;
    /* Put the thread's "main" Initialization HERE */

_%Name%_RunFunction_Main_Loop:
        /* Put the thread's "main" body HERE */

        /* Jump out of "_%Name%_RunFunction_Main_Loop" when thread done */
    jump _%Name%_RunFunction_Main_Loop;

    /* Put the thread's exit from "main" HERE */

    /* A thread is automatically Destroyed when it exits its run function */

    leaf_exit;
._%Name%_RunFunction.end:

/*************************************************************************
 * int %Name%_ErrorFunction(void **inPtr)
 */
 
.extern _VDK_GetThreadID, _VDK_DestroyThread;
.GLOBAL _%Name%_ErrorFunction;
_%Name%_ErrorFunction:
    leaf_entry;

        /*
         *  The default ErrorHandler kills the thread, if you want this function 
         *  to be called, you need to comment the following line in the %Name%.h file
         *  "return (VDK::Thread::ErrorHandler());"
         *
         *  And uncomment the following line below in the %Name%.h file
         *  "return %Name%_ErrorFunction(&m_DataPtr);"
         */

		/*
		 * Make sure to return an "int" value
		 *
		 */
	cjump _VDK_GetThreadID(db);
	dm(i7,m7)=r2;
	dm(i7,m7)=pc;
	r8=0;
	r4=r0;
	cjump _VDK_DestroyThread(db);
	dm(i7,m7)=r2;
	dm(i7,m7)=pc;
	r0=0;
		 		
    leaf_exit;
._%Name%_ErrorFunction.end:


/*************************************************************************
 * void %Name%_InitFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_InitFunction;
_%Name%_InitFunction:
    leaf_entry;

    /* Put code to be executed when this thread has just been created HERE */

    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    leaf_exit;
._%Name%_InitFunction.end:

/*************************************************************************
 * void %Name%_DestroyFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_DestroyFunction;
_%Name%_DestroyFunction:
    leaf_entry;

    /* Put code to be executed when this thread is destroyed HERE */

    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    leaf_exit;
._%Name%_DestroyFunction.end:

/* ========================================================================== */
