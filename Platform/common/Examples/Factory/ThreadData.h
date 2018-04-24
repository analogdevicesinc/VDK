/* =============================================================================
 *
 *  Description: This is a C implementation for Data Structure threadData
 *
 * -----------------------------------------------------------------------------
 *  Comments: This data structure is intended to store a number of paramaters 
 *            used to initialise some local variables in the RampGen thread 
 *            during its creation.
 *
 * ===========================================================================*/

typedef struct threadData
{
	SemaphoreID		semaphoreToPendOn;
	unsigned int	periodOfGenerator;
	unsigned int	amplitudeStep;
} threadData;
