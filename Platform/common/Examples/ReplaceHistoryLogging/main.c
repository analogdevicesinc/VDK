#include "VDK.h"

/* declaration of the function that will replace the default history
   mechanism */
extern void CustomHistoryLog(VDK_Ticks, VDK_HistoryEnum, 
									const int, const VDK_ThreadID);

int main(void) 
{
	/* Replace the default history mechanism */

    VDK_ReplaceHistorySubroutine(CustomHistoryLog);
    
    /** Initialize VDK. No VDK APIs other than ReplaceHistorySubroutine
       can be called directly or indirectly before this point  **/
	VDK_Initialize();
	
	/** Run VDK. No VDK APIs that can trigger a context switch can be
	   called directly or indirectly before this point  **/
    VDK_Run();
}
