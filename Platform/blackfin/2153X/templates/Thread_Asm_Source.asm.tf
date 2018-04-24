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

.file_attr OS_Component=Threads;
.file_attr Threads;

.section/data/doubleany data1;

/* place any static local data here */

.section/code/doubleany program;


/*************************************************************************
 * void %Name%_RunFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_RunFunction;
_%Name%_RunFunction:
    /* Add any necessary function prologue HERE */

    /* Put the thread's "main" Initialization HERE */

_%Name%_RunFunction_Main_Loop:
        /* Put the thread's "main" body HERE */

        /* Jump out of "_%Name%_RunFunction_Main_Loop" when thread done */
    jump _%Name%_RunFunction_Main_Loop;

    /* Put the thread's exit from "main" HERE */

    /* A thread is automatically Destroyed when it exits its run function */

    /* Add any necessary function epilogue HERE */
    RTS;
._%Name%_RunFunction.end:

/*************************************************************************
 * int %Name%_ErrorFunction(void **inPtr)
 */
 
.extern _VDK_GetThreadID, _VDK_DestroyThread;
.GLOBAL _%Name%_ErrorFunction;
_%Name%_ErrorFunction:
    /* Add any necessary function prologue HERE */

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
		 		
	CALL.X _VDK_GetThreadID;
	R1 = 0 ;
	CALL.X _VDK_DestroyThread;
	R0 = 0 ;
    /* Add any necessary function epilogue HERE */
    RTS;
._%Name%_ErrorFunction.end:


/*************************************************************************
 * void %Name%_InitFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_InitFunction;
_%Name%_InitFunction:
    /* Add any necessary function prologue HERE */

    /* Put code to be executed when this thread has just been created HERE */

    /* This routine does NOT run in new thread's context.  Any non-static thread
     *   initialization should be performed at the beginning of "Run()."
     */

    /* Add any necessary function epilogue HERE */
    RTS;
._%Name%_InitFunction.end:

/*************************************************************************
 * void %Name%_DestroyFunction(void **inPtr)
 */
 
.GLOBAL _%Name%_DestroyFunction;
_%Name%_DestroyFunction:
    /* Add any necessary function prologue HERE */

    /* Put code to be executed when this thread is destroyed HERE */

    /* This routine does NOT run in the thread's context.  Any VDK API calls
     *   should be performed at the end of "Run()."
     */

    /* Add any necessary function epilogue HERE */
    RTS;
._%Name%_DestroyFunction.end:

/* ========================================================================== */
