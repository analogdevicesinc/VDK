/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* =============================================================================
 *
 *   $RCSfile$
 *
 *   Description: The implementation of the public CreateThreadEx2 function
 *
 *   Last modified $Date$
 *	 Last modified by $Author$
 *   $Revision$
 *   $Source$ 
 *
 * -----------------------------------------------------------------------------
 *  Comments: The implementation of CreateThreadEx2() API function 
 * -----------------------------------------------------------------------------
 * ===========================================================================*/

#include "TMK.h"
#include <stdlib.h>

#include "VDK_API.h"
#include "VDK_History.h"
#include "VDK_Macros.h"
#include "VDK_Thread.h"
#include "ThreadTable.h"
#pragma file_attr("OS_Component=Threads")
#pragma file_attr("Threads")
#pragma file_attr("FuncName=CreateThreadEx2")
#pragma file_attr("FuncName=_CreateThreadEx2__3VDKFPQ2_3VDK19ThreadCreationBlockUi")

#define BITISSET(fields,bit) (fields & bit)

#ifndef __ADSPTS__
namespace VDK
{
	extern ThreadID CreateThreadInternal( ThreadCreationBlock* );

	extern ThreadTemplate  			g_ThreadTemplates[];    	 // in vdk.cpp
	extern ThreadTable				g_ThreadTable;
	extern unsigned int 	        kNumThreadTemplates;		 
	extern unsigned int             g_localNode;

    unsigned int GetContextRecordSize(void)
    {
        return TMK_QueryContextRecordSize();
    };

	// This is the CreateThreadEx2() API function, this differs from the old
	// CreateThreadEx function in that it can modify more thread attributes
	// and it takes an additional argument which specifies which thread
	// attributes should be replaced with user chosen values.
	//


	ThreadID
	CreateThreadEx2( ThreadCreationBlock *inOutTCB, int fieldsRequired )
	{
		// Check if the function is being called from an ISR
		if NERRCHK(IS_ISR_LEVEL ()) 
			KernelPanic(kISRError, kAPIUsedfromISR, IS_USER_ISR_LEVEL());	

		// Copy the thread type ID from the TCB
	    const ThreadType inTemplateID = inOutTCB->template_id;

#if (VDK_INSTRUMENTATION_LEVEL_>0)
		// The error cases that we are checking for are if the user has specified...
		// 1. A thread template ID that is out of range.
		if (BITISSET(fieldsRequired, kSetThreadTemplateID) && (inTemplateID >= kNumThreadTemplates))
		{
			DispatchThreadError(kUnknownThreadType, inTemplateID);
	        return static_cast<ThreadID> (UINT_MAX);
		}

		// 2. A dynamic thread type with NULL for pTemplate.
		if (BITISSET(fieldsRequired, kSetThreadTemplatePointer) && (NULL == inOutTCB->pTemplate))
		{
			DispatchThreadError(kInvalidPointer, (unsigned int) inOutTCB->pTemplate);
	        return static_cast<ThreadID> (UINT_MAX);
		}

		// 3. Both a thread template ID and template pointer
		// 4. Neither a thread template ID nor a template pointer.
		// 5. Both a thread stack pointer and a stack heap.
		// 6. A thread stack pointer without a stack size.
		if ((BITISSET(fieldsRequired, kSetThreadTemplateID) && BITISSET(fieldsRequired, kSetThreadTemplatePointer))		||
			(!BITISSET(fieldsRequired, kSetThreadTemplateID) && !BITISSET(fieldsRequired, kSetThreadTemplatePointer))	||
			(BITISSET(fieldsRequired, kSetThreadStackPointer) && !BITISSET(fieldsRequired, kSetThreadStackSize)))
		{
			DispatchThreadError(kIncompatibleArguments, fieldsRequired);
	        return static_cast<ThreadID> (UINT_MAX);
		}
#endif

		// Create internal version of the TCB
        ThreadCreationBlock internalTCB;

		// If the user has specified a ThreadType, then copy this into the
		// internal ThreadCreationBlock. Otherwise use -1 (kDynamicThreadType)
		if (BITISSET(fieldsRequired, kSetThreadTemplateID))
			internalTCB.template_id = inOutTCB->template_id;
		else
			internalTCB.template_id = (ThreadType) -1;

		// Obtain the thread template pointer, from the input TCB if the user
		// has provided their own or from the Thread Template table if not.
		if (BITISSET(fieldsRequired, kSetThreadTemplatePointer))
			internalTCB.pTemplate = inOutTCB->pTemplate;
		else
			internalTCB.pTemplate = &g_ThreadTemplates[inTemplateID];

		// We need to ensure that any fields that have zero in the bitfield
		// are not replaced by their TCB counterparts. So we copy the values
		// from the template for any fields that have 0 in the bitfield.
		if (BITISSET(fieldsRequired, kSetThreadStackSize))
			internalTCB.thread_stack_size = inOutTCB->thread_stack_size;
		else
			internalTCB.thread_stack_size = internalTCB.pTemplate->stackSize;

		if (BITISSET(fieldsRequired, kSetThreadPriority))
			internalTCB.thread_priority = inOutTCB->thread_priority;
		else
			internalTCB.thread_priority = internalTCB.pTemplate->priority;

		// The two fields that are pointers can just be set to 0, as they do 
		// not exist in the thread templates and 0 is never a valid pointer
		if (BITISSET(fieldsRequired, kSetUserDataPointer))
			internalTCB.user_data_ptr = inOutTCB->user_data_ptr;
		else
			internalTCB.user_data_ptr = 0;

		if (BITISSET(fieldsRequired, kSetThreadStackPointer))
		{
			internalTCB.stack_pointer = inOutTCB->stack_pointer;

#if (VDK_INSTRUMENTATION_LEVEL_>0) 
#ifdef __ADSPBLACKFIN__
			unsigned int stackPtr = (unsigned int) internalTCB.stack_pointer;
			// Check the user stack pointer is correctly aligned...
			if (0 != (stackPtr & 3))
			{
				// If not dispatch an error
				DispatchThreadError(kInvalidPointer, (int) inOutTCB->stack_pointer);
				return static_cast<ThreadID> (UINT_MAX);
			}
#endif	// #ifdef __ADSPBLACKFIN__
#endif	// #if (VDK_INSTRUMENTATION_LEVEL_>0) 
		}
		else
		{
			internalTCB.stack_pointer = NULL;
		}

		// Call CreateThreadInternal and get the thread ID into the thread_id
		// output field of the user's TCB.
		inOutTCB->thread_id = CreateThreadInternal(&internalTCB);

		// Return the Thread ID.
		return inOutTCB->thread_id;
	}
}
#endif
